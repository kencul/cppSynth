#include "audioOut.hpp"

AudioOut::AudioOut(int channels){
    // Attempt to initialize portaudio
    error = Pa_Initialize();

    // Print success message if initialized, throw error if not
    if(error == paNoError){
        std::cout << "Portaudio initialised" << std::endl;
    }
    else
        throw std::runtime_error("Failed to initialize portaudio!");

    // Set basic parameters for portaudio
    paParams.channelCount = channels;
    paParams.sampleFormat = paFloat32;
    paParams.hostApiSpecificStreamInfo = NULL;
}


AudioOut::~AudioOut(){
    // Only run these if boolean flags are true
    if(isStarted)
        Pa_StopStream(paHandle);
    if(isOpened)
        Pa_CloseStream(paHandle);
    
    // Always run as pa initialize runs on construction
    Pa_Terminate();
}

void AudioOut::setCallback(PaCallbackFunction cb, void* userD) {
    userCallback = cb;
    userData = userD;
}

void AudioOut::printAndSelectDevices(){
    // Get number of available audio devices
    const int numDevices = Pa_GetDeviceCount();

    // If no devices are found, throw error
    if (numDevices < 0){
        throw std::runtime_error("Failed to get device count!");
    }

    // inits
    std::vector<int> outputDevices;

    // print formatting
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Output Devices:" << std::endl;

    // Iterate and print through all discovered devices for only output devices
    for(int i=0; i < numDevices; i++){
        // Fetch device
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        // IF device has available output channels
        if(deviceInfo->maxOutputChannels > 0){
            outputDevices.push_back(i);
            std::cout << outputDevices.size() - 1 << ": " << deviceInfo->name << " (Channels: " << deviceInfo->maxOutputChannels << ")" << std::endl;
        }
    }

    // formatting
    std::cout << "---------------------------------------" << std::endl;

    // If no output devices are found, throw error
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

    // Set device index to portaudio params
    paParams.device = outputDevices[selectedIndex];
    // Gets selected device's info
    const PaDeviceInfo * outputDeviceInfo = Pa_GetDeviceInfo(paParams.device);
    // Sets latency based on the selected device as port audio parameter
    paParams.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;
    
    // Print name of seleted output device
    std::cout << outputDeviceInfo -> name << " selcted! " << std::endl;
}

void AudioOut::selectDefaultDevice(){
    // Gets the default output device index and save to portaudio params
    paParams.device = Pa_GetDefaultOutputDevice();
    // gets device info
    const PaDeviceInfo * outputDeviceInfo = Pa_GetDeviceInfo(paParams.device);
    // Sets latency based on the selected device as port audio parameter
    paParams.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;
    
    // Print name of seleted output device
    std::cout << outputDeviceInfo->name << " selcted! " << std::endl;
}

void AudioOut::openAndStartStream(){
    // Check if callback is defined
    if(!userCallback){
        throw std::runtime_error("Callback not set! Use setCallback() before opening the stream.");
    }

    // Open stream with portaudio params, callback function, and userdata
    error = Pa_OpenStream(&paHandle, NULL, &paParams, sampleRate, bufferSize, paNoFlag, userCallback, userData);
    
    // Check if stream was opened succesfully
    if (error == paNoError){
        std::cout << "Portaudio stream successfully opened!" << std::endl;
        isOpened = true;
    } else {
        throw std::runtime_error("Error opening portaudio stream!");
    }

    // Start portaudio stream and check if successful
    error = Pa_StartStream(paHandle);
    if(error == paNoError){
        std::cout << "Portaudio stream successfully started!" << std::endl;
        isStarted = true;
    } else {
        throw std::runtime_error("Error starting portaudio stream!");
    }
}