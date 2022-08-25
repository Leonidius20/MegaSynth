module;

#include <numbers>;

export module megasynth.oscillator;

constexpr inline double twoPi = 2 * std::numbers::pi;

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
  void setPitchMod(double amount);
 
  // void generate(double* buffer, int numFrames);
  virtual double nextSample();

  void reset() { phase = 0.0; }

  Oscillator() { updateIncrement(); }

protected:
  Waveform waveform = SINE;
  double frequency = 440.0;
  double phase = 0.0;  
  static double sampleRate;
  double phaseIncrement;
  double pitchMod = 0.0;

  void updateIncrement();
  double naiveWaveform(Waveform wave);
};


export class PolyBlepOscillator : public Oscillator {
public:
    PolyBlepOscillator() { updateIncrement(); }
    double nextSample() override;
private:
    double lastOutput = 0.0;

    double poly_blep(double t);
};