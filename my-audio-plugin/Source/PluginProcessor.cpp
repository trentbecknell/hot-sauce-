#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters",
                 {
                     std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 2.0f, 1.0f),
                     std::make_unique<juce::AudioParameterFloat>("pan", "Pan", -1.0f, 1.0f, 0.0f),
                     std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f)
                 })
{
    gainParameter = parameters.getRawParameterValue("gain");
    panParameter = parameters.getRawParameterValue("pan");
    mixParameter = parameters.getRawParameterValue("mix");
}

PluginProcessor::~PluginProcessor()
{
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
    return false;
}

bool PluginProcessor::producesMidi() const
{
    return false;
}

bool PluginProcessor::isMidiEffect() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't have input
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get parameter values
    float gain = gainParameter->load();
    float pan = panParameter->load();
    float mix = mixParameter->load();

    // Create a copy of the dry signal for mixing
    juce::AudioBuffer<float> dryBuffer(buffer);

    // Process audio with gain and panning
    if (totalNumInputChannels >= 2)
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);
        
        // Calculate pan gains (constant power panning)
        float leftGain = std::cos((pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f) * gain;
        float rightGain = std::sin((pan + 1.0f) * juce::MathConstants<float>::pi / 4.0f) * gain;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float leftSample = leftChannel[sample];
            float rightSample = rightChannel[sample];
            
            // Apply gain and panning
            leftChannel[sample] = leftSample * leftGain + rightSample * leftGain * (pan < 0.0f ? -pan : 0.0f);
            rightChannel[sample] = rightSample * rightGain + leftSample * rightGain * (pan > 0.0f ? pan : 0.0f);
        }
    }
    else if (totalNumInputChannels == 1)
    {
        auto* channelData = buffer.getWritePointer(0);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= gain;
        }
    }

    // Apply dry/wet mix
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* wetData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            wetData[sample] = dryData[sample] * (1.0f - mix) + wetData[sample] * mix;
        }
    }
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
}

bool PluginProcessor::hasEditor() const
{
    return true;
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// This creates the plugin instance
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}