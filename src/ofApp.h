#pragma once

#include "ofMain.h"
#include "SineVoice.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void audioOut(ofSoundBuffer& buffer) override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;

		// Audio
		ofSoundStream soundStream; // audio stream to the device
		SineVoice voice;           // sine oscillator voice

		int sampleRate = 44100;    // audio sample rate in Hz
		bool noteHeld = false;     // true while a mapped key is held down
		float currentFreq = 0.0f;  // frequency of the note currently held
};
