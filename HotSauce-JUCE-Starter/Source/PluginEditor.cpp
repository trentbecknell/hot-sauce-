#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "EQDesigner.h"

HotSauceAudioProcessorEditor::HotSauceAudioProcessorEditor (HotSauceAudioProcessor& p, juce::AudioProcessorValueTreeState& s)
: juce::AudioProcessorEditor (&p), processor (p), apvts (s)
{
    setSize (520, 340);

    spice.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    spice.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
    addAndMakeVisible (spice);

    target.addItemList (juce::StringArray{"Modern R&B","Soulful Hip-Hop","Alt Rock","Custom Reference"}, 1);
    addAndMakeVisible (target);

    addAndMakeVisible (tpGuard);

    spiceAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spice", spice);
    targetAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "target", target);
    tpAtt     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "tplim", tpGuard);

    startTimerHz (30);
}

void HotSauceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawText ("Hot Sauce — Industry-Tilt EQ (Starter)", 10, 6, getWidth()-20, 20, juce::Justification::centred);

    // simple spectrum visualization (placeholder)
    auto area = getLocalBounds().reduced(10).withTop(60).withHeight(200);
    g.setColour (juce::Colours::darkgrey);
    g.fillRect (area);
    g.setColour (juce::Colours::orange);
    g.strokePath (currentPath, juce::PathStrokeType(1.5f));
    g.setColour (juce::Colours::cornflowerblue);
    g.strokePath (targetPath, juce::PathStrokeType(1.5f));
    g.setColour (juce::Colours::lightgrey);
    g.drawFittedText ("Orange = current • Blue = target", area.removeFromBottom(20), juce::Justification::centred, 1);
}

void HotSauceAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(10);
    auto top = r.removeFromTop(44);
    spice.setBounds (top.removeFromLeft(160).reduced(10));
    target.setBounds (top.removeFromLeft(180).reduced(10));
    tpGuard.setBounds (top.removeFromLeft(120).reduced(10));
}

void HotSauceAudioProcessorEditor::timerCallback()
{
    // In a fuller build, you'd pull analyzer curves. Here we just draw static placeholders.
    auto area = getLocalBounds().reduced(10).withTop(60).withHeight(200);

    currentPath.clear(); targetPath.clear();
    auto toXY = [area](float normX, float normY){
        const float x = area.getX() + normX * area.getWidth();
        const float y = area.getBottom() - normY * area.getHeight();
        return juce::Point<float>(x,y);
    };

    // Dummy shapes for now
    currentPath.startNewSubPath (toXY(0.0f, 0.2f));
    targetPath.startNewSubPath  (toXY(0.0f, 0.3f));
    for (int i=1;i<=32;++i)
    {
        float x = i / 32.0f;
        float yCur = 0.2f + 0.1f * std::sin (x * juce::MathConstants<float>::twoPi);
        float yTar = 0.3f + 0.05f * std::cos (x * juce::MathConstants<float>::twoPi);
        currentPath.lineTo (toXY(x, yCur));
        targetPath.lineTo  (toXY(x, yTar));
    }
    repaint();
}
