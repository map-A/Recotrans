
## Project Overview: Real-time Audio Capture and Recognition using Whisper + RtAudio

### Configuration 1: Windows 10 + Visual Studio 2022 + CUDA 11.7 + Qt5

#### Environment Setup

- **Operating System**: Windows 10
- **Compiler**: Visual Studio 2022 (MSVC)
- **CUDA Version**: CUDA 11.7
- **Qt Version**: Qt 5.14.1 (or any compatible version)

#### Installation Steps

1. **Install Visual Studio 2022**:
   - Download and install Visual Studio 2022, ensuring the "Desktop development with C++" workload is installed.

2. **Install CUDA 11.7**:
   - Download and install the CUDA 11.7 Toolkit.
   - After installation, ensure that the CUDA `bin` and `lib` paths are added to the system `PATH` environment variable.

3. **Install Qt 5**:
   - Install Qt 5.14.1 (or the required version) using the Qt Online Installer.
   - Ensure that you select the MSVC 2017/2019/2022 64-bit version.

4. **Configure CMake**:
   - Set the `Qt5_DIR` to `C:/Qt/Qt5.14.1/5.14.1/msvc2017_64/lib/cmake/Qt5`.
   - In the `CMakeLists.txt`, add the paths to the CUDA headers and libraries:

     ```cmake
     include_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/include")
     link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/lib/x64")
     ```

#### Build and Run

   ```
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release  -DGGML_CUDA:BOOL=ON  -G Ninja -S . -B ./build
   cmake --build ./build  --target rtaudio -j 4
   cmake --build ./build  --target lyric -j 4
   ```

### Configuration 2: Ubuntu 20.04 + GCC 9.9.3 + Qt5 (TODO)

#### Environment Setup

- **Operating System**: Ubuntu 20.04
- **Compiler**: GCC 9.9.3
- **Qt Version**: Qt 5.14.x or higher

1. **Configure CMake**:
   - Automatically find the Qt5 libraries; no additional path configuration is needed during CMake setup.

2. **CUDA Installation (Optional)**:
   - If CUDA is needed, follow the official NVIDIA documentation to install the appropriate version.

#### Build and Run (TODO)

- Build the project using CMake:
  
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```

- Run the generated executable.

### Configuration 3: macOS 15.6 + Clang + Qt5 (TODO)

#### Environment Setup

- **Operating System**: macOS 15.6
- **Compiler**: Clang (included with macOS)
- **Qt Version**: Qt 5.14.x or higher

#### Installation Steps (TODO)

1. **Install Homebrew**:

   - Open Terminal and install Homebrew:
  
     ```bash
     /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
     ```

2. **Install Qt 5**:
   - Install Qt 5 using Homebrew:
  
     ```bash
     brew install qt5
     ```

3. **Configure Environment Variables**:
   - Add Qt 5’s `qmake` to the PATH:
  
     ```bash
     export PATH="/usr/local/opt/qt/bin:$PATH"
     ```

4. **Configure CMake**:
   - CMake will automatically detect the installation of Clang and Qt 5, so no additional configuration is necessary.

#### Build and Run (TODO)

- Build the project using CMake:
  
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```

- Run the generated executable.

---

### Project File Description

1. **main.cpp**
   - **Description**: The main entry file of the project, containing the main function. This file is typically responsible for initializing key components of the application, setting up the main window, starting the main event loop, and invoking the functions of other modules.

2. **lyric.cpp and lyric.h**
   - **Description**: Includes the interface design.

3. **inference.h and inference.cpp**
   - **Description**: Handles audio inference using Whisper.

4. **audio.h and audio.cpp**
   - **Description**: Manages audio interfaces.

5. **stringgenerator.h and stringgenerator.cpp**
   - **Description**: Generates strings and passes them to the GUI.

### TODO:

1. Optimize text segmentation for recognition.
