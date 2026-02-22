#pragma once

#include <JuceHeader.h>

namespace KFactorAssets
{
const void* getKnobPngData();
size_t getKnobPngSize();
const void* getKnob2PngData();
size_t getKnob2PngSize();
const void* getVuCoverPngData();
size_t getVuCoverPngSize();
const void* getBackgroundPngData();
size_t getBackgroundPngSize();
const void* getToggleSpritePngData();
size_t getToggleSpritePngSize();
const void* getVuFacePngData();
size_t getVuFacePngSize();
const void* getPowerButtonPngData();
size_t getPowerButtonPngSize();
const void* getFuseButtonPngData();
size_t getFuseButtonPngSize();
const void* getPowerToggleBasePngData();
size_t getPowerToggleBasePngSize();
const void* getPowerToggleTipPngData();
size_t getPowerToggleTipPngSize();

juce::Image getKnobImage();
juce::Image getKnob2Image();
juce::Image getVuCoverImage();
juce::Image getBackgroundImage();
juce::Image getToggleSpriteImage();
juce::Image getVuFaceImage();
juce::Image getPowerButtonImage();
juce::Image getFuseButtonImage();
juce::Image getPowerToggleBaseImage();
juce::Image getPowerToggleTipImage();
}
