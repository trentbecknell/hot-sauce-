# My Audio Plugin

[![Build Audio Plugin](https://github.com/trentbecknell/hot-sauce-/actions/workflows/build-plugin.yml/badge.svg)](https://github.com/trentbecknell/hot-sauce-/actions/workflows/build-plugin.yml)

## Overview
This is a JUCE-based audio plugin that works in VST3, AU (macOS), and Standalone formats. It can be loaded in any compatible DAW (Digital Audio Workstation) for audio processing.

## Download

### Pre-built Binaries
Download the latest pre-built plugin for your platform:

**[Download Latest Release](https://github.com/trentbecknell/hot-sauce-/releases/latest)**

Or check the [Actions tab](https://github.com/trentbecknell/hot-sauce-/actions) for the latest development builds.

### Installation

**Windows:**
1. Download `MyAudioPlugin-Windows-VST3.zip`
2. Extract the `My Audio Plugin.vst3` folder
3. Copy it to: `C:\Program Files\Common Files\VST3\`

**macOS:**
1. Download `MyAudioPlugin-macOS-VST3.tar.gz` or `MyAudioPlugin-macOS-AU.tar.gz`
2. Extract the archive
3. Copy to:
   - VST3: `~/Library/Audio/Plug-Ins/VST3/`
   - AU: `~/Library/Audio/Plug-Ins/Components/`

**Linux:**
1. Download `MyAudioPlugin-Linux-VST3.tar.gz`
2. Extract: `tar -xzf MyAudioPlugin-Linux-VST3.tar.gz`
3. Copy to: `~/.vst3/`

After installation, rescan plugins in your DAW.

## Features
- Audio passthrough processing
- Cross-platform support (Windows, macOS, Linux)
- VST3 and AU plugin formats
- Standalone application included

## Project Structure
```
my-audio-plugin
├── Source
│   ├── PluginProcessor.cpp      # Audio processing logic
│   ├── PluginProcessor.h        # Audio processor class declaration
│   ├── PluginEditor.cpp         # User interface implementation
│   ├── PluginEditor.h           # User interface class declaration
│   └── PluginParameters.h        # Parameter definitions
├── Builds
│   ├── CMakeLists.txt           # CMake configuration for building
│   └── Makefile                  # Makefile for building the project
├── Tests
│   └── test_plugin.cpp           # Unit tests for the plugin
├── CMakeLists.txt               # Main CMake configuration
├── .gitignore                   # Git ignore file
├── LICENSE                      # Licensing information
└── README.md                    # Project documentation
```

## Building from Source

### Prerequisites
- CMake 3.15 or higher
- C++17 compatible compiler
- Git

**Linux dependencies:**
```bash
sudo apt-get install libasound2-dev libx11-dev libxrandr-dev \
  libxinerama-dev libxcursor-dev libfreetype6-dev libgl1-mesa-dev
```

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/trentbecknell/hot-sauce-.git
   cd hot-sauce-/my-audio-plugin
   ```

2. Clone JUCE framework:
   ```bash
   cd ..
   git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git
   cd my-audio-plugin
   ```

3. Build the project:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release -j$(nproc)
   ```

4. The built plugins will be in `build/MyAudioPlugin_artefacts/`

## Development

The plugin is built with JUCE 7.0.12 and uses CMake as the build system.

**Project Structure:**
- `Source/PluginProcessor.cpp` - Audio processing logic
- `Source/PluginEditor.cpp` - User interface
- `CMakeLists.txt` - Build configuration

## Usage
Load the plugin in your DAW and it will pass audio through. You can modify the `processBlock()` function in `PluginProcessor.cpp` to add custom audio processing.

## Creating a Release

To create a downloadable release:
1. Create and push a version tag:
   ```bash
   git tag v1.0.0
   git push origin v1.0.0
   ```
2. GitHub Actions will automatically build for all platforms and create a release

## Contribution
Contributions are welcome! Please submit a pull request or open an issue for any suggestions or improvements.