#include "MegaSynth.h"

#include "IPlug_include_in_plug_src.h" // has to be after MegaSynth.h

#include "IControls.h"
#include "IPlugMidi.h"
// #include "Oscillator.h"
// #include "MIDIReceiver.h"

#include <algorithm>;

using std::copy;

using iplug::InstanceInfo;
using iplug::Plugin;
using iplug::MakeConfig;
using iplug::sample;
using iplug::IMidiMsg;

using iplug::igraphics::IGraphics;
using iplug::igraphics::EUIResizerMode;
using iplug::igraphics::ITextControl;
using iplug::igraphics::IText;
using iplug::igraphics::IVKnobControl;
using iplug::igraphics::IRECT;
using iplug::igraphics::MakeGraphics;

MegaSynth::MegaSynth(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  createParams();

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS,
      GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    createGraphics(pGraphics);
  };
#endif

  this->midiReceiver.noteOn.Connect(this, &MegaSynth::onNoteOn);
  this->midiReceiver.noteOff.Connect(this, &MegaSynth::onNoteOff);

  this->envelopeGenerator.beganEnvelopeCycle.Connect(this, &MegaSynth::onBeganEnvelopeCycle);
  this->envelopeGenerator.finishedEnvelopeCycle.Connect(this, &MegaSynth::onFinishedEnvelopeCycle);

  this->lfo.setWaveform(Oscillator::Waveform::TRIANGLE);
  this->lfo.setFrequency(6.0);
  this->lfo.setMuted(false);
}

void MegaSynth::createParams() {
  const double step = 0.001; // parameter regulation step

  GetParam(EParams::osc1Waveform)->InitEnum("Osc 1 Waveform", Oscillator::Waveform::SINE, Oscillator::Waveform::kNumberOfWaveforms, "", 0, "", "Sine", "Saw", "Square", "Triangle");
  GetParam(EParams::osc2Waveform)->InitEnum("Osc 2 Waveform", Oscillator::Waveform::SINE, Oscillator::Waveform::kNumberOfWaveforms, "", 0, "", "Sine", "Saw", "Square", "Triangle");
  GetParam(EParams::osc1PitchMod)->InitDouble("Osc 1 Pitch Mod", 0.0, 0.0, 1.0, step);
  GetParam(EParams::osc2PitchMod)->InitDouble("Osc 2 Pitch Mod", 0.0, 0.0, 1.0, step);
  GetParam(EParams::osc2PitchMod)->InitDouble("Osc Mix", 0.5, 0.0, 1.0, step);

  GetParam(EParams::attack)->InitDouble("Attack", 0.01, 0.01, 10.0, 0.001, "s", 0, "", IParam::ShapePowCurve(3));
  GetParam(EParams::decay)->InitDouble("Decay", 0.5, 0.01, 15.0, 0.001, "s", 0, "", IParam::ShapePowCurve(3));
  GetParam(EParams::sustain)->InitDouble("Sustain", 0.1, 0.001, 1.0, 0.001, "", 0, "", IParam::ShapePowCurve(2));
  GetParam(EParams::release)->InitDouble("Release", 1.0, 0.001, 15.0, 0.001, "s", 0, "", IParam::ShapePowCurve(3));

  GetParam(EParams::filterMode)->InitEnum("Filter mode", Filter::Mode::LOW_PASS, Filter::Mode::kNumModes, "", 0, "", "Low Pass", "High Pass", "Band Pass");

  GetParam(EParams::filterCutoff)->InitDouble("Filter Cutoff", 0.99, 0.01, 0.99, 0.001, "", 0, "", IParam::ShapePowCurve(2));
  GetParam(EParams::filterResonance)->InitDouble("Filter Resonance", 0.01, 0.01, 1.0, 0.001);

  GetParam(EParams::filterAttack)->InitDouble("Filter Attack", 0.01, 0.01, 10.0, 0.001, "s", 0, "", IParam::ShapePowCurve(3));
  GetParam(EParams::filterDecay)->InitDouble("Filter Decay", 0.5, 0.01, 15.0, 0.001, "s", 0, "", IParam::ShapePowCurve(3));
  GetParam(EParams::filterSustain)->InitDouble("Filter Sustain", 0.1, 0.001, 1.0, 0.001, "", 0, "", IParam::ShapePowCurve(2));
  GetParam(EParams::filterRelease)->InitDouble("Filter Release", 1.0, 0.001, 15.0, 0.001, "", 0, "", IParam::ShapePowCurve(3));

  GetParam(EParams::filterEnvelopeAmount)->InitDouble("Filter Env Amount", 0.0, -1.0, 1.0, 0.001);
  GetParam(EParams::filterLfoAmount)->InitDouble("Filter LFO Amount", 0.0, 0.0, 1.0, step);

  GetParam(EParams::lfoWaveform)->InitEnum("LFO Waveform", Oscillator::Waveform::TRIANGLE, Oscillator::Waveform::kNumberOfWaveforms, "", 0, "", "Sine", "Saw", "Square", "Triangle");
  GetParam(EParams::lfoRate)->InitDouble("LFO Rate", 6.0, 0.01, 30.0, step, "Hz");
}

