#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "EQDesigner.h"

HotSauceAudioProcessorEditor::HotSauceAudioProcessorEditor (HotSauceAudioProcessor& p, juce::AudioProcessorValueTreeState& s)
: juce::AudioProcessorEditor (&p), processor (p), apvts (s)
{
    setSize (600, 420);

    // Spice slider
    spice.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    spice.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
    addAndMakeVisible (spice);

    // Target profile dropdown
    target.addItemList (juce::StringArray{"Modern R&B","Soulful Hip-Hop","Alt Rock","Custom Reference"}, 1);
    addAndMakeVisible (target);
    
    // Analysis speed dropdown
    speed.addItemList (juce::StringArray{"Slow", "Medium", "Fast"}, 1);
    addAndMakeVisible (speed);
    
    // Wet/Dry slider
    wetDry.setSliderStyle (juce::Slider::LinearHorizontal);
    wetDry.setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 16);
    wetDry.setRange (0.0, 1.0, 0.01);
    wetDry.setNumDecimalPlacesToDisplay (0);
    wetDry.textFromValueFunction = [](double value) { return juce::String((int)(value * 100.0)) + "%"; };
    addAndMakeVisible (wetDry);

    // TP Guard toggle
    addAndMakeVisible (tpGuard);
    
    // Bypass toggle
    addAndMakeVisible (bypass);

    // Attachments
    spiceAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "spice", spice);
    targetAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "target", target);
    speedAtt  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "speed", speed);
    wetDryAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "wetdry", wetDry);
    tpAtt     = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "tplim", tpGuard);
    bypassAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "bypass", bypass);

    startTimerHz (30);
}

void HotSauceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawText ("Hot Sauce — Industry-Tilt EQ", 10, 6, getWidth()-20, 24, juce::Justification::centred);

    // Draw spectrum visualization area
    auto area = getLocalBounds().reduced(15).withTop(90).withHeight(220);
    g.setColour (juce::Colours::darkgrey.darker());
    g.fillRect (area);
    
    // Draw frequency grid lines (vertical)
    g.setColour (juce::Colours::grey.withAlpha(0.3f));
    const float freqs[] = {50, 100, 200, 500, 1000, 2000, 5000, 10000};
    for (float freq : freqs)
    {
        float x = std::log(freq / 20.0f) / std::log(20000.0f / 20.0f);
        int xPos = area.getX() + (int)(x * area.getWidth());
        g.drawLine (xPos, area.getY(), xPos, area.getBottom(), 1.0f);
    }
    
    // Draw dB grid lines (horizontal)
    for (int db = -40; db <= 20; db += 10)
    {
        float y = 1.0f - (db + 60.0f) / 80.0f; // Map -60 to +20 dB
        int yPos = area.getY() + (int)(y * area.getHeight());
        g.drawLine (area.getX(), yPos, area.getRight(), yPos, 1.0f);
    }
    
    // Draw current spectrum
    g.setColour (juce::Colours::orange.withAlpha(0.8f));
    g.strokePath (currentPath, juce::PathStrokeType(2.0f));
    
    // Draw target spectrum
    g.setColour (juce::Colours::cornflowerblue.withAlpha(0.8f));
    g.strokePath (targetPath, juce::PathStrokeType(2.0f));
    
    // Legend
    g.setFont (12.0f);
    g.setColour (juce::Colours::orange);
    g.drawText ("Current", area.getX() + 10, area.getBottom() + 5, 60, 20, juce::Justification::left);
    g.setColour (juce::Colours::cornflowerblue);
    g.drawText ("Target", area.getX() + 80, area.getBottom() + 5, 60, 20, juce::Justification::left);
    
    // Draw EQ band gains
    g.setColour (juce::Colours::lightgreen);
    g.setFont (10.0f);
    auto& bands = processor.getEQDesigner().getBands();
    juce::String bandInfo = "EQ Bands: ";
    bool hasActiveBands = false;
    for (size_t i = 0; i < bands.size(); ++i)
    {
        if (std::fabs(bands[i].gainDb) > 0.1f)
        {
            if (hasActiveBands)
                bandInfo << "  ";
            bandInfo << juce::String((int)bands[i].f0) << "Hz:" << juce::String(bands[i].gainDb, 1) << "dB";
            hasActiveBands = true;
        }
    }
    g.drawText (bandInfo, area.getX() + 160, area.getBottom() + 5, area.getWidth() - 280, 20, juce::Justification::left);
    
    // Draw labels for controls
    g.setColour (juce::Colours::lightgrey);
    g.setFont (11.0f);
    auto topArea = getLocalBounds().reduced(15).removeFromTop(80);
    
    auto row1 = topArea.removeFromTop(35);
    g.drawText ("Spice", row1.getX(), row1.getY(), 100, 15, juce::Justification::left);
    g.drawText ("Target Profile", row1.getX() + 110, row1.getY(), 120, 15, juce::Justification::left);
    g.drawText ("Speed", row1.getX() + 250, row1.getY(), 100, 15, juce::Justification::left);
    
    auto row2 = topArea.removeFromTop(35);
    g.drawText ("Wet/Dry", row2.getX(), row2.getY(), 80, 15, juce::Justification::left);
    
    // Show target LUFS
    float targetLUFS = processor.getEQDesigner().getTargetLUFS();
    g.setColour (juce::Colours::yellow);
    g.drawText (juce::String("Target: ") + juce::String(targetLUFS, 1) + " LUFS", 
                area.getRight() - 120, area.getBottom() + 5, 120, 20, juce::Justification::right);
}

void HotSauceAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(15);
    auto top = r.removeFromTop(80);
    
    // Row 1: Spice, Target, Speed, TP Guard, Bypass
    auto row1 = top.removeFromTop(35);
    row1.removeFromTop(15); // space for label
    spice.setBounds (row1.removeFromLeft(100).reduced(2));
    target.setBounds (row1.removeFromLeft(130).reduced(2));
    speed.setBounds (row1.removeFromLeft(110).reduced(2));
    tpGuard.setBounds (row1.removeFromLeft(90).reduced(2));
    bypass.setBounds (row1.removeFromLeft(80).reduced(2));
    
    // Row 2: Wet/Dry
    auto row2 = top.removeFromTop(35);
    row2.removeFromTop(15); // space for label
    wetDry.setBounds (row2.removeFromLeft(280).reduced(2));
}

void HotSauceAudioProcessorEditor::timerCallback()
{
    // Get real analyzer data
    lastCurrent = processor.getAnalyzer().getSmoothedMagDb();
    lastTarget = processor.getEQDesigner().getTargetCurveDb();
    
    auto area = getLocalBounds().reduced(15).withTop(90).withHeight(220);

    auto toXY = [area](float normX, float normDb){
        const float x = area.getX() + normX * area.getWidth();
        // Map dB range: -60 to +20 dB
        const float normY = juce::jlimit(0.0f, 1.0f, (normDb + 60.0f) / 80.0f);
        const float y = area.getBottom() - normY * area.getHeight();
        return juce::Point<float>(x, y);
    };

    // Draw current spectrum
    currentPath.clear();
    if (!lastCurrent.empty())
    {
        currentPath.startNewSubPath (toXY(0.0f, lastCurrent[0]));
        for (size_t i = 1; i < lastCurrent.size(); ++i)
        {
            float x = (float)i / (float)(lastCurrent.size() - 1);
            currentPath.lineTo (toXY(x, lastCurrent[i]));
        }
    }

    // Draw target spectrum
    targetPath.clear();
    if (!lastTarget.empty())
    {
        targetPath.startNewSubPath (toXY(0.0f, lastTarget[0]));
        for (size_t i = 1; i < lastTarget.size(); ++i)
        {
            float x = (float)i / (float)(lastTarget.size() - 1);
            targetPath.lineTo (toXY(x, lastTarget[i]));
        }
    }
    
    repaint();
}
