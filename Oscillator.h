#pragma once

enum OscillatorWaveform
{
  WAVEFORM_SINE,
  WAVEFORM_SAW,
  WAVEFORM_SQUARE,
  WAVEFORM_TRIANGLE
};

class Oscillator {
private:
  OscillatorWaveform waveform = WAVEFORM_SINE;
  double frequency = 440.0;
  double phase = 0.0;
  double sampleRate = 44100.0;
  double phaseIncrement;
  void updateIncrement();

public:
  void setWaveform(OscillatorWaveform waveform) { this->waveform = waveform; };
  void setFrequency(double frequency);
  void setSampleRate(double sampleRate);

  void generate(double* buffer, int numFrames);

  Oscillator() { updateIncrement(); }
};