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
class KFactorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    KFactorAudioProcessorEditor (KFactorAudioProcessor&);
    ~KFactorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    KFactorAudioProcessor& audioProcessor;

    // Metallic dark theme matching the logo - fine-grained leather/matte metal texture
    juce::Colour darkBlack = juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.1f, 1.0f);  // Dark textured background
    juce::Colour metallicSilver = juce::Colour::fromFloatRGBA (0.7f, 0.7f, 0.75f, 1.0f);  // Metallic silver
    juce::Colour metallicDark = juce::Colour::fromFloatRGBA (0.3f, 0.3f, 0.35f, 1.0f);  // Dark metallic
    juce::Colour glowRed = juce::Colour::fromFloatRGBA (1.0f, 0.1f, 0.15f, 1.0f);  // Vibrant red glow
    juce::Colour glowRedBright = juce::Colour::fromFloatRGBA (1.0f, 0.2f, 0.25f, 1.0f);  // Bright red
    juce::Colour textColor = metallicSilver;

    // Controls
    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider makeupSlider;
    
    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label makeupLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KFactorAudioProcessorEditor)
};

