#include "NoiseVoice.h"

#include <cstdlib>

void NoiseVoice::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
	envelope.setSampleRate(sampleRate);
}

void NoiseVoice::noteOn(float frequency, float velocity) {
	this->frequency = frequency;
	this->velocity = velocity;
	active = true;
	filterState = 0.0f;
	envelope.noteOn();
}

void NoiseVoice::noteOff() {
	active = false;
	envelope.noteOff();
}

// Fill output with white noise, copied to all channels.
// One random value in -1..1 per frame, scaled by velocity.
void NoiseVoice::render(float* output, int bufferSize, int nChannels) {
	for (int i = 0; i < bufferSize; i++) {
		float env = envelope.next();
		float sample = 0.0f;

		if (env > 0.0f) {
			float raw = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
			const float coeff = 0.3f;
			filterState += coeff * (raw - filterState);
			sample = filterState * velocity * env;
			if (!envelope.isActive()) {
				active = false;
			}
		}

		for (int c = 0; c < nChannels; c++) {
			output[i * nChannels + c] = sample;
		}
	}
}