void MegaSynth::createGraphics(IGraphics* pGraphics) {
  pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
  pGraphics->AttachSVGBackground(BACKGROUND_FN);

  pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
  const IRECT b = pGraphics->GetBounds();

  this->virtualKeyboard = new IVKeyboardControl(b.GetFromBottom(75), this->virtualKeyboardMinimumNoteNumber, this->virtualKeyboardMinimumNoteNumber + 5 * 12);
  pGraphics->AttachControl(this->virtualKeyboard);

  // waveform control & its label
  auto waveformsBitmap = pGraphics->LoadBitmap(WAVEFORMS_FN, 4);
  auto* waveformSwitch = new IBSwitchControl(28, 53, waveformsBitmap, EParams::osc1Waveform);
  pGraphics->AttachControl(waveformSwitch);

  auto knobStyle = IVStyle::IVStyle(false, false);

  // osc 1 pitch mod knob
  auto osc1PitchModKnobRect = waveformSwitch->GetRECT().GetHShifted(53).GetScaledAboutCentre(1.5);
  auto* osc1PitchModKnob = new IVKnobControl(osc1PitchModKnobRect, 
      EParams::osc1PitchMod, " ", knobStyle);
  pGraphics->AttachControl(osc1PitchModKnob);

  // osc mix knob
  auto oscMixKnobRect = osc1PitchModKnobRect.GetHShifted(53);
  auto* oscMixKnob = new IVKnobControl(oscMixKnobRect, 
      EParams::oscMix, " ", knobStyle);
  pGraphics->AttachControl(oscMixKnob);

  // osc2 pitch mod knob
  auto osc2PitchKnobRect = oscMixKnobRect.GetHShifted(53);
  auto *osc2PitchKnob = new IVKnobControl(osc2PitchKnobRect, 
      EParams::osc2PitchMod, " ", knobStyle);
  pGraphics->AttachControl(osc2PitchKnob);

  // osc 2 waveform
  auto osc2WaveformSwitchRect = osc2PitchKnobRect.GetHShifted(53).GetScaledAboutCentre(1.0 / 1.5);
  auto* osc2WaveformSwitch = new IBSwitchControl(osc2WaveformSwitchRect,
      waveformsBitmap, EParams::osc2Waveform);
  pGraphics->AttachControl(osc2WaveformSwitch);

  // attack knob & its label
  auto attackKnobPos = waveformSwitch->GetRECT().GetHShifted(290).GetScaledAboutCentre(1.5);
  auto* attackKnob = new IVKnobControl(attackKnobPos, EParams::attack, " ", knobStyle);
  pGraphics->AttachControl(attackKnob);

  // decay
  auto decayKnobRect = attackKnobPos.GetHShifted(60);
  auto* decayKnob = new IVKnobControl(decayKnobRect, EParams::decay, " ", knobStyle);
  pGraphics->AttachControl(decayKnob);

  // sustian
  auto sustainKnobRect = decayKnobRect.GetHShifted(60);
  auto* sustainKnob = new IVKnobControl(sustainKnobRect, EParams::sustain, " ", knobStyle);
  pGraphics->AttachControl(sustainKnob);

  // release
  auto* releaseKnob = new IVKnobControl(sustainKnobRect.GetHShifted(60), EParams::release, " ", knobStyle);
  pGraphics->AttachControl(releaseKnob);

  // filter mode & its label
  auto filterModesBitmap = pGraphics->LoadBitmap(FILTER_MODES_FN, 3);
  auto* filterModeSwitch = new IBSwitchControl(28, 178, filterModesBitmap, EParams::filterMode);
  pGraphics->AttachControl(filterModeSwitch);

  // cutoff
  auto cutoffKnobRect = filterModeSwitch->GetRECT().GetHShifted(53).GetScaledAboutCentre(1.5);
  auto* cutoffKnob = new IVKnobControl(cutoffKnobRect, EParams::filterCutoff, "cutoff", knobStyle);
  pGraphics->AttachControl(cutoffKnob);

  // resonance
  auto resonanceKnobRect = cutoffKnobRect.GetHShifted(53);
  auto* resonanceKnob = new IVKnobControl(resonanceKnobRect, EParams::filterResonance, "resonance", knobStyle);
  pGraphics->AttachControl(resonanceKnob);

  // filter lfo amount
  auto filterLfoAmtKnobRect = resonanceKnobRect.GetHShifted(53);
  pGraphics->AttachControl(new IVKnobControl(filterLfoAmtKnobRect, 
      EParams::filterLfoAmount, "fltr lfo amt", knobStyle));

  // filter attack
  auto filterAttackKnobRect = attackKnobPos.GetVShifted(125);
  pGraphics->AttachControl(new IVKnobControl(filterAttackKnobRect, EParams::filterAttack, "fltr attack", knobStyle));

  // filter decay
  auto filterDecayKnobPos = filterAttackKnobRect.GetHShifted(60);
  pGraphics->AttachControl(new IVKnobControl(filterDecayKnobPos, EParams::filterDecay, "fltr decay", knobStyle));

  // filter sustain
  auto filterSustainKnobPos = filterDecayKnobPos.GetHShifted(60);
  pGraphics->AttachControl(new IVKnobControl(filterSustainKnobPos, EParams::filterSustain, "fltr sustain", knobStyle));

  // filter release
  auto filterReleaseKnobPos = filterSustainKnobPos.GetHShifted(60);
  pGraphics->AttachControl(new IVKnobControl(filterReleaseKnobPos, EParams::filterRelease, "fltr release", knobStyle));

  // filter envelope amount
  auto filterEnvAmtKnobPos = filterLfoAmtKnobRect.GetHShifted(53);
  pGraphics->AttachControl(new IVKnobControl(filterEnvAmtKnobPos, EParams::filterEnvelopeAmount, "fltr env amt", knobStyle));

  // lfo waveform switch
  auto* lfoWaveSwitch = new IBSwitchControl(28, 300, waveformsBitmap, EParams::lfoWaveform);
  pGraphics->AttachControl(lfoWaveSwitch);
  
  // lfo rate knob
  auto lfoRateKnobPos = lfoWaveSwitch->GetRECT().GetHShifted(60).GetScaledAboutCentre(1.5);
  pGraphics->AttachControl(new IVKnobControl(lfoRateKnobPos, 
      EParams::lfoRate, "LFO Rate", knobStyle));
}

