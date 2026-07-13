# How to build the assignment tracker 

This project uses CMake & Requires Qt 6 (Widgets & SQL modules) 
Follow the install methods for your system. 


## Pre-reqs 

### macOS (apple silicon / intel) 

- easiest way to install Qt6 --> homebrew  

```bash
brew install qt
```

### windows 
- must have Qt6 & CMake installed 
    - can use **Qt Online Installer**[https://www.qt.io/development/download-qt-installer-oss]
    - or use CLI tool **aqtinstall** 
    - ensure CMake added to system PATH 


## Build Instructions 

### macOS 
- navigate to root directory 
- create build directory & cd into it: 

```bash
mkdir build 
cd build
``` 
- point brew-based Qt install location in CMake 

```bash
cmake -DCMAKE_MAKE_PATH=$(brew --prefix qt) .. 
```
- compile the code: 

```bash
make 
```

- run the executable:  

```bash
./Tracker
```

### Windows (PowerShell or CMD) 
- open terminal & navigate to root of project directory 
- create a build directory & move into it: 

```DOS
mkdir build 
cd build 
```

- tell CMake to configure the project 
- **Note:** replace the path below with exact location where Qt was installed on your machine: 

```DOS 
cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.7.2\msvc2019_64" .. 
``` 

- compile the code using CMake's build command: 

```DOS 
cmake --build .
```
- run the executable 

```
.\Debug\Tracker.exe
```

