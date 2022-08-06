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
  OscillatorWaveform waveform = WAVEFORM_TRIANGLE;
  double frequency = 440.0;
  double phase = 0.0;
  double sampleRate = 44100.0;
  double phaseIncrement;
  bool isMuted = true;

  const static double twoPi;

  void updateIncrement();

public:
  inline void setWaveform(OscillatorWaveform waveform) { this->waveform = waveform; }
  void setFrequency(double frequency);
  void setSampleRate(double sampleRate);
  inline void setMuted(bool muted) { this->isMuted = muted; }

  void generate(double* buffer, int numFrames);
  double nextSample();

  Oscillator() { updateIncrement(); }
};