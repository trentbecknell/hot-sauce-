
# Hot Sauce — JUCE Starter (VST3/AU)

This is a minimal, compiling starter for your **Hot Sauce** "industry-tilt" EQ plugin. 
Open it in **Visual Studio 2022** via CMake and build VST3 (and AU on macOS).

## Quick Start (Windows, Visual Studio 2022)

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
The first configure step will automatically fetch **JUCE** with submodules (including the VST3 SDK).

3) Find the plugin
- Windows VST3: `build/HotSauce_artefacts/Release/VST3/Hot Sauce.vst3`
- Copy it to your VST3 folder if needed: `C:\Program Files\Common Files\VST3`

4) Load in your DAW and insert on a track or mix bus.

---

## What’s inside

- **Spice** control: sets intensity (0..1)
- **Target** dropdown: Modern R&B, Soulful Hip-Hop, Alt Rock, Custom Reference (placeholders)
- Live analyzer (mono sum), smoothed spectrum
- Curve → 8-band parametric EQ design (placeholder implementation — safe, subtle)
- TP guard and Wet/Dry (scaffolded)

The heavier ML / profile work comes later; this starter is clean C++ you can extend.

---

## Editing in Visual Studio

Open the folder in VS (`File → Open → Folder...`) or open the generated solution in `build/` after configure.
Set **HotSauce** target and **Release** config, then build.

---

## Reference Profiles

Edit `Resources/TargetProfiles.json` to adjust target curves and LUFS. You can wire up a "Load Reference" button later to analyze a WAV and save JSON.

---

## Notes

- This project uses **JUCE CMake**, not Projucer.
- The included analyzer/EQ is intentionally simple and safe; it won't create unstable filters or extreme boosts.
- Marketing: say “reference-grade profiles and platform-friendly loudness” rather than implying proprietary UMG/Sony/Spotify data.

Enjoy!
