#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(400, 300);

    // Setup Gain Slider
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(gainSlider);
    
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);
    
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getValueTreeState(), "gain", gainSlider);

    // Setup Pan Slider
    panSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(panSlider);
    
    panLabel.setText("Pan", juce::dontSendNotification);
    panLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(panLabel);
    
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getValueTreeState(), "pan", panSlider);

    // Setup Mix Slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(mixSlider);
    
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getValueTreeState(), "mix", mixSlider);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("My Audio Plugin", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60); // Space for title
    
    auto sliderWidth = 100;
    auto sliderHeight = 120;
    auto spacing = (bounds.getWidth() - (sliderWidth * 3)) / 4;
    
    // Position gain slider
    auto gainBounds = bounds.removeFromLeft(spacing).withWidth(sliderWidth);
    gainBounds.setY(80);
    gainBounds.setHeight(sliderHeight);
    gainLabel.setBounds(gainBounds.removeFromTop(20));
    gainSlider.setBounds(gainBounds);
    
    // Position pan slider
    auto panBounds = bounds.removeFromLeft(spacing + sliderWidth).withWidth(sliderWidth);
    panBounds.setY(80);
    panBounds.setHeight(sliderHeight);
    panLabel.setBounds(panBounds.removeFromTop(20));
    panSlider.setBounds(panBounds);
    
    // Position mix slider
    auto mixBounds = bounds.removeFromLeft(spacing + sliderWidth).withWidth(sliderWidth);
    mixBounds.setY(80);
    mixBounds.setHeight(sliderHeight);
    mixLabel.setBounds(mixBounds.removeFromTop(20));
    mixSlider.setBounds(mixBounds);
}