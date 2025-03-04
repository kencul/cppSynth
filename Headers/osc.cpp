#include "osc.hpp"

double Osc::process(double freqInput, double ampInput){
    // update member variables IF default values are overriden
    if (ampInput > 0) 
        amp = ampInput;
    if (freqInput > 0) 
        freq = freqInput;
    return generateSample();
}

double Osc::generateSample(){
    // process audio
    double sample = amp * sin(phase);
    phase += twopi * freq/samplerate;

    // wrap phase under 2π
    if (phase >= twopi)
        phase -= twopi;
    return sample;
}

double *Osc::processBuffer(std::vector<double> freqs, std::vector<double> amps)
{
    // Loop through entire buffer
    for(int frame = 0, sample = 0; frame < m_framesPerBuffer; frame++){
        // Calculate sample
        double sampleValue = amps[sample] * sin(phase);
        phase += twopi * freqs[sample] / samplerate;

        // Assign sample value to all samples of frame
        for(size_t channel = 0; channel < m_channels; channel ++){
            m_outputBuffer[sample++] = sampleValue;
        }

        // wrap phase under 2π
        if (phase >= twopi)
        phase -= twopi;
    }
    return m_outputBuffer.data();
}

void Osc::setFramesPerBuffer(size_t frames)
{
    if (frames != m_framesPerBuffer){
        m_framesPerBuffer = bufferSize;
        m_outputBuffer.resize(m_framesPerBuffer * m_channels);
    }
    return;
}

Osc::~Osc()
{
}
