module;

#include <cmath>

module megasynth.envelope_generator;

using std::log;
using std::fmax;

double EnvelopeGenerator::nextSample() {
  if (this->currentStage != EnvelopeGenerator::EnvelopeStage::OFF && this->currentStage != EnvelopeGenerator::EnvelopeStage::SUSTAIN)
  {
    if (this->currentSampleIndex == this->nextStageSampleIndex) {
      auto newStage = static_cast<EnvelopeGenerator::EnvelopeStage>((this->currentStage + 1) % EnvelopeGenerator::EnvelopeStage::numStages);
      this->enterStage(newStage);
    }
    this->currentLevel *= this->multiplier; // advance the A, D, R effect
    this->currentSampleIndex++;             // keep track of time to switch modes
  }
  return this->currentLevel;
}

void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples) { this->multiplier = 1.0 + (log(endLevel) - log(startLevel)) / lengthInSamples; }

void EnvelopeGenerator::enterStage(EnvelopeGenerator::EnvelopeStage newStage) {
  this->currentStage = newStage;

  // make sure we start counting the length of new stage from 0
  this->currentSampleIndex = 0; 

  // calculate how long this new stage lasts (in samples)
  if (newStage == EnvelopeGenerator::EnvelopeStage::OFF || newStage == EnvelopeGenerator::EnvelopeStage::SUSTAIN)
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
  case EnvelopeGenerator::EnvelopeStage::OFF:
    this->currentLevel = 0.0;
    this->multiplier = 1.0;
    break;
  case EnvelopeGenerator::EnvelopeStage::ATTACK:
    this->currentLevel = this->minimumLevel;
    this->calculateMultiplier(this->currentLevel, 1.0, this->nextStageSampleIndex);
    break;
  case EnvelopeGenerator::EnvelopeStage::DECAY:
    this->currentLevel = 1.0;
    this->calculateMultiplier(this->currentLevel, fmax(this->stageValues[EnvelopeGenerator::EnvelopeStage::SUSTAIN], this->minimumLevel), this->nextStageSampleIndex);
    break;
  case EnvelopeGenerator::EnvelopeStage::SUSTAIN:
    this->currentLevel = this->stageValues[EnvelopeGenerator::EnvelopeStage::SUSTAIN];
    this->multiplier = 1.0;
    break;
  case EnvelopeGenerator::EnvelopeStage::RELEASE:
    // not chaging current level as we could switch to release from Attack or Decay
    this->calculateMultiplier(this->currentLevel, this->minimumLevel, this->nextStageSampleIndex);
    break;
  default:
    break;
  }
}

void EnvelopeGenerator::setSampleRate(double sampleRate) { this->sampleRate = sampleRate; }