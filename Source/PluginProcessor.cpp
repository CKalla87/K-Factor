/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SteppedSlider.h"
#include <cmath>
#include <utility>

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
    inputGainLeftParam = apvts.getRawParameterValue("INPUT_GAIN_L");
    inputGainRightParam = apvts.getRawParameterValue("INPUT_GAIN_R");
    thresholdLeftParam = apvts.getRawParameterValue("THRESHOLD_L");
    thresholdRightParam = apvts.getRawParameterValue("THRESHOLD_R");
    timeConstantLeftParam = apvts.getRawParameterValue("TIME_CONSTANT_L");
    timeConstantRightParam = apvts.getRawParameterValue("TIME_CONSTANT_R");
    meterModeLeftParam = apvts.getRawParameterValue("METER_MODE_L");
    meterModeRightParam = apvts.getRawParameterValue("METER_MODE_R");
    agcModeParam = apvts.getRawParameterValue("AGC_MODE");
    powerParam = apvts.getRawParameterValue("POWER");
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

    for (auto& state : channelStates)
    {
        state.envelope = 0.0f;
        state.releaseFast = 0.0f;
        state.releaseSlow = 0.0f;
        state.lastGainReductionDb = 0.0f;
    }

    meterInputLeft.store (0.0f);
    meterInputRight.store (0.0f);
    meterOutputLeft.store (0.0f);
    meterOutputRight.store (0.0f);
    meterGainReductionLeft.store (0.0f);
    meterGainReductionRight.store (0.0f);
}

