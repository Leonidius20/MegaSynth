module;

#include <functional>

export module megasynth.voice_manager;

import megasynth.voice;

export class VoiceManager {
private:
	static const int numberOfVoices = 64;
	Voice voices[numberOfVoices]; // TODO: dynamic allocation
	Oscillator lfo;
	Voice *findFreeVoice();
public:
	void onNoteOn(int noteNumber, int velocity);
	void onNoteOff(int noteNumber, int velocity);
	double nextSample();
	void setSampleRate(double rate);

	inline void setLfoWaveform(Oscillator::Waveform wave) {
		lfo.setWaveform(wave);
	}

	inline void setLfoRate(double rate) {
		lfo.setFrequency(rate);
	}

	typedef std::function<void (Voice&)> VoiceChangerFunction;

	void changeAllVoices(VoiceChangerFunction changer) {
		for (size_t i = 0; i < numberOfVoices; i++) {
			changer(voices[i]);
		}
	}


	static void setVolumeEnvStageValue(Voice &voice, EnvelopeGenerator::Stage stage, double value) {
		voice.getVolumeEnv().setStageValue(stage, value);
	}

	static void setFilterEnvStageValue(Voice &voice, EnvelopeGenerator::Stage stage, double value) {
		voice.getFilterEnv().setStageValue(stage, value);
	}

	static void setOscWaveform(Voice &voice, int oscNumber, Oscillator::Waveform wave) {
		switch (oscNumber) {
		case 1:
			voice.getOsc1().setWaveform(wave);
			break;
		case 2:
			voice.getOsc2().setWaveform(wave);
			break;
		default:
			break;
		}
	}

	static void setOscPitchLfoAmt(Voice &voice, int oscNum, double amt) {
		switch (oscNum) {
		case 1:
			voice.setOscillatorOnePitchAmount(amt);
			break;
		case 2:
			voice.setOscillatorTwoPitchAmount(amt);
			break;
		default:
			break;
		}
	}

	static void setOscMix(Voice &voice, double value) {
		voice.setOscillatorMix(value);
	}

	static void setFilterCutoff(Voice &voice, double cutoff) {
		voice.getFilter().setCutoff(cutoff);
	}

	static void setFilterResonance(Voice &voice, double resonance) {
		voice.getFilter().setResonance(resonance);
	}

	static void setFilterMode(Voice& voice, Filter::Mode mode) {
		voice.getFilter().setFilterMode(mode);
	}

	static void setFilterEnvAmount(Voice& voice, double amount) {
		voice.setFilterEnvelopeAmount(amount);
	}

	static void setFilterLFOAmount(Voice& voice, double amount) {
		voice.setFilterLFOAmount(amount);
	}
};