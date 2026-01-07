/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KFactorAudioProcessorEditor::KFactorAudioProcessorEditor (KFactorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (700, 550);  // Slightly taller to accommodate logo

    // Threshold slider
    thresholdSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    thresholdSlider.setPopupDisplayEnabled (true, false, this);
    thresholdSlider.setTextValueSuffix (" dB");
    thresholdSlider.setColour (juce::Slider::rotarySliderFillColourId, glowRed);
    thresholdSlider.setColour (juce::Slider::rotarySliderOutlineColourId, metallicDark);
    thresholdSlider.setColour (juce::Slider::thumbColourId, glowRedBright);
    thresholdSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    thresholdSlider.setColour (juce::Slider::textBoxBackgroundColourId, darkBlack);
    addAndMakeVisible (&thresholdSlider);
    
    thresholdLabel.setText ("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType (juce::Justification::centred);
    thresholdLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&thresholdLabel);

    // Ratio slider
    ratioSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    ratioSlider.setPopupDisplayEnabled (true, false, this);
    ratioSlider.setTextValueSuffix (" :1");
    ratioSlider.setColour (juce::Slider::rotarySliderFillColourId, glowRed);
    ratioSlider.setColour (juce::Slider::rotarySliderOutlineColourId, metallicDark);
    ratioSlider.setColour (juce::Slider::thumbColourId, glowRedBright);
    ratioSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    ratioSlider.setColour (juce::Slider::textBoxBackgroundColourId, darkBlack);
    addAndMakeVisible (&ratioSlider);
    
    ratioLabel.setText ("Ratio", juce::dontSendNotification);
    ratioLabel.setJustificationType (juce::Justification::centred);
    ratioLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&ratioLabel);

    // Attack slider (fast 1176-style)
    attackSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    attackSlider.setPopupDisplayEnabled (true, false, this);
    attackSlider.setTextValueSuffix (" ms");
    attackSlider.setColour (juce::Slider::rotarySliderFillColourId, glowRed);
    attackSlider.setColour (juce::Slider::rotarySliderOutlineColourId, metallicDark);
    attackSlider.setColour (juce::Slider::thumbColourId, glowRedBright);
    attackSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    attackSlider.setColour (juce::Slider::textBoxBackgroundColourId, darkBlack);
    addAndMakeVisible (&attackSlider);
    
    attackLabel.setText ("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType (juce::Justification::centred);
    attackLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&attackLabel);

    // Release slider
    releaseSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    releaseSlider.setPopupDisplayEnabled (true, false, this);
    releaseSlider.setTextValueSuffix (" ms");
    releaseSlider.setColour (juce::Slider::rotarySliderFillColourId, glowRed);
    releaseSlider.setColour (juce::Slider::rotarySliderOutlineColourId, metallicDark);
    releaseSlider.setColour (juce::Slider::thumbColourId, glowRedBright);
    releaseSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    releaseSlider.setColour (juce::Slider::textBoxBackgroundColourId, darkBlack);
    addAndMakeVisible (&releaseSlider);
    
    releaseLabel.setText ("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType (juce::Justification::centred);
    releaseLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&releaseLabel);

    // Makeup Gain slider
    makeupSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    makeupSlider.setPopupDisplayEnabled (true, false, this);
    makeupSlider.setTextValueSuffix (" dB");
    makeupSlider.setColour (juce::Slider::rotarySliderFillColourId, glowRed);
    makeupSlider.setColour (juce::Slider::rotarySliderOutlineColourId, metallicDark);
    makeupSlider.setColour (juce::Slider::thumbColourId, glowRedBright);
    makeupSlider.setColour (juce::Slider::textBoxTextColourId, textColor);
    makeupSlider.setColour (juce::Slider::textBoxBackgroundColourId, darkBlack);
    addAndMakeVisible (&makeupSlider);
    
    makeupLabel.setText ("Makeup", juce::dontSendNotification);
    makeupLabel.setJustificationType (juce::Justification::centred);
    makeupLabel.setColour (juce::Label::textColourId, textColor);
    addAndMakeVisible (&makeupLabel);

    // Attach sliders to parameters
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "THRESHOLD", thresholdSlider);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "RATIO", ratioSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "ATTACK", attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "RELEASE", releaseSlider);
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "MAKEUP", makeupSlider);
}

