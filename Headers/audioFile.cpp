#include "audioFile.hpp"

AudioFile::AudioFile(const std::string& fileName, int sr, int channels){
    info_out.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    info_out.samplerate = sr;
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

AudioFile::~AudioFile(){
    if (fileOut)
        sf_close(fileOut);
}

void AudioFile::write(const double* buffer, int samples){
    sf_write_double(fileOut, buffer, samples);
}