#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IPlugMidi.h"

import megasynth.midi_receiver;
import megasynth.voice_manager;

const int kNumPresets = 1;

enum EParams
{
  osc1Waveform = 0,

  attack,
  decay,
  sustain,
  release,

  osc1PitchMod,
  osc2Waveform,
  osc2PitchMod,
  oscMix,

  lfoWaveform,
  lfoRate,

  filterMode,
  filterCutoff,
  filterResonance,

  filterAttack,
  filterDecay,
  filterSustain,
  filterRelease,
  filterEnvelopeAmount,

  filterLfoAmount,

  kNumParams
};


/*typedef struct {
    const char *name;
    const int defaultValue;
    const int numberOfValues;
    const char **listOfValues;
} enumControls_struct;

const enumControls_struct enumControls[3] = {
    {.name = "Osc 1 Waveform", .defaultValue=Oscillator::Waveform::SINE, 
        .numberOfValues=Oscillator::Waveform::kNumberOfWaveforms, 
        .listOfValues={"", ""}
    }, 
    {}
};*/

using namespace iplug;
using namespace igraphics;

class MegaSynth final : public iplug::Plugin
{
private:
  MIDIReceiver midiReceiver;
  VoiceManager voiceManager;

  void createParams();
  void createGraphics(IGraphics* pGraphics);

public:
  MegaSynth(const iplug::InstanceInfo& info);
  void OnReset() override;
  void OnParamChange(int paramId) override;
  void ProcessMidiMsg(const iplug::IMidiMsg& msg) override;

  inline int getNumKeysPressed() const { return midiReceiver.getNumKeysPressed(); }
  inline bool getKeyStatus(int keyIndex) const { return midiReceiver.getKeyStatus(keyIndex); }

  static const int virtualKeyboardMinimumNoteNumber = 48;
  int lastVirtualKeyboardNoteNumber = virtualKeyboardMinimumNoteNumber - 1;

  IControl* virtualKeyboard;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames) override;
#endif
};
