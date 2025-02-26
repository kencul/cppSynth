#ifndef AUDIOFILE_HPP
#define AUDIOFILE_HPP

#include <sndfile.h>
#include <string>

/// @brief Handles opening a sound file, writing to sound file, and closing sound file
class AudioFile{
    public:
        /// @brief initializes the soundfile
        /// @param fileName name of output file
        /// @param sr sample rate of output file
        /// @param channels number of channels of output file
        AudioFile(const std::string& fileName, int sr, int channels);
        
        /// @brief writes samples in buffer to sound file
        /// @param buffer array of doubles of sample values
        /// @param samples number of samples to write from buffer
        void write(const double* buffer, int samples);

        /// @brief Closes sf reader automatically when class object goes out of scope
        ~AudioFile();

    private:
        SNDFILE *fileOut;
        SF_INFO info_out;
};

#endif