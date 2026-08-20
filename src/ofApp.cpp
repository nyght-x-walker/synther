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
}

void ofApp::update() {
}

void ofApp::draw() {
}

void ofApp::keyPressed(int key) {

    if (key == 'a') {
        phaseAdder = (261.63f / sampleRate) * glm::two_pi<float>(); // C4
        noteOn = true;
    }
    else if (key == 's') {
        phaseAdder = (293.66f / sampleRate) * glm::two_pi<float>(); // D4
        noteOn = true;
    }
    else if (key == 'd') {
        phaseAdder = (329.63f / sampleRate) * glm::two_pi<float>(); // E4
        noteOn = true;
    }
    else if (key == 'f') {
        phaseAdder = (349.23f / sampleRate) * glm::two_pi<float>(); // F4
        noteOn = true;
    }
    else if (key == 'g') {
        phaseAdder = (392.00f / sampleRate) * glm::two_pi<float>(); // G4
        noteOn = true;
    }
    else if (key == 'h') {
        phaseAdder = (440.00f / sampleRate) * glm::two_pi<float>(); // A4
        noteOn = true;
    }
    else if (key == 'j') {
        phaseAdder = (493.88f / sampleRate) * glm::two_pi<float>(); // B4
        noteOn = true;
    }
    else if (key == 'k') {
        phaseAdder = (523.25f / sampleRate) * glm::two_pi<float>(); // C5
        noteOn = true;
    }
    else if (key == 'l') {
        phaseAdder = (587.33f / sampleRate) * glm::two_pi<float>(); // D5
        noteOn = true;
    }
}

void ofApp::keyReleased(int key) {

    if (key == 'a' ||
        key == 's' ||
        key == 'd' ||
        key == 'f' ||
        key == 'g' ||
        key == 'h' ||
        key == 'j' ||
        key == 'k' ||
        key == 'l') {

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