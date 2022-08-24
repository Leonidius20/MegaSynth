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
};