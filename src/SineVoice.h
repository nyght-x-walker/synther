#pragma once

#include "SoundSource.h"

// Sine voice: generates a sine wave with a phase accumulator.
// One mono sample is produced per frame and replicated to all channels.

class SineVoice : public SoundSource {
public:
	// Configure the sample rate used to advance the phase.
	void setSampleRate(float sampleRate);

	// Start a note and reset the phase so the wave starts at zero.
	void noteOn(float frequency, float velocity) override;

	// Stop the note; output becomes silence.
	void noteOff() override;

	// Fill the interleaved output buffer with sine samples.
	void render(float* output, int bufferSize, int nChannels) override;

private:
	float phase = 0.0f;
	float lfoPhase = 0.0f;
	float sampleRate = 44100.0f;
};