project(MyAudioPlugin VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)

# Specify the source files
set(SOURCES
    ../Source/PluginProcessor.cpp
    ../Source/PluginEditor.cpp
)

# Include directories
include_directories(../Source)

# Add the plugin library
add_library(MyAudioPlugin SHARED ${SOURCES})

# Specify the plugin's output name
set_target_properties(MyAudioPlugin PROPERTIES
    OUTPUT_NAME "MyAudioPlugin"
    PREFIX ""
)

# Link against necessary audio plugin frameworks
find_package(JUCE REQUIRED)
target_link_libraries(MyAudioPlugin JUCE::JUCE)

# Set the plugin's version
set_target_properties(MyAudioPlugin PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Install rules
install(TARGETS MyAudioPlugin
    DESTINATION lib
)