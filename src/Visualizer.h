#pragma once

#include "ofMain.h"
#include <vector>

// Visualizer: stores recent audio and draws enhanced views.
// - audio data is stored as mono samples for drawing
// - update() copies the latest buffer and computes levels and spectrum
// - draw() shows waveform, amplitude bar, and spectrum
// - color and amplitude scale follow the active voice

class Visualizer {
public:
	// - allocate internal storage for the expected buffer size
	// - nChannels is kept to handle interleaved input
	void setup(int bufferSize, int nChannels);

	// - copy the latest audio buffer for drawing
	// - expects interleaved data (bufferSize * nChannels)
	void update(const std::vector<float>& audioBuffer);

	// - draw waveform, amplitude bar, and spectrum
	// - waveform uses voiceColor and amplitudeScale
	// - map sample values (-1..1) to vertical screen coordinates
	void draw();

	// - set the color used for the waveform and spectrum
	void setVoiceColor(ofColor color);

	// - set the scale for waveform amplitude (1.0 is normal)
	void setAmplitudeScale(float scale);

private:
	std::vector<float> waveform; // mono waveform data for drawing
	std::vector<float> spectrum; // magnitude spectrum for drawing
	int bufferSize = 0;          // expected frames per buffer
	int nChannels = 0;           // channels in the source buffer

	ofColor voiceColor = ofColor(80, 220, 180); // color for current voice
	float amplitudeScale = 1.0f;                // scale for waveform height
	float rmsLevel = 0.0f;                      // recent RMS level 0..1
	float peakLevel = 0.0f;                     // recent peak level 0..1
};
