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
        this->keyStatus[noteNumber] = true;
        this->numKeysPressed++;
        noteOn(noteNumber, velocity); // maybe should be outside of "if"
      }
    }
    else // if the message was Note Off or Velocity = 0
    {
      if (this->keyStatus[noteNumber] == true)
      {
        this->keyStatus[noteNumber] = false;
        this->numKeysPressed--;
        this->noteOff(noteNumber, velocity); // again maybe should be outside of "if"
      }
    }

    this->queue.Remove();
  }

  this->offset++;
}