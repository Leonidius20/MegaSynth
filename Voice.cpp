module megasynth.voice;

double Voice::nextSample() {
	if (!isActive) return 0.0;

	double osc1Output = osc1.nextSample();
	double osc2Output = osc2.nextSample();
	double oscSum = (1 - oscMix) * osc1Output 
		+ oscMix * osc2Output;

	double volumeEnvValue = volumeEnv.nextSample();
	double filterEnvValue = filterEnv.nextSample();

	filter.setCutoffMod(filterEnvValue * filterEnvAmt 
		+ lfoValue * filterLfoAmt);

	osc1.setPitchMod(lfoValue * osc1PitchLfoAmt);
	osc2.setPitchMod(lfoValue * osc2PitchLfoAmt);

	return filter.process(oscSum * volumeEnvValue * midiVelocity / 127.0);
}

void Voice::reset() {
	midiNoteNumber = -1;
	midiVelocity = 0;
	osc1.reset();
	osc2.reset();
	volumeEnv.reset();
	filterEnv.reset();
	filter.reset();
}