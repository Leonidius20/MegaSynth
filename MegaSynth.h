#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IPlugMidi.h"
#include "Oscillator.h"
#include "MIDIReceiver.h"

const int kNumPresets = 1;

enum EParams
{
  // kFrequency = 0,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class MegaSynth final : public iplug::Plugin
{
private:
  Oscillator osciallator;
  MIDIReceiver midiReceiver;

public:
  MegaSynth(const iplug::InstanceInfo& info);
  void OnReset() override;
  void OnParamChange(int paramId) override;
  void ProcessMidiMsg(const iplug::IMidiMsg& msg) override;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames) override;
#endif
};
