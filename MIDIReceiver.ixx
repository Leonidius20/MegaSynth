module;

// #include "IPlug_include_in_plug_hdr.h"
#include "IPlugMidi.h"
#include "signal/GallantSignal.h";

export module megasynth.midi_receiver;

export class MIDIReceiver
{
private:
  iplug::IMidiQueue queue;
  static const int keyCount = 128;
  int numKeysPressed = 0;              // how many keys are being played at the moment through MIDI
  bool keyStatus[keyCount];     // array of on/off states for each key
  int lastNoteNumber = -1;
  double lastFrequency = -1.0;
  int lastVelocity = 0;
  int offset = 0;

  double noteNumberToFrequency(int noteNumber);

public:
  MIDIReceiver();

  inline bool getKeyStatus(int keyIndex) const { return keyStatus[keyIndex]; }
  inline bool getNumKeysPressed() const { return numKeysPressed; }
  inline int getLastNoteNumber() const { return lastNoteNumber; }
  inline double getLastFrequency() const { return lastFrequency; }
  inline int getLastVelocity() const { return lastVelocity; }

  void advance();
  void onMessageReceived(const iplug::IMidiMsg &midiMessage);

  void flush(int numFrames);
  inline void resize(int blockSize) { queue.Resize(blockSize); }

  Gallant::Signal2<int, int> noteOn;
  Gallant::Signal2<int, int> noteOff;
};
