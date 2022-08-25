module;

#include <cmath>;
#include <numbers>;

module megasynth.oscillator;

using std::numbers::pi;
using std::sin;
using std::fabs;
using std::pow;
using std::fmin;
using std::fmax;

double Oscillator::sampleRate = 44100.0;

void Oscillator::setFrequency(double frequency) {
  this->frequency = frequency;
  this->updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate)
{
  this->sampleRate = sampleRate;
  this->updateIncrement();
}

void Oscillator::setPitchMod(double amount) {
    this->pitchMod = amount;
    updateIncrement();
}

void Oscillator::updateIncrement() {
  double pitchModAsFrequency = pow(2.0, fabs(pitchMod) * 14.0) - 1;
  if (pitchMod < 0) 
      pitchModAsFrequency = -pitchModAsFrequency;

  double nyquistFreq = this->sampleRate / 2.0;

  double calculatedFreq = fmin(fmax(frequency + pitchModAsFrequency, 0), nyquistFreq);


  this->phaseIncrement = calculatedFreq * twoPi / this->sampleRate;
}

/*void Oscillator::generate(double* buffer, int numFrames) {
  

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

}*/

double Oscillator::nextSample() {
  double value = this->naiveWaveform(this->waveform);

  this->phase += this->phaseIncrement;
  while (this->phase >= twoPi)
  {
    this->phase -= twoPi;
  }

  return value;
}

double Oscillator::naiveWaveform(Oscillator::Waveform wave) {
    double value = 0.0;

    switch (wave)
    {
    case Waveform::SINE:
        value = sin(this->phase);
        break;
    case Waveform::SAW:
        value = (2.0 * this->phase / twoPi) - 1.0;
        break;
    case Waveform::SQUARE:
        if (this->phase <= pi) {
            value = 1.0;
        }
        else {
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

    return value;
}