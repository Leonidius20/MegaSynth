module;

#include "signal/GallantSignal.h"

export module megasynth.envelope_generator;

export class EnvelopeGenerator
{
public:

  enum Stage
  {
    OFF = 0,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    numStages
  };

  void enterStage(Stage stage);
  double nextSample();
  static void setSampleRate(double newSampleRate);
  void setStageValue(Stage stage, double value);
  inline Stage getCurrentStage() const { return currentStage; }
  void reset();

  const double minimumLevel = 0.0001;

  Gallant::Signal0<> beganEnvelopeCycle;
  Gallant::Signal0<> finishedEnvelopeCycle;

private:
  Stage currentStage = Stage::OFF;
  double currentLevel = minimumLevel;
  double multiplier = 1.0;
  static double sampleRate;
  double stageValues[Stage::numStages] = {0.0, 0.01, 0.5, 0.1, 1.0}; // off, A, D, S, R
  void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
  unsigned long long currentSampleIndex = 0;
  unsigned long long nextStageSampleIndex = 0; // sample at which the time based modes (A, D, R) switch
};