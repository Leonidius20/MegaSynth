#include "MegaSynth.h"

#include "IPlug_include_in_plug_src.h" // has to be after MegaSynth.h

#include "IControls.h"
#include "IPlugMidi.h"
// #include "Oscillator.h"
// #include "MIDIReceiver.h"

import <algorithm>;

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
  // GetParam(kFrequency)->InitDouble("Frequency", 440., 50., 20000., 0.01, "Hz");
  GetParam(EParams::waveform)->InitEnum("Waveform", Oscillator::Waveform::SINE, Oscillator::Waveform::kNumberOfWaveforms, "", 0, "", "Sine", "Saw", "Square", "Triangle");

  GetParam(EParams::attack)->InitDouble("Attack", 0.01, 0.01, 10.0, 0.001, "s"); // setShape = 3?
  GetParam(EParams::decay)->InitDouble("Decay", 0.5, 0.01, 15.0, 0.001, "s");     // setShape = 3?
  GetParam(EParams::sustain)->InitDouble("Sustain", 0.1, 0.001, 1.0, 0.001); // setShape = 2?
  GetParam(EParams::release)->InitDouble("Release", 1.0, 0.001, 15.0, 0.001, "s"); // setShape = 3?

  GetParam(EParams::filterMode)->InitEnum("Filter mode", Filter::Mode::LOW_PASS, Filter::Mode::kNumModes, "", 0, "", "Low Pass", "High Pass", "Band Pass");

  GetParam(EParams::filterCutoff)->InitDouble("Filter Cutoff", 0.99, 0.01, 0.99, 0.001); // setShape = 2
  GetParam(EParams::filterResonance)->InitDouble("Filter Resonance", 0.01, 0.01, 1.0, 0.001);
  

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(iplug::igraphics::COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();

    this->virtualKeyboard = new IVKeyboardControl(b.GetFromBottom(75), this->virtualKeyboardMinimumNoteNumber, this->virtualKeyboardMinimumNoteNumber + 5 * 12);
    pGraphics->AttachControl(this->virtualKeyboard);

    // waveform control & its label
    auto waveformsBitmap = pGraphics->LoadBitmap(WAVEFORMS_FN, 4);
    auto* waveformSwitch = new IBSwitchControl(24, 53, waveformsBitmap, EParams::waveform);
    pGraphics->AttachControl(waveformSwitch);
    pGraphics->AttachControl(new ITextControl(b.GetFromTop(70).GetFromLeft(100), "waveform", IText(18)));

    // attack knob & its label
    auto* attackKnob = new IVKnobControl(waveformSwitch->GetRECT().GetHShifted(70).GetScaledAboutCentre(3), EParams::attack, "attack");
    pGraphics->AttachControl(attackKnob);

    // decay
    auto* decayKnob = new IVKnobControl(attackKnob->GetRECT().GetHShifted(70), EParams::decay, "decay");
    pGraphics->AttachControl(decayKnob);

    // sustian
    auto* sustainKnob = new IVKnobControl(decayKnob->GetRECT().GetHShifted(70), EParams::sustain, "sustain");
    pGraphics->AttachControl(sustainKnob);

    // release
    auto* releaseKnob = new IVKnobControl(sustainKnob->GetRECT().GetHShifted(70), EParams::release, "release");
    pGraphics->AttachControl(releaseKnob);

    // filter mode & its label
    auto filterModesBitmap = pGraphics->LoadBitmap(FILTER_MODES_FN, 3);
    auto* filterModeSwitch = new IBSwitchControl(24, 143, filterModesBitmap, EParams::filterMode);
    pGraphics->AttachControl(filterModeSwitch);
    pGraphics->AttachControl(new ITextControl(b.GetFromTop(250).GetFromLeft(100), "filter type", IText(18)));

    // cutoff
    auto* cutoffKnob = new IVKnobControl(filterModeSwitch->GetRECT().GetHShifted(70).GetScaledAboutCentre(3), EParams::filterCutoff, "cutoff");
    pGraphics->AttachControl(cutoffKnob);

    // resonance
    auto* resonanceKnob = new IVKnobControl(cutoffKnob->GetRECT().GetHShifted(70), EParams::filterResonance, "resonance");
    pGraphics->AttachControl(resonanceKnob);

    // pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kFrequency));
  };
#endif

  this->midiReceiver.noteOn.Connect(this, &MegaSynth::onNoteOn);
  this->midiReceiver.noteOff.Connect(this, &MegaSynth::onNoteOff);

  this->envelopeGenerator.beganEnvelopeCycle.Connect(this, &MegaSynth::onBeganEnvelopeCycle);
  this->envelopeGenerator.finishedEnvelopeCycle.Connect(this, &MegaSynth::onFinishedEnvelopeCycle);
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
    this->osciallator.setFrequency(this->midiReceiver.getLastFrequency());
    leftOutput[i] = this->filter.process(
      this->osciallator.nextSample() * this->envelopeGenerator.nextSample() * velocity / 127.0
    );
  }

  copy(leftOutput, leftOutput + nFrames, rightOutput);

  this->midiReceiver.flush(nFrames);
}

void MegaSynth::OnReset() {
  this->osciallator.setSampleRate(GetSampleRate());
  this->envelopeGenerator.setSampleRate(GetSampleRate());
}

void MegaSynth::OnParamChange(int paramId) {
  // MUTEX LOCK?

  using EParams::waveform;
  using EParams::attack;
  using EParams::decay;
  using EParams::sustain;
  using EParams::release;

  switch (paramId)
  {
  case waveform:
    this->osciallator.setWaveform(static_cast<Oscillator::Waveform>(GetParam(waveform)->Int()));
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
  default:
    break;
  }
}

void MegaSynth::ProcessMidiMsg(const IMidiMsg& msg) {
  this->midiReceiver.onMessageReceived(msg);
  // this->virtualKeyboard->SetDirty();                  // to display the MIDI note as pressed/unpressed
}
#endif
