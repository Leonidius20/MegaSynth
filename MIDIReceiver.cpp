module;

#include <cmath>;
#include <algorithm>;
#include "IPlugMidi.h"

module megasynth.midi_receiver;

using std::pow;
using std::fill;
using iplug::IMidiMsg;

MIDIReceiver::MIDIReceiver() {
  fill(this->keyStatus, this->keyStatus + this->keyCount, false);
}

double MIDIReceiver::noteNumberToFrequency(int noteNumber) {
  return 440.0 * pow(2.0, (noteNumber - 69.0) / 12.0);
}

void MIDIReceiver::flush(int numFrames) {
  this->queue.Flush(numFrames);
  this->offset = 0;
}

void MIDIReceiver::onMessageReceived(const IMidiMsg &midiMessage) {
  auto status = midiMessage.StatusMsg();
  if (status == IMidiMsg::EStatusMsg::kNoteOn || status == IMidiMsg::EStatusMsg::kNoteOff) {
    this->queue.Add(midiMessage);
  }
}

void MIDIReceiver::advance()
{
  while (!this->queue.Empty())
  {
    const IMidiMsg& message = this->queue.Peek();
    if (message.mOffset > this->offset)
      break;

    auto status = message.StatusMsg();
    int noteNumber = message.NoteNumber();
    int velocity = message.Velocity();

    if (status == IMidiMsg::EStatusMsg::kNoteOn && velocity != 0)
    {
      if (this->keyStatus[noteNumber] == false)
      {
        this->keyStatus[noteNumber] == true;
        this->numKeysPressed++;
      }

      if (noteNumber != this->lastNoteNumber)    // a key pressed later overrides previously presssed key
      {
        this->lastNoteNumber = noteNumber;
        this->lastFrequency = this->noteNumberToFrequency(noteNumber);
        this->lastVelocity = velocity;
        this->noteOn(noteNumber, velocity);
      }
    }
    else // if the message was Note Off or Velocity = 0
    {
      if (this->keyStatus[noteNumber] == true)
      {
        this->keyStatus[noteNumber] == false;
        this->numKeysPressed--;
      }

      if (noteNumber == lastNoteNumber)         // if last note was released, nothing should play
      {
        this->lastNoteNumber = -1;
        this->noteOff(noteNumber, this->lastVelocity);
      }
    }

    this->queue.Remove();
  }

  this->offset++;
}