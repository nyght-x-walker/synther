#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

    void audioOut(ofSoundBuffer& buffer);

    ofSoundStream soundStream;

    int sampleRate;
    float phase;
    float phaseAdder;
    float volume;

    bool noteOn;
};