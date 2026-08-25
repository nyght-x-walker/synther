#include "SquareVoice.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

void SquareVoice::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
	envelope.setSampleRate(sampleRate);
}

void SquareVoice::noteOn(float frequency, float velocity) {
	this->frequency = frequency;
	this->velocity = velocity;
	phase = 0.0f;
	lfoPhase = 0.0f;
	filterState = 0.0f;
	active = true;
	envelope.noteOn();
}

void SquareVoice::noteOff() {
	active = false;
	envelope.noteOff();
}

void SquareVoice::render(float* output, int bufferSize, int nChannels) {
	const float twoPi = glm::two_pi<float>();
	const float pi = glm::pi<float>();
	const float lfoInc = (5.0f / sampleRate) * twoPi;

	for (int i = 0; i < bufferSize; i++) {
		float env = envelope.next();
		float sample = 0.0f;

		if (env > 0.0f) {
			float lfo = std::sin(lfoPhase) * 0.015f;
			lfoPhase += lfoInc;
			if (lfoPhase >= twoPi) lfoPhase -= twoPi;
			float effFreq = frequency * (1.0f + lfo);
			float inc = (effFreq / sampleRate) * twoPi;
			phase += inc;
			if (phase >= twoPi) {
				phase -= twoPi;
			}
			float raw = (phase < pi ? 1.0f : -1.0f) * velocity * env;
			filterState += 0.35f * (raw - filterState);
			sample = filterState;
			if (!envelope.isActive()) {
				active = false;
			}
		}

		for (int c = 0; c < nChannels; c++) {
			output[i * nChannels + c] = sample;
		}
	}
}
