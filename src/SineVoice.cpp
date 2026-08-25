#include "SineVoice.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

void SineVoice::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
	envelope.setSampleRate(sampleRate);
}

void SineVoice::noteOn(float frequency, float velocity) {
	this->frequency = frequency;
	this->velocity = velocity;
	phase = 0.0f;
	active = true;
	envelope.noteOn();
}

void SineVoice::noteOff() {
	active = false;
	envelope.noteOff();
}

// Fill output with one sine sample per frame, copied to all channels.
// The phase advances by one cycle (2*PI) per period, i.e. by
// frequency / sampleRate, and wraps at 2*PI to keep it bounded.
void SineVoice::render(float* output, int bufferSize, int nChannels) {
	const float phaseIncrement = (frequency / sampleRate) * glm::two_pi<float>();
	const float twoPi = glm::two_pi<float>();

	for (int i = 0; i < bufferSize; i++) {
		float env = envelope.next();
		float sample = 0.0f;

		if (env > 0.0f) {
			phase += phaseIncrement;
			if (phase >= twoPi) {
				phase -= twoPi;
			}
			sample = std::sin(phase) * velocity * env;
			if (!envelope.isActive()) {
				active = false;
			}
		}

		for (int c = 0; c < nChannels; c++) {
			output[i * nChannels + c] = sample;
		}
	}
}