#include <iostream>
#include <cstdio>
#include <cmath>
#include <string> // for std::stoi
#include <vector> // for std::vector, allowing use of range based for loops

#include <chrono> // for std::chrono::high_resolution_clock::now() to calculate run time

#include <sndfile.h>

#define SR 44100
#define VOLUME 0.8
#define LENGTH 20
#define OUTPUTFILENAME "output2.wav"
#define BUFFERSIZE 512

// g++ osc.cpp -o out -L/opt/homebrew/lib -I/opt/homebrew/include -lsndfile

const double twopi = 8. * atan(1.);

struct Osc{
    double phase;
    int samplerate;
    int freq;
    // Constructor with init values for phase and SR
    // freq is an optional parameter for the contructor, due to it having a default value
    Osc(int freq = 440) : phase(0.), samplerate(44100), freq(freq) {};
    double process(double amp){
        double sample = amp * sin(phase);
        phase += twopi * freq/samplerate;
        // clamp phase between 0 and 2π
        if (phase >= twopi) {
            phase -= twopi;
        } else if (phase < 0){
            phase += twopi;
        }
        return sample;
    }
};


int main (int argc, char *argv[]){
    // Saves start time of program to print runtime at end of program
    auto startTime = std::chrono::high_resolution_clock::now();

    // Checks number of arguments passed
    if (argc < 2){
        std::cout << "Not enough arguments! \nUsage: ./osc2 freq1 [freq2 ...]\n";
        return -1;
    }

    // Parse arguments
    int numVoices = argc-1;
    // create vector without initial size
    std::vector<Osc> oscillators;
    // allocate memory to hold all voices
    oscillators.reserve(argc-1);

    for(int i = 0; i < numVoices; i++){
        int freq;
        try{
            freq = std::stoi(argv[i+1]);
        } catch (const std::exception& e){
            std::cerr << "Invalid argument: " << argv[i] << "! Input must be an integer..." << std::endl;
            return -1;
        }

        // contructs object Osc at the end of the vector with argument freq
        // no copying is done with emplace back, and a parameter can be passed to the contructor, overriding the default constructor of osc
        oscillators.emplace_back(freq);
        std::cout << "Voice " << i + 1 << ": " << freq << "Hz" << std::endl;
    }

    // portaudio init
    const int bufferSize = BUFFERSIZE;
    double *outputBuffer = new double[512];
    SNDFILE *fileOut;
    SF_INFO info_out;

    info_out.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    info_out.samplerate = SR;
    info_out.channels = 1;

    const char * outputFileName = OUTPUTFILENAME;

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

    int frames = 0, length = LENGTH;
    double amp = VOLUME/numVoices;
    while(frames < SR * length){
        int samplesToWrite = std::min(bufferSize, SR * length - frames);
        // Calculate buffer
        for(int i = 0; i < bufferSize; i++){
            outputBuffer[i] = 0.0; // Clear the buffer
            // a reference is used as if a reference isnt used, a copy is made
            // this would be extremely inefficient for larger types
            for (Osc& osc : oscillators){
                outputBuffer[i] += osc.process(amp);
            }
            frames++;
        }
        // Ensures extra frames past length isn't written by calculating how many samples to write before hand
        sf_write_double(fileOut, outputBuffer, samplesToWrite);
    }

    delete[] outputBuffer;
    sf_close(fileOut);

    // get end time of program and find difference. Print runtime
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Program ran succesfully in " << duration.count() << "ms!" << std::endl;

    return 0;
}