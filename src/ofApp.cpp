#include "ofApp.h"
#include <algorithm>
#include <cmath>

static std::string freqToNote(float freq) {
    if (freq <= 0.0f) return "-";
    const std::string names[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    float n = 12.0f * std::log2(freq / 440.0f) + 9.0f;
    int note = static_cast<int>(std::round(n));
    int octave = static_cast<int>(std::floor(note / 12.0f)) + 4;
    int idx = ((note % 12) + 12) % 12;
    float cents = (n - note) * 100.0f;
    std::string s = names[idx] + ofToString(octave);
    if (std::abs(cents) > 1.0f) {
        s += (cents > 0 ? " +" : " ") + ofToString(cents, 0) + "c";
    }
    return s;
}

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

	lastBufferSize = settings.bufferSize;
	lastNumChannels = settings.numOutputChannels;
	visualizer.setup(lastBufferSize, lastNumChannels);
	lastAudioBuffer.assign(lastBufferSize * lastNumChannels, 0.0f);

	ofSetWindowTitle("Synther - Capricorn");
	delayLine.assign(8192, 0.0f);
	delayPos = 0;
	presets = { {"Warm", 0, 0.0f, 0.8f}, {"Bright", 1, 5.0f, 0.9f}, {"Noisy", 2, -3.0f, 0.7f}, {"Deep", 0, -7.0f, 0.85f} };
	particles.assign(18, {0, 0, 0, 0, 0});
	octave = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
	ofColor col = ofColor(80, 220, 180);
	if (activeVoice == &squareVoice) col = ofColor(255, 180, 60);
	else if (activeVoice == &noiseVoice) col = ofColor(180, 180, 180);
	float hueShift = ofMap(mouseX, 0, static_cast<float>(ofGetWidth()), -5.0f, 5.0f, true);
	col.setHue(static_cast<int>(fmod(col.getHue() + hueShift + 255, 255)));
	visualizer.setVoiceColor(col);

	float scale = noteHeld ? (currentVelocity * 1.4f + 0.3f) : 0.5f;
	visualizer.setAmplitudeScale(scale);

	std::vector<float> copy;
	{
		std::lock_guard<std::mutex> lock(audioMutex);
		copy = lastAudioBuffer;
	}
	visualizer.update(copy);

	if (activeVoice == &noiseVoice && noteHeld) {
		for (auto &p : particles) {
			p.x += p.vx;
			p.y += p.vy;
			p.life -= 0.03f;
			if (p.life <= 0.0f) {
				p.x = static_cast<float>(ofGetWidth()) * 0.5f;
				p.y = static_cast<float>(ofGetHeight()) * 0.5f;
				float a = ofRandom(0, TWO_PI);
				float s = ofRandom(1.0f, 4.0f);
				p.vx = std::cos(a) * s;
				p.vy = std::sin(a) * s;
				p.life = 1.0f;
			}
		}
	}

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

	if (activeVoice == &noiseVoice && noteHeld) {
		for (auto &p : particles) {
			ofSetColor(180, 180, 180, static_cast<int>(p.life * 120));
			ofDrawCircle(p.x, p.y, 2.5f);
		}
	}

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

	float availableW = static_cast<float>(ofGetWidth()) - 40.0f;
	int keyWidth = static_cast<int>(availableW / 9.0f);
	keyWidth = ofClamp(keyWidth, 56, 110);
	int keyHeight = static_cast<int>(keyWidth * 1.9f);
	keyHeight = ofClamp(keyHeight, 110, 210);
	int totalWidth = 9 * keyWidth;
	int startX = (ofGetWidth() - totalWidth) / 2;
	int startY = ofGetHeight() - keyHeight - 80;

	if (octave != 0) {
		ofSetColor(80, 220, 180, std::abs(octave) * 12 + 10);
		ofDrawRectRounded(startX - 8, startY - 18, totalWidth + 16, keyHeight + 36, 12);
	}

	int pitchClass[9] = {0, 2, 4, 5, 7, 9, 11, 0, 2};
	for (int i = 0; i < 9; i++) {
		int x = startX + i * keyWidth;
		float glow = keyGlow[i];
		float hue = pitchClass[i] * (255.0f / 12.0f);
		ofColor base = ofColor::fromHsb(hue, 170, 230);
		ofColor col = base;
		ofColor voiceCol = ofColor(80, 220, 180);
		if (activeVoice == &squareVoice) voiceCol = ofColor(255, 180, 60);
		else if (activeVoice == &noiseVoice) voiceCol = ofColor(180, 180, 180);
		if (keyDown[i]) {
			col = voiceCol;
			if (glow < 0.7f) {
				col = voiceCol.lerp(ofColor(12, 12, 14), 0.15f);
			}
		} else if (glow > 0.0f) {
			col = base.lerp(ofColor(255), glow * 0.45f);
		}
		ofSetColor(col);
		ofDrawRectRounded(x, startY, keyWidth, keyHeight, 10);

		ofNoFill();
		if (keyDown[i]) {
			ofSetColor(255, 255, 255, 140);
		} else if (glow > 0.0f) {
			ofSetColor(80, 220, 180, static_cast<int>(glow * 110));
		} else {
			ofSetColor(255, 255, 255, 22);
		}
		ofSetLineWidth(keyDown[i] ? 2.8f : 1.2f);
		ofDrawRectRounded(x, startY, keyWidth, keyHeight, 10);
		ofFill();

		if (glow > 0.0f) {
			ofSetColor(80, 220, 180, static_cast<int>(glow * 50));
			ofDrawRectRounded(x - 2, startY - 2, keyWidth + 4, keyHeight + 4, 12);
		}

		if (keyDown[i]) {
			ofSetColor(255, 255, 255, 210);
			ofDrawCircle(x + keyWidth * 0.5f, startY + 18, 4);
		}

		ofSetColor(255);
		ofDrawBitmapString(std::string(1, keys[i]), x + 40, startY + 40);
		ofDrawBitmapString(notes[i], x + 35, startY + 90);
		ofDrawBitmapString(ofToString(frequencies[i], 2) + " Hz", x + 12, startY + 140);
	}

	float presetY = startY - 38;
	float presetW = 70;
	float presetH = 18;
	float presetX = (static_cast<float>(ofGetWidth()) - 4 * presetW - 30) * 0.5f;
	for (int i = 0; i < 4; i++) {
		bool sel = (currentPreset == i);
		ofColor c = (presets[i].voice == 0 ? ofColor(80, 220, 180) : presets[i].voice == 1 ? ofColor(255, 180, 60) : ofColor(180, 180, 180));
		ofSetColor(c, sel ? 210 : 85);
		ofDrawRectRounded(presetX + i * (presetW + 10), presetY, presetW, presetH, 4);
		ofSetColor(255, sel ? 255 : 160);
		ofDrawBitmapString(presets[i].name, presetX + i * (presetW + 10) + 8, presetY + 13);
	}
	ofSetColor(255, 255, 255, 90);
	ofDrawBitmapString("Presets: 4 Warm  5 Bright  6 Noisy  7 Deep  |  Z/X Octave " + ofToString(octave), presetX, presetY - 12);

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
		std::string noteName = freqToNote(currentFreq);
		ofDrawBitmapString("Now playing: " + noteName + " " + ofToString(currentFreq, 1) + " Hz (" + voiceName + ") vol " + ofToString(currentVelocity, 2), 20, 118);
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
	auto& out = buffer.getBuffer();
	std::fill(out.begin(), out.end(), 0.0f);

	if (activeVoice) {
		activeVoice->render(out.data(), buffer.getNumFrames(), buffer.getNumChannels());
	}

	if (!delayLine.empty()) {
		int frames = buffer.getNumFrames();
		int nChannels = buffer.getNumChannels();
		for (int i = 0; i < frames; i++) {
			for (int c = 0; c < nChannels; c++) {
				int idx = i * nChannels + c;
				float dry = out[idx];
				float delayed = delayLine[delayPos];
				float wet = dry * 0.8f + delayed * 0.28f;
				delayLine[delayPos] = dry * 0.35f + delayed * 0.55f;
				out[idx] = wet;
			}
			delayPos = (delayPos + 1) % static_cast<int>(delayLine.size());
		}
	}
	{
		std::lock_guard<std::mutex> lock(audioMutex);
		lastAudioBuffer = out;
		lastBufferSize = buffer.getNumFrames();
		lastNumChannels = buffer.getNumChannels();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'z' || key == 'Z') {
		octave = std::max(-1, octave - 1);
		return;
	}
	if (key == 'x' || key == 'X') {
		octave = std::min(2, octave + 1);
		return;
	}
	if (key >= '4' && key <= '7') {
		int idx = key - '4';
		if (idx >= 0 && idx < static_cast<int>(presets.size())) {
			Preset &p = presets[idx];
			currentPreset = idx;
			if (p.voice == 0) activeVoice = &sineVoice;
			else if (p.voice == 1) activeVoice = &squareVoice;
			else if (p.voice == 2) activeVoice = &noiseVoice;
			mousePitchOffset = p.pitch;
			mouseVolume = p.vol;
			if (noteHeld && activeVoice) {
				activeVoice->noteOn(currentFreq, currentVelocity);
			}
			return;
		}
	}
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
	float finalFreq = (currentFreq * std::pow(2.0f, static_cast<float>(octave))) + mousePitchOffset;
	float finalVelocity = baseVelocity * std::pow(mouseVolume, 1.5f);
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