void KFactorAudioProcessor::releaseResources()
{
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

    const bool isPowered = powerParam->load() > 0.5f;
    if (! isPowered)
    {
        meterInputLeft.store (0.0f);
        meterInputRight.store (0.0f);
        meterOutputLeft.store (0.0f);
        meterOutputRight.store (0.0f);
        meterGainReductionLeft.store (0.0f);
        meterGainReductionRight.store (0.0f);
        return;
    }

    const auto agcMode = agcModeParam->load() > 0.5f ? AgcMode::latVert : AgcMode::leftRight;

    const float inputGainLeftDb = inputGainLeftParam->load();
    const float inputGainRightDb = inputGainRightParam->load();
    const float thresholdLeftIndex = thresholdLeftParam->load();
    const float thresholdRightIndex = thresholdRightParam->load();
    const int timeConstantLeft = static_cast<int> (timeConstantLeftParam->load());
    const int timeConstantRight = static_cast<int> (timeConstantRightParam->load());

    const float inputGainLeft = juce::Decibels::decibelsToGain (inputGainLeftDb);
    const float inputGainRight = juce::Decibels::decibelsToGain (inputGainRightDb);

    auto computeThresholdDb = [] (float indexValue)
    {
        // Fairchild threshold control (1-6) mapped to a gentle dB range.
        const float clamped = juce::jlimit (1.0f, 6.0f, indexValue);
        return -2.0f - (clamped - 1.0f) * 4.0f;
    };

    const float thresholdLeftDb = computeThresholdDb (thresholdLeftIndex);
    const float thresholdRightDb = computeThresholdDb (thresholdRightIndex);

    const int numSamples = buffer.getNumSamples();

    float inputEnergyLeft = 0.0f;
    float inputEnergyRight = 0.0f;
    float outputEnergyLeft = 0.0f;
    float outputEnergyRight = 0.0f;
    float gainReductionSumLeft = 0.0f;
    float gainReductionSumRight = 0.0f;

    auto processSample = [this] (float sample, ChannelState& state, float thresholdDb, int timeConstant) -> std::pair<float, float>
    {
        const float detector = std::abs (sample);

        const float attackMs = getTimeConstantAttackMs (timeConstant);
        const auto release = getReleaseCoefficients (timeConstant, state.lastGainReductionDb);

        const float attackCoeff = std::exp (-1.0f / (0.001f * attackMs * static_cast<float> (currentSampleRate)));

        if (detector > state.envelope)
        {
            state.envelope = attackCoeff * state.envelope + (1.0f - attackCoeff) * detector;
            state.releaseFast = state.envelope;
            state.releaseSlow = state.envelope;
        }
        else
        {
            if (release.dualStage)
            {
                state.releaseFast = release.fastCoeff * state.releaseFast + (1.0f - release.fastCoeff) * detector;
                state.releaseSlow = release.slowCoeff * state.releaseSlow + (1.0f - release.slowCoeff) * detector;
                state.envelope = state.releaseFast * (1.0f - release.mix) + state.releaseSlow * release.mix;
            }
            else
            {
                state.envelope = release.fastCoeff * state.envelope + (1.0f - release.fastCoeff) * detector;
            }
        }

        const float levelDb = juce::Decibels::gainToDecibels (state.envelope + 1.0e-6f);
        const float gainReductionDb = computeGainReductionDb (levelDb, thresholdDb);
        state.lastGainReductionDb = gainReductionDb;

        const float targetGain = juce::Decibels::decibelsToGain (-gainReductionDb);
        float processed = sample * targetGain;

        // Gentle vari-mu style saturation
        const float drive = 1.4f;
        const float bias = 0.015f;
        processed = std::tanh ((processed + bias) * drive) - std::tanh (bias * drive);
        return { processed, gainReductionDb };
    };

    if (agcMode == AgcMode::leftRight)
    {
        auto* left = buffer.getWritePointer (0);
        auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            float leftSample = left[i] * inputGainLeft;
            float rightSample = right != nullptr ? right[i] * inputGainRight : leftSample;

            inputEnergyLeft += leftSample * leftSample;
            inputEnergyRight += rightSample * rightSample;

            auto [processedLeft, grLeft] = processSample (leftSample, channelStates[0], thresholdLeftDb, timeConstantLeft);
            auto [processedRight, grRight] = processSample (rightSample, channelStates[1], thresholdRightDb, timeConstantRight);

            left[i] = processedLeft;
            if (right != nullptr)
                right[i] = processedRight;

            outputEnergyLeft += processedLeft * processedLeft;
            outputEnergyRight += processedRight * processedRight;
            gainReductionSumLeft += grLeft;
            gainReductionSumRight += grRight;
        }
    }
    else
    {
        auto* left = buffer.getWritePointer (0);
        auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            const float leftSample = left[i] * inputGainLeft;
            const float rightSample = right != nullptr ? right[i] * inputGainRight : leftSample;

            const float mid = (leftSample + rightSample) * 0.7071067f;
            const float side = (leftSample - rightSample) * 0.7071067f;

            inputEnergyLeft += mid * mid;
            inputEnergyRight += side * side;

            auto [processedMid, grMid] = processSample (mid, channelStates[0], thresholdLeftDb, timeConstantLeft);
            auto [processedSide, grSide] = processSample (side, channelStates[1], thresholdRightDb, timeConstantRight);

            const float outLeft = (processedMid + processedSide) * 0.7071067f;
            const float outRight = (processedMid - processedSide) * 0.7071067f;

            left[i] = outLeft;
            if (right != nullptr)
                right[i] = outRight;

            outputEnergyLeft += processedMid * processedMid;
            outputEnergyRight += processedSide * processedSide;
            gainReductionSumLeft += grMid;
            gainReductionSumRight += grSide;
        }
    }

    const float rmsInputLeft = std::sqrt (inputEnergyLeft / static_cast<float> (numSamples));
    const float rmsInputRight = std::sqrt (inputEnergyRight / static_cast<float> (numSamples));
    const float rmsOutputLeft = std::sqrt (outputEnergyLeft / static_cast<float> (numSamples));
    const float rmsOutputRight = std::sqrt (outputEnergyRight / static_cast<float> (numSamples));

    meterInputLeft.store (juce::Decibels::gainToDecibels (rmsInputLeft + 1.0e-6f));
    meterInputRight.store (juce::Decibels::gainToDecibels (rmsInputRight + 1.0e-6f));
    meterOutputLeft.store (juce::Decibels::gainToDecibels (rmsOutputLeft + 1.0e-6f));
    meterOutputRight.store (juce::Decibels::gainToDecibels (rmsOutputRight + 1.0e-6f));
    meterGainReductionLeft.store (gainReductionSumLeft / static_cast<float> (numSamples));
    meterGainReductionRight.store (gainReductionSumRight / static_cast<float> (numSamples));
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
float KFactorAudioProcessor::getTimeConstantAttackMs (int timeConstant) const
{
    switch (timeConstant)
    {
        case 1: return 0.2f;
        case 2: return 0.2f;
        case 3: return 0.4f;
        case 4: return 0.4f;
        case 5: return 0.2f;
        case 6: return 0.2f;
        default: return 0.2f;
    }
}

