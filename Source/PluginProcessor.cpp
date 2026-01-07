/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
KFactorAudioProcessor::KFactorAudioProcessor()
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
    // Get parameter pointers
    thresholdParam = apvts.getRawParameterValue("THRESHOLD");
    ratioParam = apvts.getRawParameterValue("RATIO");
    attackParam = apvts.getRawParameterValue("ATTACK");
    releaseParam = apvts.getRawParameterValue("RELEASE");
    makeupGainParam = apvts.getRawParameterValue("MAKEUP");
}

KFactorAudioProcessor::~KFactorAudioProcessor()
{
}

//==============================================================================
const juce::String KFactorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KFactorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KFactorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KFactorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KFactorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KFactorAudioProcessor::getNumPrograms()
{
    return 1;
}

int KFactorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KFactorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KFactorAudioProcessor::getProgramName (int index)
{
    return {};
}

void KFactorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KFactorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    
    compressor.prepare (spec);
    makeupGain.prepare (spec);
    
    updateParameters();
}

void KFactorAudioProcessor::releaseResources()
{
    compressor.reset();
    makeupGain.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KFactorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void KFactorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update parameters less frequently to reduce CPU (every N blocks)
    // Always update on first block to ensure initialization
    parameterUpdateCounter++;
    if (parameterUpdateCounter >= parameterUpdateInterval || parameterUpdateCounter == 1)
    {
    updateParameters();
        if (parameterUpdateCounter >= parameterUpdateInterval)
            parameterUpdateCounter = 0;
    }

    // Process audio
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    
    // Process compressor
    compressor.process (context);
    
    // Apply makeup gain
    makeupGain.process (context);
    
    // Note: Gain reduction calculation removed to reduce CPU usage
    // Can be added back if needed for UI meter display
}

//==============================================================================
bool KFactorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* KFactorAudioProcessor::createEditor()
{
    return new KFactorAudioProcessorEditor (*this);
}

//==============================================================================
void KFactorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void KFactorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
void KFactorAudioProcessor::updateParameters()
{
    // Only update if parameters have changed (optimization to reduce CPU)
    // Use tolerance for float comparison to avoid constant updates
    const float tolerance = 0.001f;
    
    float threshold = thresholdParam->load();
    float ratio = ratioParam->load();
    float attack = attackParam->load();
    float release = releaseParam->load();
    float makeup = makeupGainParam->load();
    
    if (std::abs (threshold - cachedThreshold) > tolerance)
    {
        compressor.setThreshold (threshold);
        cachedThreshold = threshold;
    }
    
    if (std::abs (ratio - cachedRatio) > tolerance)
    {
        compressor.setRatio (ratio);
        cachedRatio = ratio;
    }
    
    if (std::abs (attack - cachedAttack) > tolerance)
    {
        compressor.setAttack (attack);
        cachedAttack = attack;
    }
    
    if (std::abs (release - cachedRelease) > tolerance)
    {
        compressor.setRelease (release);
        cachedRelease = release;
    }
    
    if (std::abs (makeup - cachedMakeupGain) > tolerance)
    {
        makeupGain.setGainDecibels (makeup);
        cachedMakeupGain = makeup;
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout KFactorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Threshold: -60dB to 0dB, default -12dB
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("THRESHOLD", 1), "Threshold",
        juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f),
        -12.0f, "dB"
    ));

    // Ratio: 1:1 to 20:1, default 4:1 (1176-style)
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("RATIO", 1), "Ratio",
        juce::NormalisableRange<float> (1.0f, 20.0f, 0.1f),
        4.0f, ":1"
    ));

    // Attack: 0.01ms to 100ms, default 0.8ms (1176-style fast attack)
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("ATTACK", 1), "Attack",
        juce::NormalisableRange<float> (0.01f, 100.0f, 0.01f, 0.3f),
        0.8f, "ms"
    ));

    // Release: 1ms to 2000ms, default 100ms
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("RELEASE", 1), "Release",
        juce::NormalisableRange<float> (1.0f, 2000.0f, 0.1f, 0.3f),
        100.0f, "ms"
    ));

    // Makeup Gain: 0dB to 24dB, default 0dB
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("MAKEUP", 1), "Makeup Gain",
        juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f),
        0.0f, "dB"
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KFactorAudioProcessor();
}