#if IPLUG_DSP
void MegaSynth::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  sample* leftOutput = outputs[0];
  sample* rightOutput = outputs[1];

  for (int i = 0; i < nFrames; i++)
  {
    this->midiReceiver.advance();
    int velocity = this->midiReceiver.getLastVelocity();
    double lfoFilterModulation = this->lfo.nextSample() * this->lfoFilterModAmount;

    this->osciallator.setFrequency(this->midiReceiver.getLastFrequency());

    this->filter.setCutoffMod((this->filterEnvelope.nextSample() * this->filterEnvelopeAmount) + lfoFilterModulation);
    leftOutput[i] = this->filter.process(
      this->osciallator.nextSample() * this->envelopeGenerator.nextSample() * velocity / 127.0
    );
  }

  copy(leftOutput, leftOutput + nFrames, rightOutput);

  this->midiReceiver.flush(nFrames);
}

void MegaSynth::OnReset() {
  auto sampleRate = GetSampleRate();
  this->osciallator.setSampleRate(sampleRate);
  this->envelopeGenerator.setSampleRate(sampleRate);
  this->filterEnvelope.setSampleRate(sampleRate);
  this->lfo.setSampleRate(sampleRate);
}

void MegaSynth::OnParamChange(int paramId) {
  // MUTEX LOCK?

  using EParams::osc1Waveform;
  using EParams::attack;
  using EParams::decay;
  using EParams::sustain;
  using EParams::release;

  switch (paramId)
  {
  case osc1Waveform:
    this->osciallator.setWaveform(static_cast<Oscillator::Waveform>(GetParam(osc1Waveform)->Int()));
    break;
  case attack:
  case decay:
  case sustain:
  case release:
    this->envelopeGenerator.setStageValue(static_cast<EnvelopeGenerator::Stage>(paramId), GetParam(paramId)->Value());
    break;
  case filterCutoff:
    this->filter.setCutoff(GetParam(filterCutoff)->Value());
    break;
  case filterResonance:
    this->filter.setResonance(GetParam(filterResonance)->Value());
    break;
  case filterMode:
    this->filter.setFilterMode(static_cast<Filter::Mode>(GetParam(filterMode)->Int()));
    break;
  case filterAttack:
    this->filterEnvelope.setStageValue(
      EnvelopeGenerator::Stage::ATTACK, GetParam(paramId)->Value());
    break;
  case filterDecay:
    this->filterEnvelope.setStageValue(
      EnvelopeGenerator::Stage::DECAY, GetParam(paramId)->Value());
    break;
  case filterSustain:
    this->filterEnvelope.setStageValue(
      EnvelopeGenerator::Stage::SUSTAIN, GetParam(paramId)->Value());
    break;
  case filterRelease:
    this->filterEnvelope.setStageValue(
      EnvelopeGenerator::Stage::RELEASE, GetParam(paramId)->Value());
    break;
  case EParams::filterEnvelopeAmount:
    this->filterEnvelopeAmount = GetParam(paramId)->Value();
    break;
  default:
    break;
  }
}

void MegaSynth::ProcessMidiMsg(const IMidiMsg& msg) {
  this->midiReceiver.onMessageReceived(msg);
  // this->virtualKeyboard->SetDirty();                  // to display the MIDI note as pressed/unpressed
}
#endif
