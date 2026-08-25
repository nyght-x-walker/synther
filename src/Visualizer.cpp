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
		float target = ofClamp(mag * 4.0f, 0.0f, 1.0f);
		spectrum[k] = spectrum[k] * 0.65f + target * 0.35f;
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

	float barW = 14.0f;
	float barH = height * 0.36f;
	float barX = width - barW - 18.0f;
	float barY = centerY - barH * 0.5f;
	ofSetColor(255, 255, 255, 28);
	ofDrawRectRounded(barX, barY, barW, barH, 4);
	ofSetColor(voiceColor, 190);
	float rmsH = rmsLevel * barH;
	float peakH = peakLevel * barH;
	ofDrawRectRounded(barX, barY + barH - rmsH, barW, rmsH, 4);
	ofSetColor(255, 255, 255, 45);
	for (int t = 1; t < 4; t++) {
		float y = barY + barH * (t / 4.0f);
		ofDrawLine(barX, y, barX + barW, y);
	}
	ofSetColor(255, 255, 255, 220);
	float peakY = barY + barH - peakH;
	ofDrawRectangle(barX - 3, peakY - 1, barW + 6, 2);

	if (!spectrum.empty()) {
		float specY = height - 28.0f;
		float specH = 58.0f;
		int bins = std::min(64, static_cast<int>(spectrum.size()));
		float barStep = (width - 40.0f) / static_cast<float>(bins);
		float barWidth = barStep * 0.72f;
		for (int i = 0; i < bins; i++) {
			float t = static_cast<float>(i) / bins;
			float logT = std::pow(t, 1.6f);
			int idx = static_cast<int>(logT * (spectrum.size() - 1));
			idx = ofClamp(idx, 0, static_cast<int>(spectrum.size()) - 1);
			float mag = spectrum[idx];
			float h = mag * specH;
			float x = 20.0f + i * barStep;
			float y = specY - h;
			ofSetColor(voiceColor, 85 + static_cast<int>(mag * 130));
			ofDrawRectRounded(x, y, barWidth, h, 2);
		}
		ofSetColor(255, 255, 255, 25);
		ofDrawLine(20, specY, width - 20, specY);
	}
}
