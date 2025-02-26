#ifndef AUDIOOUT_HPP
#define AUDIOOUT_HPP

#include "osc.hpp"
#include <portaudio.h>
#include <iostream>

// REF: https://github.com/orazdow/PortAudio-Wrapper/tree/master

typedef int (*PaCallbackFunction) (const void*, void*, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void*);

class AudioOut{
    private:
        PaCallbackFunction userCallback = nullptr;
        PaError error;
        // flags for destructor to close portaudio properly
        bool isInitialized = false, isOpened = false, isStarted = false;
        PaStreamParameters paParams;
        PaStream * paHandle;
        void* userData;
    
    public:
        AudioOut(int channels){
            error = Pa_Initialize();
            if(error == paNoError){
                std::cout << "Portaudio initialised" << std::endl;
                isInitialized = true;
            }
            else
                throw std::runtime_error("Failed to initialize portaudio!");

            paParams.channelCount = channels;
            paParams.sampleFormat = paFloat32;
            paParams.hostApiSpecificStreamInfo = NULL;
        }

        ~AudioOut(){
            if(isStarted)
                Pa_StopStream(paHandle);
            if(isOpened)
                Pa_CloseStream(paHandle);
            if(isInitialized)
                Pa_Terminate();
        }

        void setCallback(PaCallbackFunction cb, void* userD) {
            userCallback = cb;
            userData = userD;
        }

        /// @brief Prints out all available output audio devices, prompts user to select one, and saves their choice as the output device index
        void printAndSelectDevices(){
            const int numDevices = Pa_GetDeviceCount();

            if (numDevices < 0){
                throw std::runtime_error("Failed to get device count!");
            }

            std::vector<int> outputDevices;
            std::cout << "---------------------------------------" << std::endl;
            std::cout << "Output Devices:" << std::endl;

            // Iterate and print through all discovered devices
            for(int i=0; i < numDevices; i++){
                // Fetch device
                const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
                // IF device has available output channels
                if(deviceInfo->maxOutputChannels > 0){
                    outputDevices.push_back(i);
                    std::cout << outputDevices.size() - 1 << ": " << deviceInfo->name << " (Channels: " << deviceInfo->maxOutputChannels << ")" << std::endl;
                }
            }
            std::cout << "---------------------------------------" << std::endl;

            
            if (outputDevices.empty()){
                throw std::runtime_error("No valid output devices found!");
            }

            // Prompt user to select audio device
            int selectedIndex = -1;
            std::cout << "Enter the number of output device to use: ";

            // Check if user input is a number
            if (!(std::cin >> selectedIndex)) {
                std::cin.clear(); // Clear the error flag, allowing cin to be used again after error
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input from cin buffer
                throw std::runtime_error("Invalid input! Please enter a number.");
            }

            // Ensure user input is valid
            if (selectedIndex < 0 || selectedIndex >= outputDevices.size()){
                throw std::runtime_error("User selected index out of range!");
            }

            paParams.device = outputDevices[selectedIndex];
            const PaDeviceInfo * outputDeviceInfo = Pa_GetDeviceInfo(paParams.device);
            paParams.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;
            
            std::cout << outputDeviceInfo -> name << " selcted! " << std::endl;
        }
        
        /// @brief Gets device index of the default output audio device
        void selectDefaultDevice(){
            paParams.device = Pa_GetDefaultOutputDevice();
            const PaDeviceInfo * outputDeviceInfo = Pa_GetDeviceInfo(paParams.device);
            paParams.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;
            
            std::cout << outputDeviceInfo->name << " selcted! " << std::endl;
        }

        void openAndStartStream(){
            // Check if callback is defined
            if(!userCallback){
                throw std::runtime_error("Callback not set! Use setCallback() before opening the stream.");
            }

            error = Pa_OpenStream(&paHandle, NULL, &paParams, sampleRate, bufferSize, paNoFlag, userCallback, userData);
            if (error == paNoError){
                std::cout << "Portaudio stream successfully opened!" << std::endl;
                isOpened = true;
            } else {
                throw std::runtime_error("Error opening portaudio stream!");
            }

            error = Pa_StartStream(paHandle);
            if(error == paNoError){
                std::cout << "Portaudio stream successfully started!" << std::endl;
                isStarted = true;
            } else {
                throw std::runtime_error("Error starting portaudio stream!");
            }

        }

        void writeToStream(double *buffer, int bufferSize){
            error = Pa_WriteStream(paHandle, buffer, bufferSize);
            if (error != paNoError)
                throw std::runtime_error("Error writing to stream!");
        }
    };



#endif