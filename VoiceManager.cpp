module megasynth.voice_manager;

Voice *VoiceManager::findFreeVoice() {
	for (size_t i = 0; i < numberOfVoices; i++) {
		if (!voices[i].isVoiceActive()) {
			return &(voices[i]);
		}
	}
	return nullptr;
}

void VoiceManager::onNoteOn(int noteNumber, int velocity) {
	auto *voice = findFreeVoice();
	if (!voice) return;

	voice->reset();
	voice->setNoteNumber(noteNumber);
	voice->setVelocity(velocity);
	voice->setActive(true);
	voice->getVolumeEnv().enterStage(EnvelopeGenerator::Stage::ATTACK);
	voice->getFilterEnv().enterStage(EnvelopeGenerator::Stage::ATTACK);
}

void VoiceManager::onNoteOff(int noteNumber, int velocity) {
	for (size_t i = 0; i < numberOfVoices; i++) {
		auto &voice = voices[i];
		if (voice.isVoiceActive() 
			&& voice.getNoteNumber() == noteNumber) {
			voice.getVolumeEnv().enterStage(EnvelopeGenerator::Stage::RELEASE);
			voice.getFilterEnv().enterStage(EnvelopeGenerator::Stage::RELEASE);
		}
	}
}

double VoiceManager::nextSample() {
	double output = 0.0;
	double lfoValue = lfo.nextSample();
	for (size_t i = 0; i < numberOfVoices; i++) {
		auto &voice = voices[i];
		voice.setLFOValue(lfoValue);
		output += voice.nextSample();
	}
	return output;
}