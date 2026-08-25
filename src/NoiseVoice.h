#pragma once

#include "SoundSource.h"

// Noise voice: white noise.
// Random sample per frame, uniform in -1..1.

class NoiseVoice : public SoundSource {
public:
	void setSampleRate(float sampleRate);

	void noteOn(float frequency, float velocity) override;
	void noteOff() override;
	void render(float* output, int bufferSize, int nChannels) override;

private:
	float sampleRate = 44100.0f;
	float filterState = 0.0f;
};
