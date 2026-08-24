# Synther - Software Requirements

Version 1.0 - August 2026

## 1 Introduction

### 1.1 Purpose

This document describes the software requirements for Synther, a real time audio synthesizer built with openFrameworks. It defines what the application must do, the constraints it operates under, and the quality attributes expected from the implementation.

This specification is for the software itself and is intended for developers and users of the application.

### 1.2 Scope

Synther is a desktop instrument that generates sound in real time, accepts keyboard and mouse input, and visualizes audio output. The first release focuses on three synthesized voices, direct control, and lightweight visualization. Sample playback, presets, and effects are planned for later releases.

### 1.3 Definitions

* Voice - a sound generator that implements noteOn, noteOff, and render
* Active voice - the voice currently selected for playback
* Frame - one audio sample per channel
* Buffer - a block of frames sent to the audio device

## 2 Overall Description

### 2.1 Product Perspective

Synther is a standalone openFrameworks application. It owns its audio stream, voices, and visualizer. It does not depend on external audio servers beyond the system audio device and does not require network access.

### 2.2 User Characteristics

The user is a desktop user with a keyboard and mouse, headphones or speakers, and basic familiarity with playing notes on a computer keyboard.

### 2.3 Operating Environment

* Desktop operating system: Linux, macOS, or Windows
* CPU capable of real time audio at 44.1 kHz
* Audio output device

## 3 System Requirements

### 3.1 Hardware

* CPU with at least one core free for audio rendering
* 100 MB free memory
* Keyboard and mouse
* Audio output

### 3.2 Software

* openFrameworks 0.12 or later
* C++17 compiler (GCC, Clang, or MSVC)
* make build tool
* System audio driver

### 3.3 Dependencies

* openFrameworks core
* ofSoundStream and ofSoundBuffer for audio output
* Standard C++ library

No third party plugins are required to build or run the base version.

## 4 Functional Requirements

### 4.1 Audio Engine

* FR1 - The application shall initialize an audio stream at 44100 Hz with 2 output channels and a buffer size of 256 frames.
* FR2 - The application shall provide an audioOut callback that fills the output buffer for each audio block.
* FR3 - The audio thread shall clear the buffer to silence before rendering the active voice.
* FR4 - The application shall allow the sample rate to be configured for all voices at startup.

### 4.2 Sound Generation

* FR5 - The system shall provide at least three distinct voices.
  * FR5.1 - Sine voice: smooth tone using a phase accumulator and sin function.
  * FR5.2 - Square voice: bright tone using the sign of the phase, high for half the cycle, low for the other half.
  * FR5.3 - Noise voice: white noise with uniform random values in -1 to 1.
* FR6 - Each voice shall expose noteOn with frequency and velocity, noteOff to silence, and render to fill an interleaved buffer.
* FR7 - Voices shall share a common abstract interface so the application can switch the active voice through a single pointer.
* FR8 - Only one voice shall be active at a time. Selecting a new voice with keys 1, 2, 3 shall switch the active voice. A held note shall retrigger on the new voice.
* FR9 - noteOn shall reset phase to reduce clicks. Velocity shall be in 0 to 1.
* FR10 - render shall write one mono sample per frame and replicate it to all channels.

### 4.3 Interaction

* FR11 - Keyboard shall trigger notes and select voices.
  * FR11.1 - Keys A S D F G H J K L shall trigger C4, D4, E4, F4, G4, A4, B4, C5, D5 from 261.63 Hz to 587.33 Hz.
  * FR11.2 - Releasing a mapped key shall call noteOff and mark no note held.
  * FR11.3 - Keys 1, 2, 3 shall select Sine, Square, Noise.
* FR12 - Mouse shall provide fine control.
  * FR12.1 - Horizontal position shall map to pitch offset -20 Hz to +20 Hz.
  * FR12.2 - Vertical position shall map to volume factor 1.0 to 0.0.
  * FR12.3 - Mouse move and mouse drag shall use the same mapping.
  * FR12.4 - Offset and volume shall be applied at noteOn. Frequency shall be clamped above 20 Hz and velocity clamped to 0 to 1.
* FR13 - The application shall display a HUD with current voice, key hints, mouse offset and volume, playing frequency and velocity, and a tip for controls.

### 4.4 Visualization

* FR14 - The system shall visualize the last rendered audio buffer in real time.
  * FR14.1 - Waveform: centered trace, colored by voice, scaled by velocity.
  * FR14.2 - Amplitude bar: right edge, RMS fill and peak tick.
  * FR14.3 - Spectrum: bottom bar view with 64 bars from a lightweight DFT over 256 samples.
* FR15 - Visualization data shall be shared between audio and main threads with a mutex to avoid tearing.
* FR16 - Visualizer shall expose setup for buffer size and channel count, update with a new buffer, and draw the views.

## 5 Non-Functional Requirements

* NFR1 - Performance: audio rendering shall complete within the audio callback budget. Visualization update and draw shall sustain 60 fps on typical hardware.
* NFR2 - Reliability: the application shall not crash on rapid key presses, voice switches, or mouse movement. Invalid input shall be ignored.
* NFR3 - Usability: controls shall be discoverable from the HUD without reading documentation. The window shall provide immediate visual feedback for sound changes.
* NFR4 - Maintainability: code shall be modular with separate classes for SoundSource, each voice, Visualizer, and ofApp. The inheritance and composition structure shall be clear.
* NFR5 - Portability: the application shall build and run on Linux, macOS, and Windows with openFrameworks and a C++17 toolchain.
* NFR6 - Resource usage: idle CPU shall be low when no note is held. Memory usage shall stay constant after setup.

## 6 Interface Requirements

### 6.1 User Interface

* Window size is determined by the system and is resizable. Visual layout adapts to window width and height.
* Text is rendered with bitmap strings for low overhead.
* No configuration files are required for the base version.

### 6.2 Software Interfaces

* ofSoundStream provides the audio callback with ofSoundBuffer.
* Standard library mutex and vector are used for thread safe buffer sharing.

## 7 Constraints and Assumptions

* The application assumes a working audio device. If no device is available, setup will fail and the application will not produce sound.
* The application assumes the window has focus for keyboard input.
* Sample rate is fixed at startup and not changed during playback.
* Only one note is held at a time in the base version. Polyphony is a future extension.

## 8 Future Enhancements

* ADSR envelope for amplitude shaping.
* Sample based voice for triggered hits.
* Preset system for voice and control settings.
* Simple filter or voice mixing.
* Frequency based color animation and additional visual modes.

## 9 Verification

* Build with make -j4 completes without errors.
* Run with make RunRelease and verify sound on key hold, voice switch with 1 to 3, pitch shift with mouse X, volume with mouse Y, and visualization response.
* Stress test with rapid key and mouse input and check for dropouts or crashes.

## 10 Version History

* 1.0 - August 2026 - Initial software requirements for Synther release