float KFactorAudioProcessor::getTimeConstantReleaseMs (int timeConstant, float gainReductionDb) const
{
    const float reduction = juce::jlimit (0.0f, 20.0f, gainReductionDb);

    switch (timeConstant)
    {
        case 1: return 300.0f;
        case 2: return 800.0f;
        case 3: return 2000.0f;
        case 4: return 5000.0f;
        case 5: return 2000.0f;
        case 6: return 5000.0f;
        default: return 800.0f;
    }
}

KFactorAudioProcessor::ReleaseCoefficients KFactorAudioProcessor::getReleaseCoefficients (int timeConstant, float gainReductionDb) const
{
    const float reduction = juce::jlimit (0.0f, 20.0f, gainReductionDb);

    if (timeConstant == 5 || timeConstant == 6)
    {
        const float fastMs = timeConstant == 5 ? 2000.0f : 5000.0f;
        const float baseSlowMs = timeConstant == 5 ? 10000.0f : 25000.0f;
        const float extraSlowMs = timeConstant == 5 ? 10000.0f : 15000.0f;
        const float slowMs = baseSlowMs + (reduction / 20.0f) * extraSlowMs;

        const float fastCoeff = std::exp (-1.0f / (0.001f * fastMs * static_cast<float> (currentSampleRate)));
        const float slowCoeff = std::exp (-1.0f / (0.001f * slowMs * static_cast<float> (currentSampleRate)));

        return { fastCoeff, slowCoeff, 0.5f, true };
    }

    const float releaseMs = getTimeConstantReleaseMs (timeConstant, gainReductionDb);
    const float coeff = std::exp (-1.0f / (0.001f * releaseMs * static_cast<float> (currentSampleRate)));
    return { coeff, coeff, 1.0f, false };
}

float KFactorAudioProcessor::computeGainReductionDb (float levelDb, float thresholdDb) const
{
    const float over = levelDb - thresholdDb;
    if (over <= -3.0f)
        return 0.0f;

    // Vari-mu style soft-knee compression curve
    const float kneeDb = 6.0f;
    const float effectiveOver = juce::jmax (0.0f, over);
    const float kneeStart = -kneeDb * 0.5f;
    const float kneeEnd = kneeDb * 0.5f;

    float kneeBlend = 1.0f;
    if (over < kneeEnd)
    {
        const float t = juce::jlimit (0.0f, 1.0f, (over - kneeStart) / (kneeEnd - kneeStart));
        kneeBlend = t * t * (3.0f - 2.0f * t);
    }

    const float ratio = 1.5f + 6.0f * (1.0f - std::exp (-effectiveOver / 12.0f));
    const float rawReduction = effectiveOver * (1.0f - 1.0f / ratio);
    return rawReduction * kneeBlend;
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout KFactorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    const std::vector<float> inputGainSteps { 0.0f, 0.6f, 1.2f, 2.0f, 3.0f, 4.0f, 6.0f, 8.0f,
                                              10.0f, 15.0f, 20.0f, 30.0f };
    const auto inputGainRange = makeSteppedNormalisableRange (inputGainSteps);

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("INPUT_GAIN_L", 1), "Left Input Gain",
        inputGainRange,
        0.0f, "dB"
    ));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID ("INPUT_GAIN_R", 1), "Right Input Gain",
        inputGainRange,
        0.0f, "dB"
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("THRESHOLD_L", 1), "Left Threshold", 1, 6, 3
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("THRESHOLD_R", 1), "Right Threshold", 1, 6, 3
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("TIME_CONSTANT_L", 1), "Left Time Constant", 1, 6, 2
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("TIME_CONSTANT_R", 1), "Right Time Constant", 1, 6, 2
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("METER_MODE_L", 1), "Left Meter Mode", 0, 2, 1
    ));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID ("METER_MODE_R", 1), "Right Meter Mode", 0, 2, 1
    ));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("AGC_MODE", 1), "AGC Mode", false
    ));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID ("POWER", 1), "Power", true
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KFactorAudioProcessor();
}


