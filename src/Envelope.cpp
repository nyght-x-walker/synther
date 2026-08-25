#include "Envelope.h"
#include <algorithm>

void Envelope::setSampleRate(float rate) {
    sampleRate = rate;
    updateIncrements();
}

void Envelope::setADSR(float a, float d, float s, float r) {
    attack = a;
    decay = d;
    sustain = s;
    release = r;
    updateIncrements();
}

void Envelope::updateIncrements() {
    attackInc = (attack > 0.0f) ? (1.0f / (attack * sampleRate)) : 1.0f;
    decayInc = (decay > 0.0f) ? ((1.0f - sustain) / (decay * sampleRate)) : 1.0f;
    releaseInc = (release > 0.0f) ? (sustain / (release * sampleRate)) : 1.0f;
}

void Envelope::noteOn() {
    state = Attack;
}

void Envelope::noteOff() {
    if (state != Idle) {
        state = Release;
    }
}

void Envelope::reset() {
    state = Idle;
    level = 0.0f;
}

bool Envelope::isActive() const {
    return state != Idle;
}

float Envelope::next() {
    switch (state) {
        case Idle:
            level = 0.0f;
            break;
        case Attack:
            level += attackInc;
            if (level >= 1.0f) {
                level = 1.0f;
                state = Decay;
            }
            break;
        case Decay:
            level -= decayInc;
            if (level <= sustain) {
                level = sustain;
                state = Sustain;
            }
            break;
        case Sustain:
            level = sustain;
            break;
        case Release:
            level -= releaseInc;
            if (level <= 0.0f) {
                level = 0.0f;
                state = Idle;
            }
            break;
    }
    return level;
}
