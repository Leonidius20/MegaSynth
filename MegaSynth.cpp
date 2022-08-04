#include <algorithm>
#include "MegaSynth.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "Oscillator.h"

using std::copy;

MegaSynth::MegaSynth(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kFrequency)->InitDouble("Frequency", 440., 50., 20000., 0.01, "Hz");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "Hello iPlug 2!", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kFrequency));
  };
#endif
}

#if IPLUG_DSP
void MegaSynth::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  sample* leftOutput = outputs[0];
  sample* rightOutput = outputs[1];

  this->osciallator.generate(leftOutput, nFrames);

  copy(leftOutput, leftOutput + nFrames, rightOutput);
}

void MegaSynth::OnReset() { this->osciallator.setSampleRate(GetSampleRate()); }

void MegaSynth::OnParamChange(int paramId) {
  // MUTEX LOCK?

  switch (paramId)
  {
  case EParams::kFrequency:
    this->osciallator.setFrequency(GetParam(EParams::kFrequency)->Value());
    break;
  default:
    break;
  }
}
#endif
