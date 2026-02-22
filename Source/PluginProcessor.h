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

    enum class MeterMode
    {
        input = 0,
        gainReduction = 1,
        output = 2
    };

    enum class AgcMode
    {
        leftRight = 0,
        latVert = 1
    };

    // Fairchild-style control parameters
    std::atomic<float>* inputGainLeftParam = nullptr;
    std::atomic<float>* inputGainRightParam = nullptr;
    std::atomic<float>* thresholdLeftParam = nullptr;
    std::atomic<float>* thresholdRightParam = nullptr;
    std::atomic<float>* timeConstantLeftParam = nullptr;
    std::atomic<float>* timeConstantRightParam = nullptr;
    std::atomic<float>* meterModeLeftParam = nullptr;
    std::atomic<float>* meterModeRightParam = nullptr;
    std::atomic<float>* agcModeParam = nullptr;
    std::atomic<float>* powerParam = nullptr;

    // Metering accessors
    float getMeterInputLeft() const { return meterInputLeft.load(); }
    float getMeterInputRight() const { return meterInputRight.load(); }
    float getMeterOutputLeft() const { return meterOutputLeft.load(); }
    float getMeterOutputRight() const { return meterOutputRight.load(); }
    float getMeterGainReductionLeft() const { return meterGainReductionLeft.load(); }
    float getMeterGainReductionRight() const { return meterGainReductionRight.load(); }

private:
    //==============================================================================
    struct ChannelState
    {
        float envelope = 0.0f;
        float releaseFast = 0.0f;
        float releaseSlow = 0.0f;
        float lastGainReductionDb = 0.0f;
    };

    ChannelState channelStates[2];

    float getTimeConstantAttackMs (int timeConstant) const;
    float getTimeConstantReleaseMs (int timeConstant, float gainReductionDb) const;
    float computeGainReductionDb (float levelDb, float thresholdDb) const;
    struct ReleaseCoefficients
    {
        float fastCoeff = 0.0f;
        float slowCoeff = 0.0f;
        float mix = 1.0f;
        bool dualStage = false;
    };
    ReleaseCoefficients getReleaseCoefficients (int timeConstant, float gainReductionDb) const;

    double currentSampleRate = 44100.0;
    std::atomic<float> meterInputLeft{0.0f};
    std::atomic<float> meterInputRight{0.0f};
    std::atomic<float> meterOutputLeft{0.0f};
    std::atomic<float> meterOutputRight{0.0f};
    std::atomic<float> meterGainReductionLeft{0.0f};
    std::atomic<float> meterGainReductionRight{0.0f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KFactorAudioProcessor)
};


