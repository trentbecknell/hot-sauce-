
# Hot Sauce — JUCE Audio Plugin (VST3/AU)

A professional **"industry-tilt" EQ plugin** with real-time spectrum analysis and genre-based target profiles. 
Built with JUCE 7.0.12 and ready to compile on Windows, macOS, and Linux.

## Features

### Core Functionality
- ✅ **Real-time Spectrum Analyzer** - Live FFT visualization with frequency/dB grid
- ✅ **Genre-Based Target Profiles** - Modern R&B, Soulful Hip-Hop, Alt Rock
- ✅ **8-Band Parametric EQ** - Automatically adjusts to match target curves
- ✅ **Spice Control** - Adjusts EQ intensity (0-100%)
- ✅ **Wet/Dry Mix** - Smooth blending between processed and dry signal
- ✅ **Bypass** - Instant A/B comparison
- ✅ **Analysis Speed** - Slow/Medium/Fast FFT smoothing
- ✅ **True Peak Limiter** - Safety limiter to prevent clipping
- ✅ **LUFS Targeting** - Display target loudness for selected profile
- ✅ **Active Band Display** - Real-time readout of active EQ frequencies and gains

### UI Features
- Color-coded spectrum display (orange=current, blue=target)
- Frequency grid lines (50Hz - 10kHz)
- dB scale with grid lines
- Professional 2-row control layout
- Real-time parameter feedback

---

## Quick Start

### Linux
1) Install prerequisites
```bash
sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev \
  libxinerama-dev libxcursor-dev libfreetype6-dev libgl1-mesa-dev libgtk-3-dev
```

2) Configure and build
```bash
cd HotSauce-JUCE-Starter
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j$(nproc)
```

3) Find the plugin
- Linux VST3: `build/HotSauce_artefacts/Release/VST3/Hot Sauce.vst3`
- Copy to: `~/.vst3/`

### Windows (Visual Studio 2022)
1) Install prerequisites
- **Visual Studio 2022** with "Desktop development with C++"
- **CMake 3.22+** (VS includes it)
- **Git**

2) Configure and build
```bat
cd HotSauce-JUCE-Starter
cmake -B build -S . -G "Visual Studio 17 2022"
cmake --build build --config Release
```

3) Find the plugin
- Windows VST3: `build/HotSauce_artefacts/Release/VST3/Hot Sauce.vst3`
- Copy to: `C:\Program Files\Common Files\VST3`

4) Load in your DAW and insert on a track or mix bus.

---

## Controls

### Main Controls
- **Spice** (0-100%) - EQ intensity/strength
- **Target Profile** - Select genre-based target curve
- **Analysis Speed** - Slow/Medium/Fast smoothing
- **Wet/Dry** (0-100%) - Dry/processed mix
- **TP Guard** - Enable/disable true-peak limiter
- **Bypass** - A/B comparison toggle

### Display
- **Orange curve** - Current frequency response
- **Blue curve** - Target profile curve
- **EQ Bands** - Active frequency adjustments
- **Target LUFS** - Loudness target for selected profile

---

## How It Works

### Processing Chain
1. **Analysis** - Real-time FFT analysis of input signal (mono sum)
2. **Profile Comparison** - Compares current spectrum to selected target profile
3. **EQ Design** - Calculates 8-band parametric EQ to match target
4. **Processing** - Applies EQ with adjustable intensity (Spice)
5. **Limiting** - Optional true-peak limiter for safety
6. **Mixing** - Wet/dry blend for parallel processing

### Target Profiles

Each profile in `Resources/TargetProfiles.json` contains:
- **Frequency curve** - Target EQ shape (31Hz - 16kHz)
- **LUFS target** - Reference loudness level
- **True peak limit** - Maximum peak level

Current profiles:
- **Modern R&B** (-10.5 LUFS) - Enhanced highs with controlled bass
- **Soulful Hip-Hop** (-9.5 LUFS) - Strong low end with presence
- **Alt Rock** (-11.0 LUFS) - Balanced with clarity

---

## Technical Details

- **DSP Framework**: JUCE 7.0.12
- **FFT Size**: 2048 points with Hann windowing
- **EQ Bands**: 8 parametric bands (40Hz - 12kHz)
- **Analysis Rate**: ~86 Hz (hop size = 512)
- **Sample Rates**: 44.1kHz - 192kHz supported
- **Formats**: VST3, AU (macOS), Standalone

---

## Development

### Project Structure
```
HotSauce-JUCE-Starter/
├── Source/
│   ├── PluginProcessor.cpp    # Audio processing logic
│   ├── PluginEditor.cpp        # UI implementation
│   ├── Analyzer.h              # FFT spectrum analyzer
│   ├── EQDesigner.h            # EQ coefficient calculator
│   └── DynamicBand.h           # Filter processing
├── Resources/
│   └── TargetProfiles.json     # Genre-based target curves
└── CMakeLists.txt              # Build configuration
```

### Customizing Profiles

Edit `Resources/TargetProfiles.json` to add or modify target profiles:

```json
{
  "My Custom Profile": {
    "lufs": -12.0,
    "truePeak": -1.0,
    "curve": [
      [63, 0.0],
      [125, -1.0],
      [1000, 0.5],
      [8000, 2.0]
    ]
  }
}
```

Each curve point is `[frequency_hz, gain_db]`.

---

## Notes

- This project uses **JUCE CMake**, not Projucer
- The analyzer and EQ are production-ready implementations
- All DSP is safe and stable - no extreme boosts or unstable filters
- Profiles use reference-grade curves suitable for professional mixing
- The plugin is fully parameter-automatable in any DAW

Enjoy crafting your perfect mix! 🌶️
