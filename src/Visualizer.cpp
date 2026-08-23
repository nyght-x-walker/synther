#include "Visualizer.h"
#include "ofMain.h"

void Visualizer::setup(int bufferSize, int nChannels) {
	// - store dimensions for later mapping
	// - allocate mono storage, one value per frame
	this->bufferSize = bufferSize;
	this->nChannels = nChannels;
	waveform.assign(bufferSize, 0.0f);
}

void Visualizer::update(const std::vector<float>& audioBuffer) {
	// - keep internal waveform in sync with audio thread
	// - handle interleaved input by taking first channel
	// - clamp to expected size to avoid visual mismatch

	if (audioBuffer.empty() || bufferSize == 0) {
		return;
	}

	int frames = static_cast<int>(audioBuffer.size() / std::max(1, nChannels));
	frames = std::min(frames, bufferSize);

	for (int i = 0; i < frames; i++) {
		// - interleaved layout: frame * nChannels + channel
		waveform[i] = audioBuffer[i * nChannels];
	}

	// - fill remainder with silence if buffer is short
	for (int i = frames; i < bufferSize; i++) {
		waveform[i] = 0.0f;
	}
}

void Visualizer::draw() {
	if (waveform.empty() || bufferSize == 0) {
		return;
	}

	// - draw a waveform that contrasts with the dark background
	// - map sample -1..1 to vertical pixels around the center
	float width = static_cast<float>(ofGetWidth());
	float height = static_cast<float>(ofGetHeight());
	float centerY = height * 0.55f;
	float amplitude = height * 0.28f;
	float xStep = width / static_cast<float>(std::max(1, bufferSize - 1));

	ofSetColor(80, 220, 180);
	ofSetLineWidth(2.0f);

	// - connect consecutive samples with lines for a continuous trace
	for (int i = 0; i < bufferSize - 1; i++) {
		float x1 = i * xStep;
		float x2 = (i + 1) * xStep;
		// - invert sample so positive is upward, scale to amplitude
		float y1 = centerY - waveform[i] * amplitude;
		float y2 = centerY - waveform[i + 1] * amplitude;
		ofDrawLine(x1, y1, x2, y2);
	}

	// - center line for reference
	ofSetColor(255, 255, 255, 40);
	ofSetLineWidth(1.0f);
	ofDrawLine(0, centerY, width, centerY);
}
