#include <cstdio>
#include <cmath>

#include <sndfile.h>

#define SR 44100

// g++ osc.cpp -o out -L/opt/homebrew/lib -I/opt/homebrew/include -lsndfile

const double twopi = 8. * atan(1.);

struct Osc{
    double phase;
    int samplerate;
    // Constructor with init values for phase and SR
    Osc() : phase(0.), samplerate(44100) {};
    double process(double amp, double freq){
        double sample = amp * sin(phase);
        phase += twopi * freq/samplerate;
        return sample;
    }
};

// double osc(double amp, double freq, double &phase, double sr){
//     double sample = amp * sin(phase);
//     phase += twopi * freq/sr;
//     return sample;
// }

int main (int argc, char *argv[]){
    // portaudio init
    const int bufferSize = 512;
    double *outputBuffer = new double[512];
    SNDFILE *fileOut;
    SF_INFO info_out;

    info_out.format = SF_FORMAT_WAV | SF_FORMAT_DOUBLE;
    info_out.samplerate = SR;
    info_out.channels = 1;

    const char * outputFileName = "output.wav";

    // Check if output file already exists
    FILE *testOutputFile = fopen(outputFileName, "r");
    // If output file already exists, delete the file. 
    if (testOutputFile){
        remove(outputFileName);
    }
    fclose(testOutputFile);
    
    // open sndfile
    if((fileOut = sf_open(outputFileName, SFM_WRITE, &info_out)) == NULL){
        printf("Failed to open sndfile!\n");
        return -1;
    }


    //double phase1 = 0;
    //double &phase1ref = phase1;
    int frames = 0, length = 3;
    Osc osc1, osc2;
    while(frames < SR * length){
        // Create buffer
        for(int i = 0; i < bufferSize; i++){
            outputBuffer[i] = osc1.process(0.2, 440) + osc2.process(0.2, 440 * 3/2);
            frames++;
        }
        // Ensures extra frames past length isn't written
        if(frames > SR * length){
            sf_write_double(fileOut, outputBuffer, frames - (SR*length));
        } else {
            sf_write_double(fileOut, outputBuffer, bufferSize);
        }
        
    }
    while(frames < SR * length){
        // Create buffer
        for(int i = 0; i < bufferSize; i++){
            outputBuffer[i] = osc1.process(0.2, 440/2) + osc2.process(0.2, 220 * 3/2);
            frames++;
        }
        // Ensures extra frames past length isn't written
        if(frames > SR * length){
            sf_write_double(fileOut, outputBuffer, frames - (SR*length));
        } else {
            sf_write_double(fileOut, outputBuffer, bufferSize);
        }
        
    }

    sf_close(fileOut);
}