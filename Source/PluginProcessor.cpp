/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CrossfeedAudioProcessor::CrossfeedAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (amplitudeLow = new juce::AudioParameterFloat ({"amplitudeLow", 1}, "Amplitude (low)", 0.f, 1.f, .75f));
    addParameter (amplitudeHigh = new juce::AudioParameterFloat ({"amplitudeHigh", 1}, "Amplitude (high)", 0.f, 1.f, .1f));
    addParameter (delayLow = new juce::AudioParameterFloat ({"delayLow", 1}, "Delay (low)", 0.f, 1000.f, 250.f));
    addParameter (delayHigh = new juce::AudioParameterFloat ({"delayHigh", 1}, "Delay (high)", 0.f, 1000.f, 100.f));
    addParameter (crossoverFrequency = new juce::AudioParameterFloat ({"crossoverFrequency", 1}, "Crossover frequency", 20.f, 20000.f, 2000.f));
}

CrossfeedAudioProcessor::~CrossfeedAudioProcessor()
{
}

//==============================================================================
const juce::String CrossfeedAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CrossfeedAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CrossfeedAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CrossfeedAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CrossfeedAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CrossfeedAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CrossfeedAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CrossfeedAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CrossfeedAudioProcessor::getProgramName (int index)
{
    return {};
}

void CrossfeedAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CrossfeedAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    
    this->sampleRate = sampleRate;
    
    delayLine1 = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>{static_cast<int>(std::ceil(sampleRate * (1000.f / 1000.f)))};
    delayLine1.reset();
    delayLine1.prepare(spec);
    
    delayLine2 = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>{static_cast<int>(std::ceil(sampleRate * (1000.f / 1000.f)))};
    delayLine2.reset();
    delayLine2.prepare(spec);
    
    lowPassFilter.reset();
    lowPassFilter.prepare(spec);
    
    highPassFilter.reset();
    highPassFilter.prepare(spec);
}

void CrossfeedAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CrossfeedAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CrossfeedAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    double t = *delayLow / 1000.f / 1000.f;
    delayLine1.setDelay(this->sampleRate * t);

    t = *delayHigh / 1000.f / 1000.f;
    delayLine2.setDelay(this->sampleRate * t);
    
    lowPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, *crossoverFrequency);
    
    highPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, *crossoverFrequency);

    int samples = buffer.getNumSamples();

    juce::AudioBuffer<float> lowBandBuffer;
    lowBandBuffer.makeCopyOf(buffer);
    
    auto ch1 = lowBandBuffer.getWritePointer(0);
    auto ch2 = lowBandBuffer.getWritePointer(1);
    for (int i = 0; i < samples; i++)
    {
        ch1[i] = lowPassFilter.processSample(ch1[i]);
        ch2[i] = lowPassFilter.processSample(ch2[i]);
    }
    
    juce::AudioBuffer<float> highBandBuffer;
    highBandBuffer.makeCopyOf(buffer);
    
    ch1 = highBandBuffer.getWritePointer(0);
    ch2 = highBandBuffer.getWritePointer(1);
    for (int i = 0; i < samples; i++)
    {
        ch1[i] = highPassFilter.processSample(ch1[i]);
        ch2[i] = highPassFilter.processSample(ch2[i]);
    }

    float a = *amplitudeLow;
    
    ch1 = lowBandBuffer.getWritePointer(0);
    ch2 = lowBandBuffer.getWritePointer(1);
    for (int i = 0; i < samples; i++)
    {
        delayLine1.pushSample(0, ch1[i]);
        delayLine1.pushSample(1, ch2[i]);
        ch1[i] = ch1[i] * (1.f - a) + delayLine1.popSample(1) * a;
        ch2[i] = ch2[i] * (1.f - a) + delayLine1.popSample(0) * a;
    }
    
    a = *amplitudeHigh;

    ch1 = highBandBuffer.getWritePointer(0);
    ch2 = highBandBuffer.getWritePointer(1);
    for (int i = 0; i < samples; i++)
    {
        delayLine2.pushSample(0, ch1[i]);
        delayLine2.pushSample(1, ch2[i]);
        ch1[i] = ch1[i] * (1.f - a) + delayLine2.popSample(1) * a;
        ch2[i] = ch2[i] * (1.f - a) + delayLine2.popSample(0) * a;
    }
    
    buffer.clear();
    buffer.addFrom(0, 0, lowBandBuffer, 0, 0, samples);
    buffer.addFrom(0, 0, highBandBuffer, 0, 0, samples);
    buffer.addFrom(1, 0, lowBandBuffer, 1, 0, samples);
    buffer.addFrom(1, 0, highBandBuffer, 1, 0, samples);
}

//==============================================================================
bool CrossfeedAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CrossfeedAudioProcessor::createEditor()
{
    return new CrossfeedAudioProcessorEditor (*this);
}

//==============================================================================
void CrossfeedAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("CrossfeedParameter"));
    xml->setAttribute ("amplitudeLow", (double) *amplitudeLow);
    xml->setAttribute ("amplitudeHigh", (double) *amplitudeHigh);
    xml->setAttribute ("delayLow", (double) *delayLow);
    xml->setAttribute ("delayHigh", (double) *delayHigh);
    xml->setAttribute ("crossoverFrequency", (double) *crossoverFrequency);
    copyXmlToBinary (*xml, destData);
}

void CrossfeedAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("CrossfeedParameter"))
        {
            *amplitudeLow = (float) xmlState->getDoubleAttribute ("amplitudeLow", *amplitudeLow);
            *amplitudeHigh = (float) xmlState->getDoubleAttribute ("amplitudeHigh", *amplitudeHigh);
            *delayLow = (float) xmlState->getDoubleAttribute ("delayLow", *delayLow);
            *delayHigh = (float) xmlState->getDoubleAttribute ("delayHigh", *delayHigh);
            *crossoverFrequency = (float) xmlState->getDoubleAttribute ("crossoverFrequency", *crossoverFrequency);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CrossfeedAudioProcessor();
}
