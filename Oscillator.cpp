module;

#include <cmath>;
#include <numbers>;

module megasynth.oscillator;

using std::numbers::pi;
using std::sin;
using std::fabs;

const double Oscillator::twoPi = 2 * pi;

void Oscillator::setFrequency(double frequency) {
  this->frequency = frequency;
  this->updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate)
{
  this->sampleRate = sampleRate;
  this->updateIncrement();
}

void Oscillator::updateIncrement() {
  this->phaseIncrement = this->frequency * 2 * pi / this->sampleRate;
}

void Oscillator::generate(double* buffer, int numFrames) {
  

  switch (this->waveform)
  {
  case Waveform::SINE:
    for (int i = 0; i < numFrames; i++)
    {
      buffer[i] = sin(this->phase);
      this->phase += this->phaseIncrement;
      while (this->phase >= twoPi)
      {
        this->phase -= twoPi;
      }
    }
    break;
  case Waveform::SAW:
    for (int i = 0; i < numFrames; i++)
    {
      buffer[i] = 1.0 - (2.0 * this->phase / twoPi);
      this->phase += this->phaseIncrement;
      while (this->phase >= twoPi)
      {
        this->phase -= twoPi;
      }
    }
    break;
  case Waveform::SQUARE:
    for (int i = 0; i < numFrames; i++)
    {
      if (this->phase <= pi)
      {
        buffer[i] = 1.0;
      }
      else
      {
        buffer[i] = -1.0;
      }
      this->phase += this->phaseIncrement;
      while (this->phase >= twoPi)
      {
        this->phase -= twoPi;
      }
    }
    break;
  case Waveform::TRIANGLE:
    for (int i = 0; i < numFrames; i++)
    {
      double value = -1.0 + (2.0 * this->phase / twoPi);
      buffer[i] = 2.0 * (fabs(value) - 0.5);
      this->phase += this->phaseIncrement;
      while (this->phase >= twoPi)
      {
        this->phase -= twoPi;
      }
    }
    break;
  default:
    break;
  }

}

double Oscillator::nextSample() {
  double value = 0.0;
  if (isMuted)
    return value;

  switch (this->waveform)
  {
  case Waveform::SINE:
    value = sin(this->phase);
    break;
  case Waveform::SAW:
    value = 1.0 - (2.0 * this->phase / twoPi);
    break;
  case Waveform::SQUARE:
    if (this->phase <= pi)
    {
      value = 1.0;
    }
    else
    {
      value = -1.0;
    }
    break;
  case Waveform::TRIANGLE:
    value = -1.0 + (2.0 * this->phase / twoPi);
    value = 2.0 * (fabs(value) - 0.5);
    break;
  default:
    break;
  }

  this->phase += this->phaseIncrement;
  while (this->phase >= twoPi)
  {
    this->phase -= twoPi;
  }

  return value;
}