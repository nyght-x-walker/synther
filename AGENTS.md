# Agentic Project Plan — Synther

## Scope
This document describes my individual contribution to a team of 2 project in openFrameworks.
The project is an interactive software synthesizer with visualization and optional sample-based sound.

## Goal
Build an interactive software synthesizer in openFrameworks that:
- generates at least three distinct sounds,
- reacts to keyboard and mouse interaction,
- includes sound visualization,
- uses a clean OOP structure,
- and is documented with progress logs, a project PDF, and source code.

## My Contribution
My part of the project focuses on:
- the audio engine,
- interaction logic,
- visualization,
- OOP structure,
- and project documentation from my side.

## Final Deliverables
- OpenFrameworks source code (.h, .cpp, and required assets).
- A PDF project description and user manual.
- A learning log with my Yellowdig posts/comments, project stats, and GitHub link.
- A zip archive containing everything required for submission.

## Core Features
1. Three playable sounds.
   - Example: sine lead, square bass, noise percussion/sample hit.
2. Interaction.
   - Keyboard triggers notes.
   - Mouse controls pitch, volume, filter, or timbre.
3. Visualization.
   - Waveform, amplitude bars, or FFT-style spectrum display.
4. Clean OOP structure.
   - Use composition and inheritance.
5. Documentation.
   - Record my work every second day or in each work session if bundling work.

## Technical Approach
### Audio
- Use `ofSoundStream` for real-time synthesis.
- Use `audioOut()` to generate sound.
- Use `ofSoundPlayer` if adding sample-based snippets.

### Visuals
- Draw waveform traces or animated bars based on current amplitude.
- Optional: color changes based on note type, pitch, or loudness.

### Interaction
- Map keys to notes and sound modes.
- Map mouse position to pitch, volume, waveform mix, or effect amount.
- Optional: add a mode switch for keyboard / mouse / sample triggers.

## Class Structure
### Base classes
- `SoundSource`
  - common interface for generating or triggering sound
- `Envelope`
  - handles attack, decay, sustain, release
- `Visualizer`
  - renders waveform or spectrum

### Derived classes
- `SineVoice`
- `SquareVoice`
- `NoiseVoice`
- Optional: `SampleVoice`

### Relationships
- Inheritance: `SineVoice`, `SquareVoice`, `NoiseVoice` inherit from `SoundSource`.
- Composition: `SynthApp` contains voice objects, envelope, and visualizer objects.

## Work Phases

### Phase 1 — Planning
- Define sound types and controls.
- Sketch UI layout.
- Decide class relationships.
- Create a folder structure.

### Phase 2 — Audio Prototype
- Make one sound generator work.
- Verify audio output.
- Prevent clicks/pops with amplitude ramping.

### Phase 3 — Add More Sounds
- Add two more sound types.
- Test note triggering and parameter changes.
- Keep code modular.

### Phase 4 — Visualization
- Add waveform or bar visualization.
- Connect visuals to audio amplitude or spectrum.
- Improve readability and aesthetics.

### Phase 5 — Interaction Polish
- Add mouse-based control.
- Add key hints on screen.
- Tune volume and responsiveness.

### Phase 6 — Documentation
- Write the manual and project description.
- Explain the class design and why the relations were chosen.
- Add screenshots and usage instructions.

### Phase 7 — Final Packaging
- Clean source tree.
- Check required assets are included.
- Build the zip archive.
- Verify the PDF and learning log are complete.

## Progress Documentation
- Post or comment at least every second day during the project phase.
- Target at least 7 contributions from my side if the team schedule allows continuous work.
- Do not duplicate teammate posts.
- Add my own perspective, screenshots, or debugging notes.
- Include screenshots of intermediate states, error messages, or short clips when useful.

## Definition of Done
The project is complete when:
- the synthesizer produces at least three distinct sounds,
- the app is interactive,
- the visualizer works,
- the code uses a clean OOP design with at least two class relationships,
- the PDF explains usage and design choices,
- the learning log is complete,
- and the final zip contains all required files.

## Suggested Extra Credit
- Sample-trigger mode.
- Simple ADSR envelope.
- Sound layer switching.
- Responsive color animation based on frequency or amplitude.
- A small preset system for different sonic moods.
