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

