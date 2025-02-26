#include "osc.hpp"

double Osc::process(int freqInput, double ampInput){
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