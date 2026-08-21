#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// 44.1 kHz, stereo output, no input, 256-sample buffer.
	sampleRate = 44100;
	voice.setSampleRate(sampleRate);

	ofSoundStreamSettings settings;
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = 256;
	soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	// On-screen instructions and current note state
	ofBackground(30);
	ofSetColor(255);
	ofDrawBitmapString("Synther - SineVoice", 20, 30);
	ofDrawBitmapString("Hold A S D F G H J K L to play C4 to D5", 20, 50);

	if (noteHeld) {
		ofDrawBitmapString("Now playing: " + ofToString(currentFreq, 2) + " Hz", 20, 70);
	} else {
		ofDrawBitmapString("No note held", 20, 70);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	soundStream.stop();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& buffer) {
	// Render one mono sample per frame, copied to all channels.
	voice.render(buffer.getBuffer().data(), buffer.getNumFrames(), buffer.getNumChannels());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// A key sounds while held. Fixed velocity.
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

	voice.noteOn(currentFreq, 0.5f);
	noteHeld = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	// Stop the note when any mapped key is released.
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
			voice.noteOff();
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
