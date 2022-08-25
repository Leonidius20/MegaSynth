module;

#include <cmath>

export module megasynth.voice;

import megasynth.oscillator;
import megasynth.envelope_generator;
import megasynth.filter;

export class Voice {
public:
	Voice() {
		// set voice free everytime the volume env fades out of RELEASE stage
		volumeEnv.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
	}

	void reset();

	inline void setFilterEnvelopeAmount(double amount) { this->filterEnvAmt = amount; }
	inline void setFilterLFOAmount(double amount) { this->filterLfoAmt = amount; }
	inline void setOscillatorOnePitchAmount(double amount) { this->osc1PitchLfoAmt = amount; }
	inline void setOscillatorTwoPitchAmount(double amount) { this->osc2PitchLfoAmt = amount; }
	inline void setOscillatorMix(double mix) { this->oscMix = mix; }
	inline void setLFOValue(double value) { this->lfoValue = value; }

	inline bool isVoiceActive() const { return this->isActive; }
	inline void setActive(bool active) { this->isActive = active; }
	inline void setVelocity(int vel) {this->midiVelocity = vel; }

	inline EnvelopeGenerator &getVolumeEnv() { return volumeEnv; }
	inline EnvelopeGenerator &getFilterEnv() { return filterEnv; }

	inline int getNoteNumber() { return midiNoteNumber; }

	inline PolyBlepOscillator &getOsc1() { return osc1; }
	inline PolyBlepOscillator &getOsc2() { return osc2; }

	inline Filter &getFilter() { return filter; }

	inline void setNoteNumber(int noteNumber) {
		this->midiNoteNumber = noteNumber;
		double frequency = 440.0 * std::pow(2.0, (noteNumber - 69.0) / 12.0);
		osc1.setFrequency(frequency);
		osc2.setFrequency(frequency);
	}

	double nextSample();
	void setFree() { isActive = false; };

private:
	PolyBlepOscillator osc1;
	PolyBlepOscillator osc2;
	EnvelopeGenerator volumeEnv;
	EnvelopeGenerator filterEnv;
	Filter filter;

	int midiNoteNumber = -1;
	int midiVelocity = 0;

	// the next block are global values but it's too
	// hard to have access to them from Voice if they
	// are in MegaSynth class so they are just copied to each voice
	double filterEnvAmt = 0.0;
	double oscMix = 0.5;
	double filterLfoAmt = 0.0;
	double osc1PitchLfoAmt = 0.0;
	double osc2PitchLfoAmt = 0.0;
	double lfoValue = 0.0;

	bool isActive = false;
};