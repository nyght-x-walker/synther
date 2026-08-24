#pragma once

// Abstract base class for audio voices. Defines the common
// note/rendering interface and the state shared by all voices.

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
	bool active = false;     // voice sounds while this is true
	float frequency = 0.0f;  // note frequency in Hz
	float velocity = 0.0f;   // note velocity in 0..1
};