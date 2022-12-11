/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CrossfeedAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CrossfeedAudioProcessorEditor (CrossfeedAudioProcessor&);
    ~CrossfeedAudioProcessorEditor() override;
    
    const juce::Colour black = juce::Colour(0xff000000);
    const juce::Colour white = juce::Colour(0xffffffff);
    const juce::Colour green = juce::Colour(0xff29c732);
    const juce::Colour red = juce::Colour(0xfffc2125);
    const juce::Colour yellow = juce::Colour(0xfffec309);
    const juce::Colour grey = juce::Colour(0xff575757);
    const juce::Colour blue = juce::Colour(0xff5752eb);

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CrossfeedAudioProcessor& audioProcessor;
    
    juce::Slider amplitudeLowSlider;
    juce::SliderParameterAttachment amplitudeLowAttachment;
    
    juce::Slider amplitudeHighSlider;
    juce::SliderParameterAttachment amplitudeHighAttachment;
    
    juce::Slider delayLowSlider;
    juce::SliderParameterAttachment delayLowAttachment;
    
    juce::Slider delayHighSlider;
    juce::SliderParameterAttachment delayHighAttachment;
    
    juce::Slider crossoverFrequencySlider;
    juce::SliderParameterAttachment crossoverFrequencyAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrossfeedAudioProcessorEditor)
};
