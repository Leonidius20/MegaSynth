export module megasynth.filter;

export class Filter {
public:
  enum Mode {
    LOW_PASS = 0,
    HIGH_PASS,
    BAND_PASS,
    kNumModes
  };

  double process(double inputValue);

  inline void setCutoff(double cutoff) {
    this->cutoff = cutoff;
    calculateFeedbackAmount();
  }

  inline void setResonance(double resonance)
  {
    this->resonance = resonance;
    calculateFeedbackAmount();
  }

  inline void setFilterMode(Mode mode) { this->mode = mode; }


private:
  double cutoff = 0.99;
  double resonance = 0.0;
  Mode mode = Mode::LOW_PASS;
  double feedbackAmount = 0.0;
  double buf0 = 0.0;
  double buf1 = 0.0;

  inline void calculateFeedbackAmount() {
    feedbackAmount = resonance + resonance / (1.0 - cutoff);
  }

};