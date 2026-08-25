#include "ofApp.h"
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup() {
	// 44.1 kHz, stereo output, no input, 256-sample buffer.
	// all voices share the same sample rate
	// sine is the default voice
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

	// visualizer stores one buffer for drawing
	// lastAudioBuffer mirrors the audio thread output for the main thread
	lastBufferSize = settings.bufferSize;
	lastNumChannels = settings.numOutputChannels;
	visualizer.setup(lastBufferSize, lastNumChannels);
	lastAudioBuffer.assign(lastBufferSize * lastNumChannels, 0.0f);
}

//--------------------------------------------------------------
void ofApp::update() {
	ofColor col = ofColor(80, 220, 180);
	if (activeVoice == &squareVoice) col = ofColor(255, 180, 60);
	else if (activeVoice == &noiseVoice) col = ofColor(180, 180, 180);
	visualizer.setVoiceColor(col);

	float scale = noteHeld ? (currentVelocity * 1.4f + 0.3f) : 0.5f;
	visualizer.setAmplitudeScale(scale);

	std::vector<float> copy;
	{
		std::lock_guard<std::mutex> lock(audioMutex);
		copy = lastAudioBuffer;
	}
	visualizer.update(copy);

	for (int i = 0; i < 9; i++) {
		if (keyGlow[i] > 0.0f) {
			keyGlow[i] -= 0.12f;
			if (keyGlow[i] < 0.0f) keyGlow[i] = 0.0f;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackgroundGradient(ofColor(48, 38, 62), ofColor(20, 20, 26), OF_GRADIENT_BAR);
	visualizer.draw();

	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, 18);
	ofSetLineWidth(1.0f);
	ofDrawLine(mouseX, 0, mouseX, static_cast<float>(ofGetHeight()));
	ofDrawLine(0, mouseY, static_cast<float>(ofGetWidth()), mouseY);
	ofSetColor(255, 255, 255, 35);
	ofNoFill();
	ofDrawCircle(mouseX, mouseY, 4);
	ofFill();

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

	int keyWidth = 90;
	int keyHeight = 180;
	int totalWidth = 9 * keyWidth;
	int startX = (ofGetWidth() - totalWidth) / 2;
	int startY = ofGetHeight() - keyHeight - 100;

	for (int i = 0; i < 9; i++) {
		int x = startX + i * keyWidth;
		float glow = keyGlow[i];
		ofColor base = ofColor::fromHsb(i * (255.0f / 9.0f), 210, 255);
		ofColor col = base;
		if (keyDown[i]) {
			col = ofColor(12, 12, 14);
			if (glow > 0.0f) {
				col = col.lerp(ofColor(80, 220, 180), glow * 0.6f);
			}
		} else if (glow > 0.0f) {
			col = base.lerp(ofColor(255), glow * 0.5f);
		}
		ofSetColor(col);
		ofDrawRectRounded(x, startY, keyWidth, keyHeight, 10);

		ofNoFill();
		ofSetColor(255, 255, 255, keyDown[i] ? 90 : 22);
		if (glow > 0.0f) {
			ofSetColor(80, 220, 180, static_cast<int>(glow * 120));
		}
		ofSetLineWidth(keyDown[i] ? 2.5f : 1.2f);
		ofDrawRectRounded(x, startY, keyWidth, keyHeight, 10);
		ofFill();

		if (glow > 0.0f) {
			ofSetColor(80, 220, 180, static_cast<int>(glow * 55));
			ofDrawRectRounded(x - 2, startY - 2, keyWidth + 4, keyHeight + 4, 12);
		}

		ofSetColor(255);
		ofDrawBitmapString(std::string(1, keys[i]), x + 40, startY + 40);
		ofDrawBitmapString(notes[i], x + 35, startY + 90);
		ofDrawBitmapString(ofToString(frequencies[i], 2) + " Hz", x + 12, startY + 140);
	}

	std::string voiceName = "Sine";
	if (activeVoice == &squareVoice) voiceName = "Square";
	else if (activeVoice == &noiseVoice) voiceName = "Noise";

	ofSetColor(18, 18, 22, 175);
	ofDrawRectangle(0, 0, static_cast<float>(ofGetWidth()), 148);

	ofSetColor(255, 255, 255, 22);
	ofSetLineWidth(1.0f);
	ofDrawLine(0, 148, static_cast<float>(ofGetWidth()), 148);

	struct Pill { std::string label; bool active; ofColor col; };
	Pill pills[3] = {
		{"1 Sine", activeVoice == &sineVoice, ofColor(80, 220, 180)},
		{"2 Square", activeVoice == &squareVoice, ofColor(255, 180, 60)},
		{"3 Noise", activeVoice == &noiseVoice, ofColor(180, 180, 180)}
	};
	float pillW = 86;
	float pillH = 22;
	float pillX = 20;
	float pillY = 42;
	for (int i = 0; i < 3; i++) {
		ofColor bg = pills[i].active ? pills[i].col : ofColor(55, 55, 60);
		int alpha = pills[i].active ? 230 : 90;
		ofSetColor(bg, alpha);
		ofDrawRectRounded(pillX + i * (pillW + 10), pillY, pillW, pillH, 8);
		ofSetColor(255, pills[i].active ? 255 : 170);
		ofDrawBitmapString(pills[i].label, pillX + i * (pillW + 10) + 14, pillY + 15);
	}

	ofSetColor(255);
	ofDrawBitmapString("Synther - " + voiceName + " Voice  [Capricorn]", 20, 18);
	ofDrawBitmapString("Hold A S D F G H J K L to play C4 to D5", 20, 78);
	ofDrawBitmapString("Mouse X: pitch offset " + ofToString(mousePitchOffset, 1) + " Hz  Y: volume " + ofToString(mouseVolume, 2), 20, 98);
	if (noteHeld) {
		ofDrawBitmapString("Now playing: " + ofToString(currentFreq, 2) + " Hz (" + voiceName + ") vol " + ofToString(currentVelocity, 2), 20, 118);
	} else {
		ofDrawBitmapString("No note held - move mouse to adjust pitch/volume for next note", 20, 118);
	}
	ofSetColor(255, 255, 255, 150);
	ofDrawBitmapString("Tip: top = loud, bottom = quiet, left = -20 Hz, right = +20 Hz", 20, 138);
}

//--------------------------------------------------------------
void ofApp::exit() {
	soundStream.stop();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& buffer) {
	// Clear buffer to silence, then render the active voice.
	// mixing later can sum multiple voices into the same buffer
	auto& out = buffer.getBuffer();
	std::fill(out.begin(), out.end(), 0.0f);

	if (activeVoice) {
		activeVoice->render(out.data(), buffer.getNumFrames(), buffer.getNumChannels());
	}

	// copy the rendered buffer for the visualizer
	// keep the copy under lock so update() sees a consistent snapshot
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
	// 1/2/3 switch the active voice
	// A-L play notes on the active voice
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
			keyGlow[0] = 1.0f;
			break;
		case 's':
			currentFreq = 293.66f;
			keyDown[1] = true;
			keyGlow[1] = 1.0f;
			break;
		case 'd':
			currentFreq = 329.63f;
			keyDown[2] = true;
			keyGlow[2] = 1.0f;
			break;
		case 'f':
			currentFreq = 349.23f;
			keyDown[3] = true;
			keyGlow[3] = 1.0f;
			break;
		case 'g':
			currentFreq = 392.00f;
			keyDown[4] = true;
			keyGlow[4] = 1.0f;
			break;
		case 'h':
			currentFreq = 440.00f;
			keyDown[5] = true;
			keyGlow[5] = 1.0f;
			break;
		case 'j':
			currentFreq = 493.88f;
			keyDown[6] = true;
			keyGlow[6] = 1.0f;
			break;
		case 'k':
			currentFreq = 523.25f;
			keyDown[7] = true;
			keyGlow[7] = 1.0f;
			break;
		case 'l':
			currentFreq = 587.33f;
			keyDown[8] = true;
			keyGlow[8] = 1.0f;
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
	mouseX = static_cast<float>(x);
	mouseY = static_cast<float>(y);
	mousePitchOffset = ofMap(x, 0, ofGetWidth(), -20.0f, 20.0f, true);
	mouseVolume = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f, true);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	mouseX = static_cast<float>(x);
	mouseY = static_cast<float>(y);
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
