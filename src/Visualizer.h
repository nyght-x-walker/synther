#pragma once

#include <vector>

// Visualizer: stores recent audio and draws a simple waveform.
// - audio data is stored as mono samples for drawing
// - update() copies the latest buffer, draw() maps it to screen

class Visualizer {
public:
	// - allocate internal storage for the expected buffer size
	// - nChannels is kept to handle interleaved input
	void setup(int bufferSize, int nChannels);

	// - copy the latest audio buffer for drawing
	// - expects interleaved data (bufferSize * nChannels)
	void update(const std::vector<float>& audioBuffer);

	// - draw a simple waveform across the window width
	// - map sample values (-1..1) to vertical screen coordinates
	void draw();

private:
	std::vector<float> waveform; // mono waveform data for drawing
	int bufferSize = 0;          // expected frames per buffer
	int nChannels = 0;           // channels in the source buffer
};
