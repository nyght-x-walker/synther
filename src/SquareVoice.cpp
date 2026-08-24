#include "SquareVoice.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

void SquareVoice::setSampleRate(float sampleRate) {
	this->sampleRate = sampleRate;
}

void SquareVoice::noteOn(float frequency, float velocity) {
	this->frequency = frequency;
	this->velocity = velocity;
	phase = 0.0f;
	active = true;
}

void SquareVoice::noteOff() {
	active = false;
}

// Fill output with a square wave, copied to all channels.
// - Phase advances by frequency / sampleRate per sample and wraps at 2*PI.
// - Sign of the phase gives the square level.
void SquareVoice::render(float* output, int bufferSize, int nChannels) {
	const float phaseIncrement = (frequency / sampleRate) * glm::two_pi<float>();
	const float twoPi = glm::two_pi<float>();
	const float pi = glm::pi<float>();

	for (int i = 0; i < bufferSize; i++) {
		float sample = 0.0f;

		if (active) {
			phase += phaseIncrement;
			if (phase >= twoPi) {
				phase -= twoPi;
			}
			// Square from phase sign
			sample = (phase < pi ? 1.0f : -1.0f) * velocity;
		}

		for (int c = 0; c < nChannels; c++) {
			output[i * nChannels + c] = sample;
		}
	}
}
