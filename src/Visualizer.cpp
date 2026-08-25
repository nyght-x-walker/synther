#include "Visualizer.h"
#include <cmath>
#include <algorithm>

void Visualizer::setup(int bufferSize, int nChannels) {
	// store dimensions for later mapping
	// allocate mono storage, one value per frame
	// allocate spectrum with half the buffer size
	this->bufferSize = bufferSize;
	this->nChannels = nChannels;
	waveform.assign(bufferSize, 0.0f);
	spectrum.assign(std::max(1, bufferSize / 2), 0.0f);
	voiceColor = ofColor(80, 220, 180);
	amplitudeScale = 1.0f;
	rmsLevel = 0.0f;
	peakLevel = 0.0f;
}

void Visualizer::setVoiceColor(ofColor color) {
	voiceColor = color;
}

void Visualizer::setAmplitudeScale(float scale) {
	// clamp to a readable range so quiet notes stay visible
	amplitudeScale = ofClamp(scale, 0.25f, 1.8f);
}

void Visualizer::update(const std::vector<float>& audioBuffer) {
	// keep internal waveform in sync with audio thread
	// handle interleaved input by taking first channel
	// clamp to expected size to avoid visual mismatch

	if (audioBuffer.empty() || bufferSize == 0) {
		rmsLevel = 0.0f;
		peakLevel = 0.0f;
		std::fill(spectrum.begin(), spectrum.end(), 0.0f);
		return;
	}

	int frames = static_cast<int>(audioBuffer.size() / std::max(1, nChannels));
	frames = std::min(frames, bufferSize);

	for (int i = 0; i < frames; i++) {
		// interleaved layout: frame * nChannels + channel
		waveform[i] = audioBuffer[i * nChannels];
	}

	// fill remainder with silence if buffer is short
	for (int i = frames; i < bufferSize; i++) {
		waveform[i] = 0.0f;
	}

	// compute RMS and peak for the amplitude bar
	float sumSq = 0.0f;
	float peak = 0.0f;
	for (int i = 0; i < bufferSize; i++) {
		float s = waveform[i];
		sumSq += s * s;
		peak = std::max(peak, std::abs(s));
	}
	rmsLevel = std::sqrt(sumSq / std::max(1, bufferSize));
	peakLevel = peak;

	// compute a lightweight magnitude spectrum for the bottom view
	// naive DFT, fast enough for 256 samples at 60 fps
	// keep only half the bins, magnitude normalized by buffer size
	int specSize = static_cast<int>(spectrum.size());
	for (int k = 0; k < specSize; k++) {
		float real = 0.0f;
		float imag = 0.0f;
		for (int n = 0; n < bufferSize; n++) {
			float angle = 2.0f * 3.14159265f * k * n / static_cast<float>(bufferSize);
			real += waveform[n] * std::cos(angle);
			imag -= waveform[n] * std::sin(angle);
		}
		float mag = std::sqrt(real * real + imag * imag) / static_cast<float>(bufferSize);
		// boost for visibility, clamp to 0..1
		spectrum[k] = ofClamp(mag * 4.0f, 0.0f, 1.0f);
	}
}

void Visualizer::draw() {
	if (waveform.empty() || bufferSize == 0) {
		return;
	}

	// layout: waveform centered, spectrum at bottom, amplitude bar on right
	float width = static_cast<float>(ofGetWidth());
	float height = static_cast<float>(ofGetHeight());
	float centerY = height * 0.50f;
	float amplitude = height * 0.22f * amplitudeScale;
	float xStep = width / static_cast<float>(std::max(1, bufferSize - 1));

	// waveform with voice-dependent color
	ofSetColor(voiceColor);
	ofSetLineWidth(2.0f);

	// connect consecutive samples with lines for a continuous trace
	for (int i = 0; i < bufferSize - 1; i++) {
		float x1 = i * xStep;
		float x2 = (i + 1) * xStep;
		// invert sample so positive is upward, scale to amplitude
		float y1 = centerY - waveform[i] * amplitude;
		float y2 = centerY - waveform[i + 1] * amplitude;
		ofDrawLine(x1, y1, x2, y2);
	}

	// center line for reference
	ofSetColor(255, 255, 255, 35);
	ofSetLineWidth(1.0f);
	ofDrawLine(0, centerY, width, centerY);

	// amplitude bar on the right edge
	// RMS fills the bar, peak is a short tick
	float barW = 16.0f;
	float barH = height * 0.34f;
	float barX = width - barW - 18.0f;
	float barY = centerY - barH * 0.5f;
	ofSetColor(255, 255, 255, 30);
	ofDrawRectangle(barX, barY, barW, barH);
	ofSetColor(voiceColor, 200);
	float rmsH = rmsLevel * barH;
	float peakH = peakLevel * barH;
	// RMS as filled portion from the bottom
	ofDrawRectangle(barX, barY + barH - rmsH, barW, rmsH);
	// peak as a white tick
	ofSetColor(255, 255, 255, 220);
	float peakY = barY + barH - peakH;
	ofDrawRectangle(barX - 2, peakY - 1, barW + 4, 2);

	// spectrum at the bottom
	// draw a compact bar view, decimated if many bins
	if (!spectrum.empty()) {
		float specY = height - 28.0f;
		float specH = 56.0f;
		int bins = std::min(64, static_cast<int>(spectrum.size()));
		float barStep = (width - 40.0f) / static_cast<float>(bins);
		float barWidth = barStep * 0.75f;

		for (int i = 0; i < bins; i++) {
			// map bin index to spectrum index to cover full range
			int idx = static_cast<int>(static_cast<float>(i) / bins * spectrum.size());
			float mag = spectrum[idx];
			float h = mag * specH;
			float x = 20.0f + i * barStep;
			float y = specY - h;

			// use voice color with alpha scaled by magnitude
			ofSetColor(voiceColor, 90 + static_cast<int>(mag * 120));
			ofDrawRectangle(x, y, barWidth, h);
		}

		// baseline for spectrum
		ofSetColor(255, 255, 255, 25);
		ofDrawLine(20, specY, width - 20, specY);
	}
}
