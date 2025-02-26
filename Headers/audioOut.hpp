#ifndef AUDIOOUT_HPP
#define AUDIOOUT_HPP

#include "osc.hpp"
#include <portaudio.h>
#include <iostream>

// REF: https://github.com/orazdow/PortAudio-Wrapper/tree/master

typedef int (*PaCallbackFunction) (const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);

/// @brief A port audio wrapper implementing initializing and starting a portaudio stream. Allows the use of callback functions for audio callback. Only supports audio output (no input)
class AudioOut{
    private:
        PaCallbackFunction userCallback = nullptr;
        PaError error;
        // flags for destructor to close portaudio properly
        bool isOpened = false, isStarted = false;
        PaStreamParameters paParams;
        PaStream * paHandle;
        void* userData;
    
    public:
        /// @brief Class constructor, initializes portaudio and sets basic portaudio parameters
        /// @param channels Number of channels of portaudio output
        AudioOut(int channels);

        /// @brief Class destructor, closing portaudio stream
        ~AudioOut();

        /// @brief Class function to set the callback and userdata passed to callback function calls
        /// @param cb Function to be called by portaudio for audio generation (int funcName (const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*))
        /// @param userD Any data to be used in callback function. Should be recast to its original type and processed in callback function
        void setCallback(PaCallbackFunction cb, void* userD);

        /// @brief Prints out all available output audio devices, prompts user to select one, and saves their choice as the output device index
        void printAndSelectDevices();
        
        /// @brief Gets device index of the default output audio device
        void selectDefaultDevice();

        /// @brief Opens and starts portaudio stream
        void openAndStartStream();

        
        // Writing directly to stream depricated!

        // /// @brief Writes directly to audio stream.
        // /// @param buffer Pointer to array of audio samples to write to audio stream
        // /// @param bufferSize Number of samples to write to stream.
        // void writeToStream(double *buffer, int bufferSize){
        //     error = Pa_WriteStream(paHandle, buffer, bufferSize);
        //     if (error != paNoError)
        //         throw std::runtime_error("Error writing to stream!");
        // }
    };



#endif