export module megasynth.oscillator;

export class Oscillator {
public:

  enum Waveform
  {
    SINE = 0,
    SAW,
    SQUARE,
    TRIANGLE,
    kNumberOfWaveforms
  };

  inline void setWaveform(Waveform waveform) { this->waveform = waveform; }
  void setFrequency(double frequency);
  void setSampleRate(double sampleRate);
  inline void setMuted(bool muted) { this->isMuted = muted; }

  void generate(double* buffer, int numFrames);
  double nextSample();

  Oscillator() { updateIncrement(); }

private:
  Waveform waveform = SINE;
  double frequency = 440.0;
  double phase = 0.0;
  double sampleRate = 44100.0;
  double phaseIncrement;
  bool isMuted = true;

  const static double twoPi;

  void updateIncrement();


};