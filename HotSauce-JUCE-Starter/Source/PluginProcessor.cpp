#include "PluginProcessor.h"
#include "PluginEditor.h"

HotSauceAudioProcessor::HotSauceAudioProcessor()
: juce::AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
, analyzer (11) // 2^11 = 2048 FFT
, lastTargetIndex (-1)
, lastSpeedIndex (-1)
{
}

void HotSauceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    scratchMono.setSize (1, samplesPerBlock);
    wetDryBuffer.setSize (2, samplesPerBlock);
    eqDesigner.prepare (sampleRate, getTotalNumOutputChannels());
    dynStack.prepare (sampleRate, getTotalNumOutputChannels());
    tpLimiter.reset();
    tpLimiter.setRelease (50.0f);
    tpLimiter.setThreshold (0.0f); // soft safety; not true-peak but helps
}

bool HotSauceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void HotSauceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int nCh = buffer.getNumChannels();
    const int nSmps = buffer.getNumSamples();

    // Check if target profile changed
    int currentTargetIndex = (int) apvts.getRawParameterValue("target")->load();
    if (currentTargetIndex != lastTargetIndex)
    {
        lastTargetIndex = currentTargetIndex;
        juce::StringArray profiles {"Modern R&B", "Soulful Hip-Hop", "Alt Rock", "Custom Reference"};
        if (currentTargetIndex < profiles.size())
            eqDesigner.loadProfile (profiles[currentTargetIndex]);
    }
    
    // Check if analysis speed changed
    int currentSpeedIndex = (int) apvts.getRawParameterValue("speed")->load();
    if (currentSpeedIndex != lastSpeedIndex)
    {
        lastSpeedIndex = currentSpeedIndex;
        analyzer.setAnalysisSpeed (currentSpeedIndex);
    }

    // --- Analysis (mono sum)
    scratchMono.clear();
    for (int ch = 0; ch < nCh; ++ch)
        scratchMono.addFrom (0, 0, buffer, ch, 0, nSmps, 0.5f);
    analyzer.pushSamples (scratchMono.getReadPointer(0), nSmps);

    // --- Update EQ from analyzer vs. target profile
    if (analyzer.ready())
    {
        auto currentDb = analyzer.getSmoothedMagDb();
        auto targetDb  = eqDesigner.getTargetCurveDb();
        diffCurveDb = eqDesigner.computeDiff (currentDb, targetDb);
        eqDesigner.designFromDiff (diffCurveDb, apvts.getRawParameterValue("spice")->load());
    }

    // Store dry signal for wet/dry mix
    wetDryBuffer.makeCopyOf (buffer);

    // --- Apply processing (dynamic bands + static EQ biquads)
    dynStack.process (buffer, eqDesigner.getBands(), apvts);

    // --- TP guard (simple limiter for now)
    if (apvts.getRawParameterValue("tplim")->load() > 0.5f)
    {
        juce::dsp::AudioBlock<float> audioBlock (buffer);
        for (int ch=0; ch<nCh; ++ch)
        {
            auto channelBlock = audioBlock.getSingleChannelBlock (ch);
            juce::dsp::ProcessContextReplacing<float> context (channelBlock);
            tpLimiter.process (context);
        }
    }
    
    // --- Wet/Dry mix
    float wetDry = apvts.getRawParameterValue("wetdry")->load();
    if (wetDry < 0.999f) // Only mix if not 100% wet
    {
        for (int ch = 0; ch < nCh; ++ch)
        {
            auto* wet = buffer.getWritePointer (ch);
            auto* dry = wetDryBuffer.getReadPointer (ch);
            for (int i = 0; i < nSmps; ++i)
                wet[i] = dry[i] * (1.0f - wetDry) + wet[i] * wetDry;
        }
    }
}

void HotSauceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void HotSauceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr)
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout HotSauceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back (std::make_unique<juce::AudioParameterFloat>("spice","Spice",0.0f,1.0f,0.35f));
    p.push_back (std::make_unique<juce::AudioParameterChoice>("target","Target",
                    juce::StringArray{"Modern R&B","Soulful Hip-Hop","Alt Rock","Custom Reference"}, 0));
    p.push_back (std::make_unique<juce::AudioParameterChoice>("speed","Analysis Speed",
                    juce::StringArray{"Slow","Medium","Fast"}, 1));
    p.push_back (std::make_unique<juce::AudioParameterBool>("tplim","True Peak Guard", true));
    p.push_back (std::make_unique<juce::AudioParameterFloat>("wetdry","Wet/Dry", 0.0f,1.0f,1.0f));
    return { p.begin(), p.end() };
}

juce::AudioProcessorEditor* HotSauceAudioProcessor::createEditor()
{
    return new HotSauceAudioProcessorEditor (*this, apvts);
}
