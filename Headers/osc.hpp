#ifndef OSC_HPP
#define OSC_HPP

#include <cmath>
#include "libdef.hpp"


/// @brief Sine wave oscillator which stores phase, allowing for frequency changes without clicks
class Osc{
    public:
        // Constructor with init values for phase and SR
        // freq is an optional parameter for the contructor, due to it having a default value
        Osc(double amp = 1, int freq = 440) : phase(0.), samplerate(44100), freq(freq), amp(amp) {};

        /// @brief Assigns parameters to amp and freq member variables, and generates the next sample
        /// @param ampInput amplitude value to set oscillator to (0-1), -1 for no change
        /// @param freqInput frequency value to set oscillator to (positive integer), -1 for no change
        /// @return generated sample value
        double process(int freqInput = -1, double ampInput = -1);

    private:
        /// @brief Generates the next sample in the oscillator
        /// @return generated sample value
        double generateSample();

        double phase;
        int samplerate;
        int freq;
        double amp;
};

#endif