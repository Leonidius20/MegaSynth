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
 
  void generate(double* buffer, int numFrames);
  double nextSample();

  void reset() { phase = 0.0; }

  Oscillator() { updateIncrement(); }

private:
  Waveform waveform = SINE;
  double frequency = 440.0;
  double phase = 0.0;
  static double sampleRate;
  double phaseIncrement;

  const static double twoPi;

  void updateIncrement();


};