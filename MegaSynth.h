#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IPlugMidi.h"
#include "Oscillator.h"
#include "MIDIReceiver.h"

import megasynth.envelope_generator;

const int kNumPresets = 1;

enum EParams
{
  waveform = 0,
  attack,
  decay,
  sustain,
  release,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class MegaSynth final : public iplug::Plugin
{
private:
  Oscillator osciallator;
  MIDIReceiver midiReceiver;
  EnvelopeGenerator envelopeGenerator;

  void processVirtualKeyboard();

  inline void onNoteOn(const int noteNumber, const int velocity) {
    this->envelopeGenerator.enterStage(EnvelopeGenerator::Stage::ATTACK);
  }

  inline void onNoteOff(const int noteNumber, const int velocity) {
    this->envelopeGenerator.enterStage(EnvelopeGenerator::Stage::RELEASE);
  }

  inline void onBeganEnvelopeCycle() {
    this->osciallator.setMuted(false);
  }

  inline void onFinishedEnvelopeCycle() {
    this->osciallator.setMuted(true);
  }

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
