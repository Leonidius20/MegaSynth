export module megasynth.envelope_generator;

export class EnvelopeGenerator
{
public:

  enum EnvelopeStage
  {
    OFF = 0,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    numStages
  };

  void enterStage(EnvelopeStage stage);
  double nextSample();
  void setSampleRate(double newSampleRate);
  inline EnvelopeStage getCurrentStage() const { return currentStage; }
  const double minimumLevel = 0.0001;

private:
  EnvelopeStage currentStage = EnvelopeStage::OFF;
  double currentLevel = minimumLevel;
  double multiplier = 1.0;
  double sampleRate = 44100.0;
  double stageValues[EnvelopeStage::numStages] = {0.0, 0.01, 0.5, 0.1, 1.0}; // off, A, D, S, R
  void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
  unsigned long long currentSampleIndex = 0;
  unsigned long long nextStageSampleIndex = 0; // sample at which the time based modes (A, D, R) switch
};