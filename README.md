# MI Tensor Transform library

A subset of MIOpen, which handles just the tensor transform operation.

## Features

* Based on a smaller subset of MIOpen library
* Compiles quickly with the host-based compiler (e.g. GCC)
* All dependencies are either ROCm or included
* A meaningful benchmark executale is included

## Building

Use your favorite tandem of CMake and Ninja:

```
mkdir build
cd build
cmake ..
ninja
```

