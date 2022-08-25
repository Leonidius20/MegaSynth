module;

#include <cmath>
using std::fmod;

module megasynth.oscillator;

// PolyBLEP by Tale
// (slightly modified)
// http://www.kvraudio.com/forum/viewtopic.php?t=375517
double PolyBlepOscillator::poly_blep(double t) {
    double dt = this->phaseIncrement / twoPi;
    // 0 <= t < 1
    if (t < dt) {                       // at the start of a period
        t /= dt;
        return t+t - t*t - 1.0;
    }
    // -1 < t < 0
    else if (t > 1.0 - dt) {            // at the end of period
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    // 0 otherwise
    else return 0.0;
}

double PolyBlepOscillator::nextSample() {
    double value = 0.0;
    double t = this->phase / twoPi; // phase but between 0 and 1

    if (this->waveform == Waveform::SINE) {
        value = naiveWaveform(Waveform::SINE);
    } else if (this->waveform == Waveform::SAW) {
        value = naiveWaveform(Waveform::SAW);
        value -= poly_blep(t);
    } else { // square or triangle
        value = naiveWaveform(Waveform::SQUARE);
        // two polybleps bc square has jumps not only at start
        // and end, but also in the middle
        value += poly_blep(t);
        value -= poly_blep(fmod(t + 0.5, 1.0)); 

        if (this->waveform == Oscillator::TRIANGLE) {
            // adding up samples of square wave to get linear rise and fall as in triangle
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            value = this->phaseIncrement * value + (1 - this->phaseIncrement) * lastOutput;
            lastOutput = value;
        }
    }

    this->phase += this->phaseIncrement;
    while (this->phase >= twoPi)
    {
        this->phase -= twoPi;
    }

    return value;
}