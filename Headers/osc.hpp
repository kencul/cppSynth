#ifndef OSC_HPP
#define OSC_HPP

#include <cmath>
#include <vector>
#include "libdef.hpp"
#include <iostream>


/// @brief Sine wave oscillator which stores phase, allowing for frequency changes without clicks
class Osc{
public:
    /// @brief Constructor with init values for phase and SR
    // freq is an optional parameter for the contructor, due to it having a default value
    Osc(size_t channels) : phase(0.), samplerate(44100), m_channels(channels), m_framesPerBuffer(bufferSize), m_outputBuffer(m_framesPerBuffer * m_channels) {};

    /// @brief Assigns parameters to amp and freq member variables, and generates the next sample
    /// @param ampInput amplitude value to set oscillator to (0-1), -1 for no change
    /// @param freqInput frequency value to set oscillator to (positive integer), -1 for no change
    /// @return generated sample value
    double process(double freqInput = -1, double ampInput = -1);

    /// @brief Calculates a full buffer of samples based on a vector of freqs and amps corresponding to each sample in buffer
    /// @param freqs Vector of frequencies of each sample
    /// @param amps Vector of amplitudes of each sample
    /// @return Pointer to array of sampels of buffer
    double * processBuffer(std::vector<double> freqs, std::vector<double> amps);

    /// @brief Sets the buffer size of the oscillator, checking if the entered value is different form the stored value. Resizes the audio buffer in the class if changed.
    /// @param bufferSize size of buffer
    void setFramesPerBuffer(size_t bufferSize);

    /// @brief Destructor
    ~Osc();

protected:
    /// @brief Generates the next sample in the oscillator
    /// @return generated sample value
    virtual double generateSample();
    
private:
    double phase;
    int samplerate;
    double freq;
    double amp;
    size_t m_channels;
    size_t m_framesPerBuffer;
    std::vector<double> m_outputBuffer;
};

#endif