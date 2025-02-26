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

/// @brief Osc with vector of pairs added into a composite class
class OscFreqDurPair{
    public:
    // Constructor initializes Osc member object
    OscFreqDurPair(double amp = 1, int freq = 440) : osc(amp, freq){};
    
    /// @brief Parses command line arguments, separating each frequency duration pair, and validating for valid input
    /// @param argc main function argc
    /// @param argv main function argv
    void parseArguments(int argc, char* argv[]){
        for(int i = 1; i < argc; i += 2){
            int freq;
            double dur;
            try{
                freq = std::stoi(argv[i]);
                dur = std::stod(argv[i+1]);
            } catch (const std::exception& e){
                std::cerr << "Invalid argument: " << argv[i] << " or " << argv[i+1] << "! Inputs must be a number..." << std::endl;
                freqDurPairs = {};
            }
            freqDurPairs.emplace_back(freq, dur);
        }
    }
    
    /// @brief Checks if freqDurPairs member variable is empty
    /// @return true if empty, false if not empty
    bool checkVectorEmpty(){
        return freqDurPairs.empty();
    }
    
    /// @brief Returns a read-only reference to the frequency-amplitude pairs vector
    /// @return const reference to the vector
    const std::vector<std::pair<int, double>>& getPairs() const {
        return freqDurPairs;
    }
    
    double process(){
        if (noteSamplesToWrite <= noteSamplesWritten){
            noteIndex++;
            std::pair<int,double> pair = freqDurPairs[noteIndex];
            noteFreq = pair.first;
            noteSamplesToWrite = static_cast<int>(pair.second * sampleRate);
            noteSamplesWritten = 0;
            std::cout << "Freq: " << noteFreq << "Hz" << std::endl;
        }
        noteSamplesWritten++;
        return osc.process(noteFreq);
    }

    double getPerformanceTime(){
        // If totalTime is already calculated, just return it
        if(totalTime != 0) return totalTime;

        // Iterate through all freq dur pairs, add up all durations
        for (const auto& pair : freqDurPairs){
            totalTime += pair.second;
            std::cout << "Freq: " << pair.first << "Hz Duration: " << pair.second << "sec" << std::endl;
        }

        return totalTime;
    }
    
    private:
    Osc osc;
    std::vector<std::pair<int,double>> freqDurPairs;
    
    int noteIndex = -1;
    int noteSamplesWritten = 0;
    int noteSamplesToWrite = -1;
    int noteFreq = 0;

    double totalTime = 0;
};

/// @brief Callback function called by portaudio to generate real time audio
/// @param inputBuffer Input buffer of samples from input audio device
/// @param outputBuffer Buffer of samples to be sent to output audio device
/// @param framesPerBuffer Number of frames required by buffer
/// @param timeInfo 
/// @param statusFlags Not applicable/implemented
/// @param userData Data passed to callback
/// @return int
int myCallback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData) {
        float *out = (float*)outputBuffer;
        (void)inputBuffer; // Prevent unused variable warning.
        OscFreqDurPair *osc = (OscFreqDurPair*)userData;

        
        float sample = 0;
        for (unsigned int i = 0; i < framesPerBuffer; i++) {
            sample = osc->process();
            *out++ = sample; // Left channel
            *out++ = sample; // Right channel
        }

        return paContinue;
}

int main (int argc, char *argv[]){
    // Saves start time of program to print runtime at end of program
    auto startTime = std::chrono::high_resolution_clock::now();

    // Checks number of arguments passed
    if (argc < 3 || argc % 2 != 1 /* if no freq dur pairs, or incomplete pairs*/){
        std::cout << "Invalid number of arguments! \nUsage: ./osc5 freq1 dur1 [freq2 dur2 ...]\n";
        return EXIT_FAILURE;
    }
    
    // Init osc object
    OscFreqDurPair osc(volume);

    osc.parseArguments(argc, argv);

    if (osc.checkVectorEmpty())
        return EXIT_FAILURE;
    
    osc.getPerformanceTime();
    // Print total performance time
    std::cout << "Total performance time: " << osc.getPerformanceTime() << " seconds" << std::endl;

    // Creating a nullptr of object AudioOut so the initialization can be included in try
    AudioOut* audioOut = nullptr;
    // portaudio init
    try {
        audioOut = new AudioOut(2);
        audioOut -> setCallback(myCallback, &osc);
        audioOut -> selectDefaultDevice();
        audioOut -> openAndStartStream();
        // Keep stream playing for duration of performance time
        std::this_thread::sleep_for(std::chrono::duration<double>(osc.getPerformanceTime()));
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