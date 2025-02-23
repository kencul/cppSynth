# C++ Oscillator
## Ken Kobayashi - 2/18/2025

### Building
Uses a CMake file pointing to libsndfile installed in homebrew directory.  
This CMake file **only** works when libsndfile is installed threw brew in MacOS on M1 Macs.

```
mkdir builds
cd builds
cmake ..
make
```

This creates directory ```/bin``` inside of the builds directory.  
All compiled executables are store in this directory

After these commands are run, to rebuild after any changes, navigate to builds directory and use command.
```
make
```


### Usage
Run executable inside of the ```/builds/bin``` directory:
```
./bin/osc
```
Play output wav file "output.wav" in same directory where command was run.

### osc1

Simple sine wave oscillator which outputs a 3 second output.wav file.  
Uses a ```struct``` to store object variables such as phase position to consolidate information about each oscillator voice within itself.  
Frequencies and voices are hardcoded. Must be changed within code.

### osc2

Improvement on osc1 through changing oscillator voice frequencies through program arguements.

Uses ```std::vect``` instead of dynamically allocated arrays to allow in place constructing of Osc objects through ```emplace_back```, and range-based for loops that act like for each loops from python, simplifying the for loops.

Added the ```chrono``` library to track how long the program takes to execute, which is printed on successful runs.

Defines to easily change the output volume, length, file name, and buffer size

Added better logic to write exact amounts of samples to fill 3 seconds

Keeps phase member in Osc object clamped within 0 and 2π

### osc3

A variation of osc 2 which only handles one voice, but allows the voice to change frequency

Usage:

```./bin/osc3 freq1 dur1 [freq2 dur2 ...]```

Example usage:

```./bin/osc3 440 0.5 550 0.5 660 0.5 770 0.5 880 0.5 990 0.5 1100 0.5```

Use of ```std::pair``` for parameter parsing, acting like a tuple in python, storing frequency and duration as a pair.

Sample generation only fills buffer with nessassary samples, so the phases don't get messed up between partial fills of the buffer

Addition of ```sampleGeneration``` helper function in ```Osc``` struct to avoid unnessassary member variable assignments for ```process``` function overloads.

Changed macros to ```constexpr``` for better optimization by compiler (Not applicable to std::string)

Encapsulated code for parsing parameters and generating and writing audio into functions.

Encapsulated code for initializing sound file and writing to sound file into a class.