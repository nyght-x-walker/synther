#pragma once

class Envelope {
public:
    void setSampleRate(float rate);
    void setADSR(float attack, float decay, float sustain, float release);
    void noteOn();
    void noteOff();
    float next();
    bool isActive() const;
    void reset();

private:
    enum State { Idle, Attack, Decay, Sustain, Release };
    State state = Idle;
    float sampleRate = 44100.0f;
    float attack = 0.015f;
    float decay = 0.08f;
    float sustain = 0.7f;
    float release = 0.12f;
    float level = 0.0f;
    float attackInc = 0.0f;
    float decayInc = 0.0f;
    float releaseInc = 0.0f;
    void updateIncrements();
};
