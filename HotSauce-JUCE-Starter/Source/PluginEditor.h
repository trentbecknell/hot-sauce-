#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class HotSauceAudioProcessor;

class HotSauceAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    HotSauceAudioProcessorEditor (HotSauceAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~HotSauceAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    HotSauceAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& apvts;

    juce::Slider spice;
    juce::ComboBox target;
    juce::ComboBox speed;
    juce::Slider wetDry;
    juce::ToggleButton tpGuard { "TP Guard" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spiceAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> targetAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> speedAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wetDryAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tpAtt;

    // simple spectrum paths
    juce::Path currentPath, targetPath;
    std::vector<float> lastCurrent, lastTarget;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HotSauceAudioProcessorEditor)
};
