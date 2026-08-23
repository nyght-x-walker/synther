#include "NoiseVoice.h"

#include <cstdlib>

void NoiseVoice::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
}

void NoiseVoice::noteOn(float frequency, float velocity) {
	this->frequency = frequency;
	this->velocity = velocity;
	active = true;
}

void NoiseVoice::noteOff() {
	active = false;
}

// Fill output with white noise, copied to all channels.
// - One random value in -1..1 per frame, scaled by velocity.
void NoiseVoice::render(float* output, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) {
		float sample = 0.0f;

		if (active) {
			// Simple uniform white noise
			sample = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * velocity;
		}

		for (int c = 0; c < nChannels; c++) {
			output[i * nChannels + c] = sample;
		}
	}
}
