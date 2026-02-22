#pragma once

#include <JuceHeader.h>

namespace LayoutConstants
{
constexpr int kUIW = 1203;
constexpr int kUIH = 632;

const juce::Rectangle<int> kPowerToggleBounds { 72, 37, 88, 88 };
const juce::Rectangle<int> kPowerLedBounds { 68, 228, 96, 96 };
const juce::Rectangle<int> kFuseButtonBounds { 82, 437, 68, 68 };

const juce::Rectangle<int> kVUMeterLeftBounds { 194, 30, 300, 240 };
const juce::Rectangle<int> kVUMeterRightBounds { 194, 306, 300, 240 };

const juce::Rectangle<int> kMeterSwitchLeftBounds { 532, 123, 110, 110 };
const juce::Rectangle<int> kMeterSwitchRightBounds { 532, 399, 110, 110 };

const juce::Rectangle<int> kInputGainLeftBounds { 682, 128, 94, 94 };
const juce::Rectangle<int> kInputGainRightBounds { 682, 404, 94, 94 };

const juce::Rectangle<int> kThresholdLeftBounds { 824, 128, 94, 94 };
const juce::Rectangle<int> kThresholdRightBounds { 824, 404, 94, 94 };

const juce::Rectangle<int> kTimeConstantLeftBounds { 966, 128, 94, 94 };
const juce::Rectangle<int> kTimeConstantRightBounds { 966, 398, 94, 94 };

const juce::Rectangle<int> kAgcToggleBounds { 1027, 264, 102, 102 };
}
