#pragma once

#include "ofMain.h"
#include "SineVoice.h"
#include "SquareVoice.h"
#include "NoiseVoice.h"
#include "Visualizer.h"
#include <mutex>
#include <vector>

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
		// three voices share the SoundSource interface
		// activeVoice points to the selected voice
		// future mixing can sum multiple voices into the buffer
		ofSoundStream soundStream;
		SineVoice sineVoice;
		SquareVoice squareVoice;
		NoiseVoice noiseVoice;
		SoundSource* activeVoice = nullptr;

		int sampleRate = 44100;    // audio sample rate in Hz
		bool noteHeld = false;     // true while a mapped key is held down
		float currentFreq = 0.0f;  // frequency of the note currently held
		float currentVelocity = 0.5f; // velocity of the current note 0..1

		bool keyDown[9] = {
			false, false, false,
			false, false, false,
			false, false, false
		};
		float keyGlow[9] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

		// Mouse control
		float mousePitchOffset = 0.0f;
		float mouseVolume = 0.8f;
		float mouseX = 0.0f;
		float mouseY = 0.0f;
		int octave = 0;

		std::vector<float> delayLine;
		int delayPos = 0;

		struct Preset { std::string name; int voice; float pitch; float vol; };
		std::vector<Preset> presets;
		int currentPreset = -1;

		struct Particle { float x; float y; float vx; float vy; float life; };
		std::vector<Particle> particles;

		// Visualizer
		// stores recent audio for drawing
		// lastAudioBuffer is shared between audio and main threads
		Visualizer visualizer;
		std::vector<float> lastAudioBuffer;
		int lastBufferSize = 256;
		int lastNumChannels = 2;
		std::mutex audioMutex;
};
