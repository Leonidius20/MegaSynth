module;

#include <cmath>

module megasynth.envelope_generator;

using std::log;
using std::fmax;

double EnvelopeGenerator::sampleRate = 44100.0;

double EnvelopeGenerator::nextSample() {
  if (this->currentStage != EnvelopeGenerator::Stage::OFF && this->currentStage != EnvelopeGenerator::Stage::SUSTAIN)
  {
    if (this->currentSampleIndex == this->nextStageSampleIndex) {
      auto newStage = static_cast<EnvelopeGenerator::Stage>((this->currentStage + 1) % EnvelopeGenerator::Stage::numStages);
      this->enterStage(newStage);
    }
    this->currentLevel *= this->multiplier; // advance the A, D, R effect
    this->currentSampleIndex++;             // keep track of time to switch modes
  }
  return this->currentLevel;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples) { this->multiplier = 1.0 + (log(endLevel) - log(startLevel)) / lengthInSamples; }

void EnvelopeGenerator::enterStage(EnvelopeGenerator::Stage newStage) {
  if (this->currentStage == newStage)
    return;
  if (this->currentStage == EnvelopeGenerator::Stage::OFF)
  {
    this->beganEnvelopeCycle();
  }
  if (newStage == EnvelopeGenerator::Stage::OFF)
  {
    this->finishedEnvelopeCycle();
  }

  this->currentStage = newStage;

  // make sure we start counting the length of new stage from 0
  this->currentSampleIndex = 0; 

  // calculate how long this new stage lasts (in samples)
  if (newStage == EnvelopeGenerator::Stage::OFF || newStage == EnvelopeGenerator::Stage::SUSTAIN)
  {
    this->nextStageSampleIndex = 0;
  }
  else
  {
    // the index of a sample at which the ADSR stage should swith is = to the value of current stage
    // (in seconds) multiplited by sample rate aka number of samples per second
    this->nextStageSampleIndex = this->stageValues[newStage] * this->sampleRate;
  }
  switch (newStage)
  {
  case EnvelopeGenerator::Stage::OFF:
    this->currentLevel = 0.0;
    this->multiplier = 1.0;
    break;
  case EnvelopeGenerator::Stage::ATTACK:
    this->currentLevel = this->minimumLevel;
    this->calculateMultiplier(this->currentLevel, 1.0, this->nextStageSampleIndex);
    break;
  case EnvelopeGenerator::Stage::DECAY:
    this->currentLevel = 1.0;
    this->calculateMultiplier(this->currentLevel, fmax(this->stageValues[EnvelopeGenerator::Stage::SUSTAIN], this->minimumLevel), this->nextStageSampleIndex);
    break;
  case EnvelopeGenerator::Stage::SUSTAIN:
    this->currentLevel = this->stageValues[EnvelopeGenerator::Stage::SUSTAIN];
    this->multiplier = 1.0;
    break;
  case EnvelopeGenerator::Stage::RELEASE:
    // not chaging current level as we could switch to release from Attack or Decay
    this->calculateMultiplier(this->currentLevel, this->minimumLevel, this->nextStageSampleIndex);
    break;
  default:
    break;
  }
}

void EnvelopeGenerator::setSampleRate(double sampleRate) { EnvelopeGenerator::sampleRate = sampleRate; }

void EnvelopeGenerator::setStageValue(EnvelopeGenerator::Stage stage, double value) {
  this->stageValues[stage] = value;
  if (stage == this->currentStage)
  {
    if (stage == EnvelopeGenerator::Stage::SUSTAIN)
    {
      this->currentLevel = value;
    }
    else // if A, D, R (cannot be anything else bc of stage == this->currentStage if condition
    {
      double nextLevelValue;
      switch (stage)
      {
      case EnvelopeGenerator::ATTACK:
        nextLevelValue = 1.0;
        break;
      case EnvelopeGenerator::DECAY:
        nextLevelValue = fmax(this->stageValues[EnvelopeGenerator::Stage::SUSTAIN], this->minimumLevel);
        break;
      case EnvelopeGenerator::RELEASE:
        nextLevelValue = this->minimumLevel;
        break;
      default:
        break;
      }

      double currentStageProgress = (this->currentSampleIndex + 0.0)
        / this->nextStageSampleIndex;
      double remainingStageProgress = 1.0 - currentStageProgress;
      unsigned long long samplesUntilNextStage = remainingStageProgress * value * sampleRate;
      this->nextStageSampleIndex = this->currentSampleIndex + samplesUntilNextStage;
      this->calculateMultiplier(this->currentLevel, nextLevelValue, samplesUntilNextStage);
    }
  }

  if (this->currentStage == EnvelopeGenerator::Stage::DECAY
    && stage == EnvelopeGenerator::Stage::SUSTAIN)
  {
    // decay to a different sustain level than before
    unsigned long long samplesUntilNextStage = this->nextStageSampleIndex - this->currentSampleIndex;
    this->calculateMultiplier(this->currentLevel, fmax(this->stageValues[EnvelopeGenerator::Stage::SUSTAIN], this->minimumLevel), samplesUntilNextStage);
  }
}

void EnvelopeGenerator::reset() {
    currentStage = Stage::OFF;
    currentLevel = minimumLevel;
    multiplier = 1.0;
    currentSampleIndex = 0;
    nextStageSampleIndex = 0;
}