KFactorAudioProcessorEditor::~KFactorAudioProcessorEditor()
{
}

//==============================================================================
void KFactorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Textured dark background matching the logo (fine-grained leather/rough matte metal)
    g.fillAll (darkBlack);
    
    // Add subtle texture pattern to simulate fine-grained leather/matte metal
    juce::Random r;
    g.setColour (juce::Colour::fromFloatRGBA (0.06f, 0.06f, 0.08f, 1.0f));
    for (int i = 0; i < 800; ++i)
    {
        float x = r.nextFloat() * getWidth();
        float y = r.nextFloat() * getHeight();
        float size = 0.5f + r.nextFloat() * 1.5f;
        g.fillEllipse (x, y, size, size);
    }
    
    // Additional darker texture overlay
    g.setColour (juce::Colour::fromFloatRGBA (0.04f, 0.04f, 0.06f, 0.5f));
    for (int i = 0; i < 300; ++i)
    {
        float x = r.nextFloat() * getWidth();
        float y = r.nextFloat() * getHeight();
        g.fillRect (x, y, 1.0f, 1.0f);
    }

    const float centerX = getWidth() / 2.0f;
    const float logoY = 90.0f;  // Moved down
    const float logoSize = 80.0f;
    const float circleRadius = logoSize / 2.0f + 8.0f;

    // Draw red glow behind logo area
    g.setColour (glowRed.withAlpha (0.3f));
    for (int glow = 0; glow < 3; ++glow)
    {
        float glowRadius = circleRadius + (glow * 8.0f);
        g.fillEllipse (centerX - glowRadius, logoY - glowRadius, 
                      glowRadius * 2.0f, glowRadius * 2.0f);
    }

    // Draw metallic circular frame
    juce::Path circleFrame;
    circleFrame.addEllipse (centerX - circleRadius, logoY - circleRadius,
                           circleRadius * 2.0f, circleRadius * 2.0f);
    
    // Outer ring - metallic dark
    g.setColour (metallicDark);
    g.strokePath (circleFrame, juce::PathStrokeType (6.0f));
    
    // Inner highlight - metallic silver
    g.setColour (metallicSilver.withAlpha (0.6f));
    g.strokePath (circleFrame, juce::PathStrokeType (3.0f));

    // Draw stylized metallic "K" in the circle - angular and sharp
    juce::Path kPath;
    float kSize = logoSize * 0.75f;
    float kX = centerX - kSize / 2.0f;
    float kY = logoY - kSize / 2.0f;
    float barWidth = kSize * 0.22f;
    float midX = kX + kSize * 0.32f;
    float midY = kY + kSize * 0.5f;
    
    // Vertical bar (left side of K)
    kPath.addRectangle (kX, kY, barWidth, kSize);
    
    // Top diagonal leg - angular and sharp
    kPath.startNewSubPath (midX, midY);
    kPath.lineTo (kX + kSize, kY);
    kPath.lineTo (kX + kSize - barWidth * 0.6f, kY + barWidth * 0.8f);
    kPath.lineTo (midX + barWidth * 0.4f, midY - barWidth * 0.4f);
    kPath.closeSubPath();
    
    // Bottom diagonal leg - angular and sharp
    kPath.startNewSubPath (midX, midY);
    kPath.lineTo (kX + kSize, kY + kSize);
    kPath.lineTo (kX + kSize - barWidth * 0.6f, kY + kSize - barWidth * 0.8f);
    kPath.lineTo (midX + barWidth * 0.4f, midY + barWidth * 0.4f);
    kPath.closeSubPath();

    // Red glow outline for K
    g.setColour (glowRed.withAlpha (0.6f));
    g.strokePath (kPath, juce::PathStrokeType (3.0f));
    
    // Metallic fill for K with gradient
    juce::ColourGradient kGradient (metallicSilver, kX, kY,
                                   metallicDark, kX + kSize, kY + kSize,
                                   false);
    g.setGradientFill (kGradient);
    g.fillPath (kPath);
    
    // Highlight on K
    g.setColour (metallicSilver.brighter (0.3f));
    g.strokePath (kPath, juce::PathStrokeType (1.5f));

    // Draw red sparks/embers around the logo
    g.setColour (glowRed.withAlpha (0.8f));
    for (int i = 0; i < 15; ++i)
    {
        float angle = r.nextFloat() * juce::MathConstants<float>::twoPi;
        float distance = circleRadius + 10.0f + r.nextFloat() * 20.0f;
        float sparkX = centerX + std::cos (angle) * distance;
        float sparkY = logoY + std::sin (angle) * distance;
        float sparkSize = 2.0f + r.nextFloat() * 3.0f;
        g.fillEllipse (sparkX - sparkSize/2, sparkY - sparkSize/2, sparkSize, sparkSize);
    }

    // Draw "K-FACTOR" text with metallic 3D effect and red glow
    g.setFont (juce::Font (48.0f, juce::Font::bold));
    
    // Red glow behind text (multiple passes)
    g.setColour (glowRed.withAlpha (0.5f));
    for (int i = -3; i <= 3; ++i)
    {
        for (int j = -3; j <= 3; ++j)
        {
            if (i != 0 || j != 0)
            {
                g.drawText ("K-FACTOR", centerX - 140 + i, logoY + circleRadius + 25 + j, 
                          280, 60, juce::Justification::centred, false);
            }
        }
    }
    
    // Shadow layer for 3D effect
    g.setColour (juce::Colour::fromFloatRGBA (0.0f, 0.0f, 0.0f, 0.8f));
    g.drawText ("K-FACTOR", centerX - 140 + 2, logoY + circleRadius + 25 + 2, 
              280, 60, juce::Justification::centred, false);
    
    // Main metallic text
    juce::ColourGradient textGradient (metallicSilver.brighter(), centerX - 140, logoY + circleRadius + 25,
                                      metallicDark, centerX - 140, logoY + circleRadius + 85,
                                      false);
    g.setGradientFill (textGradient);
    g.drawText ("K-FACTOR", centerX - 140, logoY + circleRadius + 25, 
              280, 60, juce::Justification::centred, false);
    
    // Highlight on text
    g.setColour (metallicSilver.brighter (0.4f));
    g.drawText ("K-FACTOR", centerX - 140, logoY + circleRadius + 25, 
              280, 60, juce::Justification::centred, false);

    // Red glow line under text
    g.setColour (glowRed.withAlpha (0.6f));
    g.drawLine (centerX - 100, logoY + circleRadius + 90, 
               centerX + 100, logoY + circleRadius + 90, 2.0f);
}

void KFactorAudioProcessorEditor::resized()
{
    const int knobSize = 100;
    const int labelHeight = 25;
    const int startY = 280;  // More space below logo to avoid crowding
    const int spacing = (getWidth() - 100 - (knobSize * 5)) / 4;
    
    int x = 50;
    
    // Threshold
    thresholdSlider.setBounds (x, startY, knobSize, knobSize);
    thresholdLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Ratio
    ratioSlider.setBounds (x, startY, knobSize, knobSize);
    ratioLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Attack
    attackSlider.setBounds (x, startY, knobSize, knobSize);
    attackLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Release
    releaseSlider.setBounds (x, startY, knobSize, knobSize);
    releaseLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
    x += knobSize + spacing;
    
    // Makeup
    makeupSlider.setBounds (x, startY, knobSize, knobSize);
    makeupLabel.setBounds (x, startY + knobSize + 5, knobSize, labelHeight);
}

