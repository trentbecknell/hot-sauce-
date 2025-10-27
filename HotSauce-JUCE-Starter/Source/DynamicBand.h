#pragma once
#include <JuceHeader.h>
#include "EQDesigner.h"

class DynamicBandStack
{
public:
    void prepare (double sampleRate, int numChannels)
    {
        sr = sampleRate; ch = numChannels;
        smoothers.clear();
        smoothers.resize (8);
        for (auto& s : smoothers) s.reset (sampleRate, 0.05); // 50ms
    }

    void process (juce::AudioBuffer<float>& buffer, const std::vector<DesignedBand>& bands, juce::AudioProcessorValueTreeState& apvts)
    {
        // Simple static EQ application (biquads) + tiny dynamics placeholder.
        const int n = buffer.getNumSamples();
        const int nc = buffer.getNumChannels();
        juce::dsp::AudioBlock<float> block (buffer);

        // Apply biquads
        for (size_t i=0;i<bands.size();++i)
        {
            for (int c=0;c<juce::jmin(2, nc); ++c)
                bands[i].biq[c].process (juce::dsp::ProcessContextReplacing<float>(block.getSingleChannelBlock (c)));
        }

        // Wet/dry could be added here (omitted for brevity in starter)
        juce::ignoreUnused (apvts, n);
    }

private:
    double sr = 44100.0;
    int ch = 2;

    std::vector<juce::SmoothedValue<float>> smoothers;
};
