/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class KFactorAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    KFactorAudioProcessor();
    ~KFactorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter management
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    // Compressor parameters
    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* ratioParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* makeupGainParam = nullptr;

    // Gain reduction meter value (for UI)
    float getCurrentGainReduction() const { return currentGainReduction.load(); }

private:
    //==============================================================================
    // DSP processing
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Gain<float> makeupGain;
    
    void updateParameters();
    
    double currentSampleRate = 44100.0;
    std::atomic<float> currentGainReduction{0.0f};

    // Cached parameter values to avoid unnecessary updates
    float cachedThreshold = -12.0f;
    float cachedRatio = 4.0f;
    float cachedAttack = 0.8f;
    float cachedRelease = 100.0f;
    float cachedMakeupGain = 0.0f;
    
    // Counter to reduce parameter update frequency
    int parameterUpdateCounter = 0;
    static constexpr int parameterUpdateInterval = 4; // Update every 4 blocks

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KFactorAudioProcessor)
};


