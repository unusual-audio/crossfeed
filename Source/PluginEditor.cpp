/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CrossfeedAudioProcessorEditor::CrossfeedAudioProcessorEditor (CrossfeedAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    amplitudeLowAttachment(*p.amplitudeLow, amplitudeLowSlider),
    amplitudeHighAttachment(*p.amplitudeHigh, amplitudeHighSlider),
    delayLowAttachment(*p.delayLow, delayLowSlider),
    delayHighAttachment(*p.delayHigh, delayHighSlider),
    crossoverFrequencyAttachment(*p.crossoverFrequency, crossoverFrequencySlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (440, 360);
    
    
    addAndMakeVisible (amplitudeLowSlider);
    amplitudeLowSlider.setTextValueSuffix ("%");
    amplitudeLowSlider.setRange(amplitudeLowSlider.getRange(), 0.01f);
    amplitudeLowSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value * 100);
    };
    amplitudeLowSlider.updateText();
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    amplitudeLowSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    
    addAndMakeVisible (amplitudeHighSlider);
    amplitudeHighSlider.setTextValueSuffix ("%");
    amplitudeHighSlider.setRange(amplitudeHighSlider.getRange(), 0.01f);
    amplitudeHighSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value * 100);
    };
    amplitudeHighSlider.updateText();
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    amplitudeHighSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);

    
    addAndMakeVisible (delayLowSlider);
    delayLowSlider.setTextValueSuffix (" us");
    delayLowSlider.setRange(delayLowSlider.getRange(), 1.f);
    delayLowSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    delayLowSlider.updateText();
    delayLowSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    delayLowSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    delayLowSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    delayLowSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    delayLowSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    delayLowSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    delayLowSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    
    addAndMakeVisible (delayHighSlider);
    delayHighSlider.setTextValueSuffix (" us");
    delayHighSlider.setRange(delayHighSlider.getRange(), 1.f);
    delayHighSlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    delayHighSlider.updateText();
    delayHighSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    delayHighSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    delayHighSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    delayHighSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    delayHighSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    delayHighSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    delayHighSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    
    addAndMakeVisible (crossoverFrequencySlider);
    crossoverFrequencySlider.setTextValueSuffix (" Hz");
    crossoverFrequencySlider.setRange(crossoverFrequencySlider.getRange(), 1.f);
    crossoverFrequencySlider.textFromValueFunction = [](double value)
    {
        return juce::String::formatted("%.0f", value);
    };
    crossoverFrequencySlider.updateText();
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::thumbColourId, blue);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, blue);
    crossoverFrequencySlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
}

CrossfeedAudioProcessorEditor::~CrossfeedAudioProcessorEditor()
{
}

//==============================================================================
void CrossfeedAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Crossfeed", 40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Amplitude (low)", 40,  90, 100, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Amplitude (high)", 40,  130, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Delay (low)", 40,  170, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Delay (high)", 40,  210, 100, 30, juce::Justification::left, 1);
    
    g.setColour(blue);
    g.setFont(15);
    g.drawFittedText("Crossover", 40,  250, 100, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.setFont(12);
    g.drawFittedText("Unusual Audio", 40, getHeight() - 60, 300, 30, juce::Justification::left, 1);
}

void CrossfeedAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    amplitudeLowSlider      .setBounds (140,  95, getWidth() - 140 - 40, 20);
    amplitudeHighSlider     .setBounds (140, 135, getWidth() - 140 - 40, 20);
    delayLowSlider          .setBounds (140, 175, getWidth() - 140 - 40, 20);
    delayHighSlider         .setBounds (140, 215, getWidth() - 140 - 40, 20);
    crossoverFrequencySlider.setBounds (140, 255, getWidth() - 140 - 40, 20);
}
