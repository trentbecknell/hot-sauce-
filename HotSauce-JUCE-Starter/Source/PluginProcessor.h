#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Analyzer.h"
#include "EQDesigner.h"
#include "DynamicBand.h"

class HotSauceAudioProcessor : public juce::AudioProcessor
{
public:
    HotSauceAudioProcessor();
    ~HotSauceAudioProcessor() override = default;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "Hot Sauce"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "PARAMS", createParameterLayout() };
    
    // Public accessors for UI
    Analyzer& getAnalyzer() { return analyzer; }
    EQDesigner& getEQDesigner() { return eqDesigner; }

private:
    Analyzer analyzer;
    EQDesigner eqDesigner;
    DynamicBandStack dynStack;

    juce::AudioBuffer<float> scratchMono;
    juce::AudioBuffer<float> wetDryBuffer;
    std::vector<float> diffCurveDb;
    juce::dsp::Limiter<float> tpLimiter;
    
    int lastTargetIndex = -1;
    int lastSpeedIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HotSauceAudioProcessor)
};
