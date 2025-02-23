#include <iostream>
#include <cstdio>
#include <cmath>
#include <string> // for std::stoi
#include <vector> // for std::vector, allowing use of range based for loops
#include <array> // for std::array, for use as buffer

#include <chrono> // for std::chrono::high_resolution_clock::now() to calculate run time

#include <sndfile.h>


// g++ osc.cpp -o out -L/opt/homebrew/lib -I/opt/homebrew/include -lsndfile

// Global variables
#define OUTPUTFILENAME "output3.wav";

constexpr int sampleRate = 44100;
constexpr double volume = 0.2;
constexpr int bufferSize = 512;
constexpr double pi = 3.14159265358979323846;
constexpr double twopi = 2.0 * pi;

/// @brief Sine wave oscillator which stores phase, allowing for frequency changes without clicks
struct Osc{
    double phase;
    int samplerate;
    int freq;
    double amp;
    // Constructor with init values for phase and SR
    // freq is an optional parameter for the contructor, due to it having a default value
    Osc(double amp = 1, int freq = 440) : phase(0.), samplerate(44100), freq(freq), amp(amp) {};

    /// @brief Assigns parameters to amp and freq member variables, and generates the next sample
    /// @param ampInput amplitude value to set oscillator to (0-1)
    /// @param freqInput frequency value to set oscillator to (positive integer)
    /// @return generated sample value
    double process(int freqInput = -1, double ampInput = -1){
        // update member variables IF default values are overriden
        if (ampInput > 0) 
            amp = ampInput;
        if (freqInput > 0) 
            freq = freqInput;
        return generateSample();
    }

    // private helper function
    // not accessible outside of the struct
    // to make process overload not have to reassign unneccessary values to member variables
    private:
        /// @brief Generates the next sample in the oscillator
        /// @return generated sample value
        double generateSample(){
            // process audio
            double sample = amp * sin(phase);
            phase += twopi * freq/samplerate;

            // wrap phase under 2π
            if (phase >= twopi)
                phase -= twopi;
            return sample;
        }
};

/// @brief Handles opening a sound file, writing to sound file, and closing sound file
class AudioFile{
public:
    /// @brief initializes the soundfile
    /// @param fileName name of output file
    /// @param sr sample rate of output file
    /// @param channels number of channels of output file
    AudioFile(const std::string& fileName, int sr, int channels){
        info_out.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        info_out.samplerate = sampleRate;
        info_out.channels = channels;

        // Check if output file already exists
        FILE *testOutputFile = fopen(fileName.c_str(), "r");
        // If output file already exists, delete the file. 
        if (testOutputFile){
            remove(fileName.c_str());
        }

        fileOut = sf_open(fileName.c_str(), SFM_WRITE, &info_out);
        if(!fileOut){
            throw std::runtime_error("Failed to open soundfile!");
        }
    }

    /// @brief Closes sf reader automatically when class object goes out of scope
    ~AudioFile(){
        if (fileOut){
            sf_close(fileOut);
        }
    }

    /// @brief writes samples in buffer to sound file
    /// @param buffer array of doubles of sample values
    /// @param samples number of samples to write from buffer
    void write(const double* buffer, int samples){
        sf_write_double(fileOut, buffer, samples);
    }

    private:
        SNDFILE *fileOut;
        SF_INFO info_out;
};

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


    // portaudio init
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