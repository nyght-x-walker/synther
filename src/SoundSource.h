#pragma once
#include "Envelope.h"

class SoundSource {
public:
	virtual ~SoundSource() = default;

	// Start a note at the given frequency (Hz) and velocity (0..1).
	virtual void noteOn(float frequency, float velocity) = 0;

	// Stop the currently playing note.
	virtual void noteOff() = 0;

	// Fill the output buffer with audio samples.
	virtual void render(float* output, int bufferSize, int nChannels) = 0;

	// True while the voice is producing sound.
	bool isActive() const;

protected:
	bool active = false;
	float frequency = 0.0f;
	float velocity = 0.0f;
	Envelope envelope;
};