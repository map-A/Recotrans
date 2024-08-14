### 配置 1: Windows 10 + Visual Studio 2022 + CUDA 11.7 + Qt5

#### 环境设置

- **操作系统**: Windows 10
- **编译器**: Visual Studio 2022 (MSVC)
- **CUDA 版本**: CUDA 11.7
- **Qt 版本**: Qt 5.14.1（或其他兼容版本）

#### 安装步骤

1. **安装 Visual Studio 2022**:
   - 下载并安装 Visual Studio 2022，确保安装了“桌面开发 C++”工作负载。
   - 安装时还可以选择CUDA支持的附加组件。

2. **安装 CUDA 11.7**:
   - 下载并安装 CUDA 11.7 Toolkit。
   - 安装完成后，确保 CUDA 的 bin 和 lib 路径已添加到系统的 `PATH` 环境变量中。

3. **安装 Qt 5**:
   - 使用 Qt 在线安装程序安装 Qt 5.14.1（或所需版本）。
   - 确保选择 MSVC 2017/2019/2022 64-bit 版本。

4. **配置 CMake**:
   - 将 `Qt5_DIR` 设置为 `C:/Qt/Qt5.14.1/5.14.1/msvc2017_64/lib/cmake/Qt5`。
   - 在 CMakeLists.txt 中，添加路径到 CUDA 的头文件和库：
  
     ```cmake
     include_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/include")
     link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7/lib/x64")
     ```

#### 编译和运行

- 使用 Clion 选择msvc编译器，然后编译。

### 配置 2: Ubuntu 20.04 + GCC 9.9.3 + Qt5 (TODO)

#### 环境设置

- **操作系统**: Ubuntu 20.04
- **编译器**: GCC 9.9.3
- **Qt 版本**: Qt 5.14.x 或更高版本

1. **配置 CMake**:
   - 自动查找 Qt5 库，使用 CMake 时无需额外配置路径。

2. **CUDA 安装（可选）**:
   - 如果需要 CUDA，按照 NVIDIA 的官方文档安装合适版本。

#### 编译和运行 (TODO)

- 使用 CMake 编译项目：
  
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```

- 运行生成的可执行文件。

### 配置 3: macOS 15.6 + Clang + Qt5 (TODO)

#### 环境设置

- **操作系统**: macOS 15.6
- **编译器**: Clang（macOS 自带）
- **Qt 版本**: Qt 5.14.x 或更高版本

#### 安装步骤 (TODO)

1. **安装 Homebrew**:

   - 打开终端并安装 Homebrew：
  
     ```bash
     /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
     ```

2. **安装 Qt 5**:
   - 使用 Homebrew 安装 Qt 5：
  
     ```bash
     brew install qt5
     ```

3. **配置环境变量**:
   - 将 Qt 5 的 `qmake` 添加到路径：
  
     ```bash
     export PATH="/usr/local/opt/qt/bin:$PATH"
     ```

4. **配置 CMake**:
   - CMake 将自动检测 Clang 和 Qt 5 的安装，无需额外配置。

#### 编译和运行 (TODO)

- 使用 CMake 编译项目：
  ```bash
  mkdir build
  cd build
  cmake ..
  make
  ```

- 运行生成的可执行文件。

---


### 项目文件说明

1. main.cpp
描述: 项目的主入口文件，包含程序的 main 函数。这个文件通常负责初始化应用程序的关键组件，设置主窗口，启动主事件循环，并调用其他模块的功能。
1. floatingWindow.cpp 和 floatingWindow.h
描述:包括界面的设计

1. stream.h 和 stream.cpp
描述:
获取音频流
1. audio.h 和 audio.cpp
描述:管理音频接口
1. stringgenerator.h 和 stringgenerator.cpp
描述:使用whisper 解析音频流，生成字符串

### TODO: 

1. 识别文字分割优化
2. qt界面完善，计划做成歌词滚动效果
