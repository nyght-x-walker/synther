#pragma once

#include "SoundSource.h"

// Square voice: square wave from the sign of the phase.
// High for the first half cycle, low for the second.

class SquareVoice : public SoundSource {
public:
	void setSampleRate(float sampleRate);

	void noteOn(float frequency, float velocity) override;
	void noteOff() override;
	void render(float* output, int bufferSize, int nChannels) override;

private:
	float phase = 0.0f;          // current phase in radians
	float sampleRate = 44100.0f; // sample rate in Hz
};
