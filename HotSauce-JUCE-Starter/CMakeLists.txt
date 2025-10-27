cmake_minimum_required(VERSION 3.22)

project(HotSauce VERSION 0.1.0)

# ===== User options =====
# If you already have JUCE installed locally, set JUCE_DIR to its path when configuring:
#   cmake -B build -DJUCE_DIR="C:/path/to/JUCE"
#
# Otherwise, we'll fetch JUCE from GitHub (with submodules for VST3).

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

if (NOT DEFINED JUCE_DIR)
    message(STATUS "JUCE_DIR not specified; fetching JUCE...")
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.12
        GIT_SHALLOW OFF
    )
    FetchContent_MakeAvailable(juce)
else()
    message(STATUS "Using JUCE from: ${JUCE_DIR}")
    add_subdirectory(${JUCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/juce-build EXCLUDE_FROM_ALL)
endif()

# Enable VST3 and AU builds on supported platforms
set(JUCE_FORMATS VST3)
if (APPLE)
    list(APPEND JUCE_FORMATS AU)
endif()

# Resources
set(PROFILES_JSON ${CMAKE_CURRENT_SOURCE_DIR}/Resources/TargetProfiles.json)

juce_add_plugin(HotSauce
    COMPANY_NAME "Family Dinner Audio"
    BUNDLE_ID com.familydinner.hotsauce
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE FDa1
    PLUGIN_CODE HtSc
    VST3_CATEGORIES "Fx"
    FORMATS ${JUCE_FORMATS}
    PRODUCT_NAME "Hot Sauce"
)

target_sources(HotSauce PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Analyzer.h
    Source/Analyzer.cpp
    Source/EQDesigner.h
    Source/EQDesigner.cpp
    Source/DynamicBand.h
    Source/DynamicBand.cpp
)

target_compile_definitions(HotSauce
    PUBLIC
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        HOTSAUCE_PROFILES_JSON=\"$<IF:$<CONFIG:Debug>,${PROFILES_JSON},${PROFILES_JSON}>\")

target_link_libraries(HotSauce
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
)
