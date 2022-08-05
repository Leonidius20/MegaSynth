#include <cmath>
#include <numbers>
#include "Oscillator.h"

using std::numbers::pi;
using std::sin;
using std::fabs;

void Oscillator::setFrequency(double frequency) {
  this->frequency = frequency;
  this->updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate)
{
  this->sampleRate = sampleRate;
  this->updateIncrement();
}

void Oscillator::updateIncrement() { this->phaseIncrement = this->frequency * 2 * pi / this->sampleRate; }

void Oscillator::generate(double* buffer, int numFrames) {
  const double twoPi = 2 * pi;

  switch (this->waveform)
  {
  case OscillatorWaveform::WAVEFORM_SINE:
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
  case OscillatorWaveform::WAVEFORM_SAW:
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
  case OscillatorWaveform::WAVEFORM_SQUARE:
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
  case OscillatorWaveform::WAVEFORM_TRIANGLE:
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