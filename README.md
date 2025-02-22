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

After these commands are run, to rebuild, navigate to builds directory and use command.
```
make
```

This will create an executable ```out```

### Usage
Run executable:
```
./out
```
Play output wav file "output.wav" in same directory as executable