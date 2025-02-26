#include <iostream>
#include <string> // for std::stoi
#include <vector> // for std::vector, allowing use of range based for loops
#include <array> // for std::array, for use as buffer

#include <chrono> // for std::chrono::high_resolution_clock::now() to calculate run time
#include <thread> // for std::this_thread

// Custom header files
#include "libdef.hpp"
#include "osc.hpp"
#include "audioOut.hpp"

int myCallback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData) {
        float *out = (float*)outputBuffer;
        (void)inputBuffer; // Prevent unused variable warning.
        Osc *osc = (Osc*)userData;

        float sample = 0;
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            sample = osc -> process();
            *out++ = sample; // Left channel
            *out++ = sample; // Right channel
        }

        return paContinue;
}


int main (int argc, char *argv[]){
    // Saves start time of program to print runtime at end of program
    auto startTime = std::chrono::high_resolution_clock::now();

    // Checks number of arguments passed
    if (argc < 2){
        std::cout << "Invalid number of arguments! \nUsage: ./osc5 freq\n";
        return EXIT_FAILURE;
    }

    // Parse argument as frequency input
    int freq = std::atoi(argv[1]);
    
    std::cout <<"freq: " << freq << std::endl;
    // osc init
    Osc osc(volume, freq);

    // Creating a nullptr of object AudioOut so the initialization can be included in try
    AudioOut* audioOut = nullptr;
    // portaudio init
    try {
        audioOut = new AudioOut(2);
        audioOut -> setCallback(myCallback, &osc);
        audioOut -> selectDefaultDevice();
        audioOut -> openAndStartStream();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    } catch(const std::runtime_error e){
        std::cerr << e.what() <<std::endl;
        return EXIT_FAILURE;
    }


    // get end time of program and find difference. Print runtime
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Program ran succesfully in " << duration.count() << "ms!" << std::endl;

    return 0;
}