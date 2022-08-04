#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Oscillator.h"

const int kNumPresets = 1;

enum EParams
{
  kFrequency = 0,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class MegaSynth final : public Plugin
{
private:
  Oscillator osciallator;

public:
  MegaSynth(const InstanceInfo& info);
  void OnReset() override;
  void OnParamChange(int paramId) override;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
