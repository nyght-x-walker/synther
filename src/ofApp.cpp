#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// 44.1 kHz, stereo output, no input, 256-sample buffer.
	// - all voices share the same sample rate
	// - sine is the default voice
	sampleRate = 44100;
	sineVoice.setSampleRate(sampleRate);
	squareVoice.setSampleRate(sampleRate);
	noiseVoice.setSampleRate(sampleRate);
	activeVoice = &sineVoice;

	ofSoundStreamSettings settings;
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = 256;
	soundStream.setup(settings);

	// - visualizer stores one buffer for drawing
	// - lastAudioBuffer mirrors the audio thread output for the main thread
	lastBufferSize = settings.bufferSize;
	lastNumChannels = settings.numOutputChannels;
	visualizer.setup(lastBufferSize, lastNumChannels);
	lastAudioBuffer.assign(lastBufferSize * lastNumChannels, 0.0f);
}

//--------------------------------------------------------------
void ofApp::update() {
	// - pass the latest audio buffer to the visualizer for drawing
	// - copy under lock to avoid tearing between audio and main threads
	std::vector<float> copy;
	{
		std::lock_guard<std::mutex> lock(audioMutex);
		copy = lastAudioBuffer;
	}
	visualizer.update(copy);
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Current voice and key hints
	ofBackground(30);

	// - draw the waveform behind the text overlay
	visualizer.draw();

	ofSetColor(255);

	std::string voiceName = "Sine";
	if (activeVoice == &squareVoice) voiceName = "Square";
	else if (activeVoice == &noiseVoice) voiceName = "Noise";

	ofDrawBitmapString("Synther - " + voiceName + " Voice", 20, 30);
	ofDrawBitmapString("Keys 1: Sine  2: Square  3: Noise", 20, 50);
	ofDrawBitmapString("Hold A S D F G H J K L to play C4 to D5", 20, 70);

	if (noteHeld) {
		ofDrawBitmapString("Now playing: " + ofToString(currentFreq, 2) + " Hz (" + voiceName + ")", 20, 90);
	} else {
		ofDrawBitmapString("No note held", 20, 90);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	soundStream.stop();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& buffer) {
	// Clear buffer to silence, then render the active voice.
	// - mixing later can sum multiple voices into the same buffer
	auto& out = buffer.getBuffer();
	std::fill(out.begin(), out.end(), 0.0f);

	if (activeVoice) {
		activeVoice->render(out.data(), buffer.getNumFrames(), buffer.getNumChannels());
	}

	// - copy the rendered buffer for the visualizer
	// - keep the copy under lock so update() sees a consistent snapshot
	{
		std::lock_guard<std::mutex> lock(audioMutex);
		lastAudioBuffer = out;
		lastBufferSize = buffer.getNumFrames();
		lastNumChannels = buffer.getNumChannels();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// Voice selection and note triggering
	// - 1/2/3 switch the active voice
	// - A-L play notes on the active voice
	if (key == '1' || key == '2' || key == '3') {
		SoundSource* nextVoice = nullptr;
		if (key == '1') nextVoice = &sineVoice;
		else if (key == '2') nextVoice = &squareVoice;
		else if (key == '3') nextVoice = &noiseVoice;

		if (nextVoice != activeVoice) {
			// Retrigger the held note on the new voice so the switch is audible
			bool wasHeld = noteHeld;
			float heldFreq = currentFreq;
			if (wasHeld && activeVoice) {
				activeVoice->noteOff();
			}
			activeVoice = nextVoice;
			if (wasHeld) {
				activeVoice->noteOn(heldFreq, 0.5f);
			}
		}
		return;
	}

	switch (key) {
		case 'a': currentFreq = 261.63f; break; // C4
		case 's': currentFreq = 293.66f; break; // D4
		case 'd': currentFreq = 329.63f; break; // E4
		case 'f': currentFreq = 349.23f; break; // F4
		case 'g': currentFreq = 392.00f; break; // G4
		case 'h': currentFreq = 440.00f; break; // A4
		case 'j': currentFreq = 493.88f; break; // B4
		case 'k': currentFreq = 523.25f; break; // C5
		case 'l': currentFreq = 587.33f; break; // D5
		default: return;
	}

	if (activeVoice) {
		activeVoice->noteOn(currentFreq, 0.5f);
	}
	noteHeld = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	// Stop the note when any mapped key is released
	switch (key) {
		case 'a':
		case 's':
		case 'd':
		case 'f':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'l':
			if (activeVoice) {
				activeVoice->noteOff();
			}
			noteHeld = false;
			break;
		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
