#include "ofApp.h"
#include <algorithm>

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
	// - choose voice color so the waveform matches the active voice
	// - sine teal, square amber, noise grey
	ofColor col = ofColor(80, 220, 180);
	if (activeVoice == &squareVoice) col = ofColor(255, 180, 60);
	else if (activeVoice == &noiseVoice) col = ofColor(180, 180, 180);
	visualizer.setVoiceColor(col);

	// - scale waveform amplitude with velocity so louder notes look larger
	// - keep a minimum visible size when no note is held
	float scale = noteHeld ? (currentVelocity * 1.4f + 0.3f) : 0.5f;
	visualizer.setAmplitudeScale(scale);

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

	ofDrawBitmapString("Synther - " + voiceName + " Voice  [Capricorn]", 20, 30);
	ofDrawBitmapString("Keys 1: Sine  2: Square  3: Noise", 20, 50);
	ofDrawBitmapString("Hold A S D F G H J K L to play C4 to D5", 20, 70);
	ofDrawBitmapString("Mouse X: pitch offset " + ofToString(mousePitchOffset, 1) + " Hz  Y: volume " + ofToString(mouseVolume, 2), 20, 90);

	if (noteHeld) {
		ofDrawBitmapString("Now playing: " + ofToString(currentFreq, 2) + " Hz (" + voiceName + ") vol " + ofToString(currentVelocity, 2), 20, 110);
	} else {
		ofDrawBitmapString("No note held - move mouse to adjust pitch/volume for next note", 20, 110);
	}

	ofDrawBitmapString("Tip: top = loud, bottom = quiet, left = -20 Hz, right = +20 Hz", 20, 130);


	// - keyboard labels match the mapped A-L note keys
	// - each key stores its base note and frequency for display
	char keys[9] = {
		'A', 'S', 'D', 'F', 'G',
		'H', 'J', 'K', 'L'
	};

	std::string notes[9] = {
		"C4", "D4", "E4", "F4", "G4",
		"A4", "B4", "C5", "D5"
	};

	float frequencies[9] = {
		261.63f, 293.66f, 329.63f,
		349.23f, 392.00f, 440.00f,
		493.88f, 523.25f, 587.33f
	};


	// - draw nine connected rectangular keys near the bottom of the window
	// - center the complete keyboard horizontally
	int keyWidth = 90;
	int keyHeight = 180;
	int totalWidth = 9 * keyWidth;

	int startX = (ofGetWidth() - totalWidth) / 2;
	int startY = ofGetHeight() - keyHeight - 100;


	for (int i = 0; i < 9; i++) {
		int x = startX + i * keyWidth;

		// - held keys become black
		// - inactive keys use evenly spaced bright colors
		if (keyDown[i]) {
			ofSetColor(0);
		} else {
			float hue = i * (255.0f / 9.0f);
			ofSetColor(ofColor::fromHsb(hue, 210, 255));
		}

		ofDrawRectangle(x, startY, keyWidth, keyHeight);


		// - add a white outline to separate adjacent keys
		ofNoFill();
		ofSetColor(255);
		ofSetLineWidth(2);
		ofDrawRectangle(x, startY, keyWidth, keyHeight);
		ofFill();


		// - show keyboard key, musical note, and base frequency
		ofSetColor(255);

		ofDrawBitmapString(
			std::string(1, keys[i]),
			x + 40,
			startY + 40
		);

		ofDrawBitmapString(
			notes[i],
			x + 35,
			startY + 90
		);

		ofDrawBitmapString(
			ofToString(frequencies[i], 2) + " Hz",
			x + 12,
			startY + 140
		);
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
				activeVoice->noteOn(heldFreq, currentVelocity);
			}
		}
		return;
	}

	switch (key) {
		case 'a':
			currentFreq = 261.63f;
			keyDown[0] = true;
			break;

		case 's':
			currentFreq = 293.66f;
			keyDown[1] = true;
			break;

		case 'd':
			currentFreq = 329.63f;
			keyDown[2] = true;
			break;

		case 'f':
			currentFreq = 349.23f;
			keyDown[3] = true;
			break;

		case 'g':
			currentFreq = 392.00f;
			keyDown[4] = true;
			break;

		case 'h':
			currentFreq = 440.00f;
			keyDown[5] = true;
			break;

		case 'j':
			currentFreq = 493.88f;
			keyDown[6] = true;
			break;

		case 'k':
			currentFreq = 523.25f;
			keyDown[7] = true;
			break;

		case 'l':
			currentFreq = 587.33f;
			keyDown[8] = true;
			break;

		default:
			return;
	}

	float baseVelocity = 0.5f;
	float finalFreq = currentFreq + mousePitchOffset;
	float finalVelocity = baseVelocity * mouseVolume;
	finalFreq = std::max(20.0f, finalFreq);
	finalVelocity = ofClamp(finalVelocity, 0.0f, 1.0f);

	currentFreq = finalFreq;
	currentVelocity = finalVelocity;
	if (activeVoice) {
		activeVoice->noteOn(currentFreq, currentVelocity);
	}
	noteHeld = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

    switch (key) {

        case 'a':
            keyDown[0] = false;
            break;

        case 's':
            keyDown[1] = false;
            break;

        case 'd':
            keyDown[2] = false;
            break;

        case 'f':
            keyDown[3] = false;
            break;

        case 'g':
            keyDown[4] = false;
            break;

        case 'h':
            keyDown[5] = false;
            break;

        case 'j':
            keyDown[6] = false;
            break;

        case 'k':
            keyDown[7] = false;
            break;

        case 'l':
            keyDown[8] = false;
            break;

        default:
            return;
    }

    if (activeVoice) {
        activeVoice->noteOff();
    }

    noteHeld = false;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	mousePitchOffset = ofMap(x, 0, ofGetWidth(), -20.0f, 20.0f, true);
	mouseVolume = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f, true);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	mousePitchOffset = ofMap(x, 0, ofGetWidth(), -20.0f, 20.0f, true);
	mouseVolume = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f, true);
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
