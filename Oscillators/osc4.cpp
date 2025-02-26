#include <iostream>
#include <string> // for std::stoi
#include <vector> // for std::vector, allowing use of range based for loops
#include <array> // for std::array, for use as buffer

#include <chrono> // for std::chrono::high_resolution_clock::now() to calculate run time

// Custom header files
#include "audioFile.hpp"
#include "libdef.hpp"
#include "osc.hpp"


/// @brief Parses command line arguments, separating each frequency duration pair, and validating for valid input
/// @param argc main function argc
/// @param argv main function argv
/// @return vector of pairs of frequency and duration. empty vector if invalid input
std::vector<std::pair<int, double>> parseArguments(int argc, char *argv[]){
    std::vector<std::pair<int, double>> freqDurationPairs;

    for(int i = 1; i < argc; i += 2){
        int freq;
        double dur;
        try{
            freq = std::stoi(argv[i]);
            dur = std::stod(argv[i+1]);
        } catch (const std::exception& e){
            std::cerr << "Invalid argument: " << argv[i] << " or " << argv[i+1] << "! Inputs must be a number..." << std::endl;
            return {};
        }
        freqDurationPairs.emplace_back(freq, dur);
    }

    return freqDurationPairs;
}

/// @brief Generates all samples and writes to sound file
/// @param audioFile pointer to AudioFile object
/// @param freqDurationPairs vector of frequency and duration pairs
void writeAudio(AudioFile* audioFile, const std::vector<std::pair<int, double>>& freqDurationPairs){
    Osc osc(volume);
    std::array<double, bufferSize> outputBuffer{};
    for (const auto& note : freqDurationPairs){
        // init variables for freq
        int frames = 0;
        double length = note.second;
        int framesToWrite = static_cast<int>(sampleRate * length);

        // write all data for specific freq
        while(frames < framesToWrite){
            // Determine if entire buffer is to be filled, or to only partially fill
            int blockSize = std::min(bufferSize, framesToWrite - frames);

            // Calculate buffer
            for(int i = 0; i < blockSize; i++){
                outputBuffer[i] = osc.process(note.first);
                frames++;
            }
            // Ensures extra frames past length isn't written by calculating how many samples to write before hand
            audioFile -> write(outputBuffer.data(), blockSize);
        }
    }
}

int main (int argc, char *argv[]){
    // Saves start time of program to print runtime at end of program
    auto startTime = std::chrono::high_resolution_clock::now();

    // Checks number of arguments passed
    if (argc < 3 || argc % 2 != 1 /* if no freq dur pairs, or incomplete pairs*/){
        std::cout << "Invalid number of arguments! \nUsage: ./osc2 freq1 dur1 [freq2 dur2 ...]\n";
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::vector<std::pair<int, double>> freqDurationPairs = parseArguments(argc, argv);

    if (freqDurationPairs.empty())
        return EXIT_FAILURE;
    
    // Print parsed data
    double totalTime = 0;
    // Reference so freqDurationPairs isn't copied
    for (const auto& pair : freqDurationPairs){
        totalTime += pair.second;
        std::cout << "Freq: " << pair.first << "Hz Duration: " << pair.second << "sec" << std::endl;
    }

    std::cout << "Total performance time: " << totalTime << " seconds" << std::endl;

    // sndfile init
    std::string outputFileName = OUTPUTFILENAME;

    // Allocates memory to a null AudioFile to keep it in scope after try
    AudioFile* audioFile = nullptr;
    try {
        audioFile = new AudioFile(outputFileName, sampleRate, 1);
    } catch (const std::runtime_error e){
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    // Writing audio data
    writeAudio(audioFile, freqDurationPairs);

    // get end time of program and find difference. Print runtime
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << "Program ran succesfully in " << duration.count() << "ms!" << std::endl;

    return 0;
}