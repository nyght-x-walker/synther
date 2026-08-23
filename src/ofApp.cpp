#include "ofApp.h"

void ofApp::setup() {
    int bufferSize = 512;

    sampleRate = 44100;
    phase = 0.0f;
    volume = 0.1f;
    noteOn = false;

    phaseAdder = (440.0f / sampleRate) * glm::two_pi<float>();

    ofSoundStreamSettings settings;

    auto devices = soundStream.getMatchingDevices("default");

    if (!devices.empty()) {
        settings.setOutDevice(devices[0]);
    }

    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = bufferSize;

    soundStream.setup(settings);

    for (int i = 0; i < 9; i++) {
        keyDown[i] = false;
    }
}

void ofApp::update() {
}

void ofApp::draw() {

    ofBackground(30);

    char keys[9] = {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L'};

    string notes[9] = {
        "C4", "D4", "E4", "F4", "G4",
        "A4", "B4", "C5", "D5"
    };

    float frequencies[9] = {
        261.63f, 293.66f, 329.63f,
        349.23f, 392.00f, 440.00f,
        493.88f, 523.25f, 587.33f
    };

    int keyWidth = 90;
    int keyHeight = 250;

    int totalWidth = 9 * keyWidth;
    int startX = (ofGetWidth() - totalWidth) / 2;
    int startY = 250;

    for (int i = 0; i < 9; i++) {

        int x = startX + i * keyWidth;

        // Key color
        if (keyDown[i]) {
            ofSetColor(0);
        }
        else {
            float hue = i * (255.0f / 9.0f);
            ofSetColor(ofColor::fromHsb(hue, 200, 255));
        }

        ofDrawRectangle(x, startY, keyWidth, keyHeight);

        // White outline
        ofNoFill();
        ofSetColor(255);
        ofSetLineWidth(2);
        ofDrawRectangle(x, startY, keyWidth, keyHeight);
        ofFill();

        // Text
        ofSetColor(255);

        ofDrawBitmapString(
            string(1, keys[i]),
            x + 40,
            startY + 60
        );

        ofDrawBitmapString(
            notes[i],
            x + 35,
            startY + 120
        );

        ofDrawBitmapString(
            ofToString(frequencies[i], 2) + " Hz",
            x + 12,
            startY + 180
        );
    }
}

void ofApp::keyPressed(int key) {

    if (key == 'a') {
        phaseAdder = (261.63f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[0] = true;
    }
    else if (key == 's') {
        phaseAdder = (293.66f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[1] = true;
    }
    else if (key == 'd') {
        phaseAdder = (329.63f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[2] = true;
    }
    else if (key == 'f') {
        phaseAdder = (349.23f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[3] = true;
    }
    else if (key == 'g') {
        phaseAdder = (392.00f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[4] = true;
    }
    else if (key == 'h') {
        phaseAdder = (440.00f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[5] = true;
    }
    else if (key == 'j') {
        phaseAdder = (493.88f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[6] = true;
    }
    else if (key == 'k') {
        phaseAdder = (523.25f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[7] = true;
    }
    else if (key == 'l') {
        phaseAdder = (587.33f / sampleRate) * glm::two_pi<float>();
        noteOn = true;
        keyDown[8] = true;
    }
}

void ofApp::keyReleased(int key) {
    if (key == 'a') {
        keyDown[0] = false;
        noteOn = false;
    }
    else if (key == 's') {
        keyDown[1] = false;
        noteOn = false;
    }
    else if (key == 'd') {
        keyDown[2] = false;
        noteOn = false;
    }
    else if (key == 'f') {
        keyDown[3] = false;
        noteOn = false;
    }
    else if (key == 'g') {
        keyDown[4] = false;
        noteOn = false;
    }
    else if (key == 'h') {
        keyDown[5] = false;
        noteOn = false;
    }
    else if (key == 'j') {
        keyDown[6] = false;
        noteOn = false;
    }
    else if (key == 'k') {
        keyDown[7] = false;
        noteOn = false;
    }
    else if (key == 'l') {
        keyDown[8] = false;
        noteOn = false;
    }
}

void ofApp::audioOut(ofSoundBuffer& buffer) {
    for (size_t i = 0; i < buffer.getNumFrames(); i++) {

        phase += phaseAdder;

        if (phase > glm::two_pi<float>()) {
            phase -= glm::two_pi<float>();
        }

        float sample = sin(phase);

        if (!noteOn) {
            sample = 0.0f;
        }

        buffer[i * buffer.getNumChannels()] = sample * volume;
        buffer[i * buffer.getNumChannels() + 1] = sample * volume;
    }
}