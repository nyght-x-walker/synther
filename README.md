# Synther

![Version](https://img.shields.io/badge/version-1.0-blue)
![oF](https://img.shields.io/badge/openFrameworks-0.12%2B-lightgrey)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-green)
![License](https://img.shields.io/badge/license-MIT-green)
![Team](https://img.shields.io/badge/team-Capricorn-orange)
![Status](https://img.shields.io/badge/status-stable-brightgreen)

An interactive software synthesizer built with openFrameworks.

Synther generates audio in real time, responds to keyboard and mouse input, and visualizes the output as a waveform, amplitude bar, and spectrum.

## Features

* Three voices with a common interface: Sine, Square, and Noise
* Keyboard control for note selection and voice switching
* Mouse control for fine pitch adjustment and volume
* Real time visualization with voice color and amplitude scaling
* Clean object oriented design with inheritance and composition

## Controls

| Input | Function |
|------|----------|
| 1, 2, 3 | Select Sine, Square, Noise |
| A S D F G H J K L | Play notes C4 to D5 |
| Release key | Stop note |
| Mouse X | Pitch offset -20 Hz to +20 Hz |
| Mouse Y | Volume 1.0 to 0.0, applied at note-on |
| Mouse drag | Same as mouse move |

The on screen HUD shows the current voice, frequency, velocity, mouse offset, and volume.

## Requirements

* openFrameworks 0.12 or later
* C++17 toolchain (GCC, Clang, or MSVC)
* make and audio output device
* Tested on Linux 64-bit, expected to build on macOS and Windows

See docs/requirements.md for full functional and non-functional requirements.

## Build and Run

### Build

```bash
cd apps/myApps/Synther
make -j4
```

### Run

```bash
make RunRelease
```

Or run the binary directly:

```bash
bin/Synther
```

If you hear no sound, check system volume, press 1 to select Sine, hold A, and move the mouse to the top of the window.

## Project Structure

```
Synther/
  src/
    SoundSource.h/cpp   # abstract voice interface
    SineVoice.h/cpp     # sine wave
    SquareVoice.h/cpp   # square wave
    NoiseVoice.h/cpp    # white noise
    Visualizer.h/cpp    # waveform, amplitude, spectrum
    ofApp.h/cpp         # application, audio, input
    main.cpp
  bin/
    data/               # sample assets (currently placeholder, ready for samples)
  docs/
    manual.md           # source for PDF manual
    requirements.md     # requirement specification
    Synther_Project_Description_Manual.pdf  # full description and manual
    workflow.md         # internal workflow sketch
  README.md
  Makefile, config.make, addons.make
```

## Assets

| Asset | Location | Purpose | Status |
|-------|----------|---------|--------|
| Source code | src/ | All synthesis, interaction, and visualization code | Included |
| Sample data | bin/data/ | Placeholder for optional sample based voice | Empty, ready for wav files |
| Documentation source | docs/manual.md | Markdown source for PDF | Included |
| PDF manual | docs/Synther_Project_Description_Manual.pdf | Project description and user manual for submission | Included |
| Workflow sketch | docs/workflow.md | Early class diagram and team split | Included |
| Build files | Makefile, config.make | openFrameworks build configuration | Included |

No external samples are required to run the synthesizer. All three voices are synthesized in real time. If samples are added later, place wav files in bin/data and load via SampleVoice.

## Architecture

* **SoundSource** is an abstract base class that defines noteOn, noteOff, and render.
* **SineVoice, SquareVoice, NoiseVoice** inherit from SoundSource and implement signal generation.
* **Visualizer** stores recent audio, computes levels and spectrum, and draws the views.
* **ofApp** owns the sound stream, the three voice instances, the active voice pointer, and the visualizer. It handles setup, update, draw, audioOut, and input.

Relationships:

* Inheritance: SoundSource as parent for the three voices.
* Composition: ofApp owns voices, visualizer, and audio resources.

Data flow is setup to key input to audio thread to visual update to draw. See the PDF manual for the full class diagram and data flow.

## Documentation

* Full manual: docs/Synther_Project_Description_Manual.pdf
* Requirements: docs/requirements.md
* Source manual: docs/manual.md

## Licensing

This project is released under the MIT License for the original code written for the course.

* **Project code (src/, docs/)** - MIT License. You may use, modify, and share with attribution for educational purposes.
* **openFrameworks** - MIT License, see https://openframeworks.cc/about/
* **Third party libraries (FMOD, etc. in bin/)** - retain their original licenses.

For the course submission, the code and PDF are provided as is for evaluation. No warranty is provided.

```
MIT License

Copyright 2026 Team Capricorn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Acknowledgments

* Built with openFrameworks
* Audio via ofSoundStream and ofSoundBuffer
* Team Capricorn

## Changelog

* 1.0 (Aug 2026) - Phase 6: three voices, mouse control, visualization, manual and PDF

---

For questions about build or usage, open an issue.
