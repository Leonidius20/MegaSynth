#include "MegaSynth.h"

#include "IPlug_include_in_plug_src.h" // has to be after MegaSynth.h

#include <algorithm>
#include "IControls.h"
#include "IPlugMidi.h"
#include "Oscillator.h"
#include "MIDIReceiver.h"


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

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(iplug::igraphics::COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "Hello iPlug 2!", IText(50)));

    this->virtualKeyboard = new IVKeyboardControl(b.GetFromBottom(75), this->virtualKeyboardMinimumNoteNumber, this->virtualKeyboardMinimumNoteNumber + 5 * 12);
    pGraphics->AttachControl(this->virtualKeyboard);

    // pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kFrequency));
  };
#endif

  this->midiReceiver.noteOn.Connect(this, &MegaSynth::onNoteOn);
  this->midiReceiver.noteOff.Connect(this, &MegaSynth::onNoteOff);
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
    if (velocity > 0)
    {
      this->osciallator.setFrequency(this->midiReceiver.getLastFrequency());
      this->osciallator.setMuted(false);
    }
    else
      this->osciallator.setMuted(true);

    leftOutput[i] = this->osciallator.nextSample() * this->envelopeGenerator.nextSample() * velocity / 127.0;
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

  /* switch (paramId)
  {
  case EParams::kFrequency:
    this->osciallator.setFrequency(GetParam(EParams::kFrequency)->Value());
    break;
  default:
    break;
  }*/
}

void MegaSynth::ProcessMidiMsg(const IMidiMsg& msg) {
  this->midiReceiver.onMessageReceived(msg);
  // this->virtualKeyboard->SetDirty();                  // to display the MIDI note as pressed/unpressed
}
#endif
