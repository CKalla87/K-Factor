#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Assets.h"
#include "LayoutConstants.h"
#include "SteppedSlider.h"

class KFactorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    KFactorAudioProcessorEditor (KFactorAudioProcessor&);
    ~KFactorAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct ImageKnobLookAndFeel : public juce::LookAndFeel_V4
    {
        explicit ImageKnobLookAndFeel (juce::Image imageToUse, float pointerAngleOffsetRadians = 0.0f);
        void drawRotarySlider (juce::Graphics&, int, int, int, int, float, float, float,
                               juce::Slider&) override;

    private:
        juce::Image knobImage;
        float pointerAngleOffsetRadians = 0.0f;
    };

    class MeterSwitchSlider : public SteppedSlider
    {
    public:
        void setDrawDetentMarks (bool shouldDraw) noexcept { drawDetentMarks = shouldDraw; }
        bool getDrawDetentMarks() const noexcept { return drawDetentMarks; }

        void setDetentAngles (std::vector<float> anglesRadians)
        {
            detentAngles = std::move (anglesRadians);
        }

        const std::vector<float>& getDetentAngles() const noexcept
        {
            return detentAngles;
        }

        bool getAngleForValue (double value, float& angleOut) const
        {
            const auto& values = getAllowedValues();
            if (values.empty() || detentAngles.size() != values.size())
                return false;

            int closestIndex = 0;
            double bestDistance = std::abs (value - values.front());

            for (int i = 1; i < static_cast<int> (values.size()); ++i)
            {
                const double distance = std::abs (value - values[static_cast<size_t> (i)]);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    closestIndex = i;
                }
            }

            angleOut = detentAngles[static_cast<size_t> (closestIndex)];
            return true;
        }

    private:
        std::vector<float> detentAngles;
        bool drawDetentMarks = true;
    };

    class SpriteToggleButton : public juce::ToggleButton
    {
    public:
        explicit SpriteToggleButton (juce::Image spriteImageToUse);
        void paintButton (juce::Graphics&, bool, bool) override;

    private:
        juce::Image spriteImage;
        juce::Image offImage;
        juce::Image onImage;
    };

    class PowerLedComponent : public juce::Component
    {
    public:
        explicit PowerLedComponent (juce::Image buttonImageToUse);
        void setPowerOn (bool isOn);
        void paint (juce::Graphics&) override;

    private:
        bool isPowerOn = true;
        juce::Image buttonImage;
    };

    class ImageButtonComponent : public juce::Component
    {
    public:
        explicit ImageButtonComponent (juce::Image imageToUse);
        void setMatteColour (juce::Colour newColour);
        void paint (juce::Graphics&) override;

    private:
        juce::Image image;
        juce::Colour matteColour { juce::Colours::transparentBlack };
    };

    class PowerToggleButton : public juce::ToggleButton
    {
    public:
        PowerToggleButton (juce::Image baseImageToUse, juce::Image tipImageToUse);
        ~PowerToggleButton() override;
        void paintButton (juce::Graphics&, bool, bool) override;

    private:
        struct NoToggleLookAndFeel : public juce::LookAndFeel_V4
        {
            void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                                   bool, bool) override
            {
            }
        };

        juce::Image baseImage;
        juce::Image tipImage;
        NoToggleLookAndFeel noToggleLookAndFeel;
    };

    class VUMeterComponent : public juce::Component
    {
    public:
        explicit VUMeterComponent (juce::Image faceImageToUse);
        void setSurroundColour (juce::Colour newColour);
        void setValueDb (float newValueDb);
        void setPowerOn (bool isOn);
        void paint (juce::Graphics&) override;

    private:
        float valueDb = -60.0f;
        bool isPowerOn = true;
        juce::Image faceImage;
        juce::Colour surroundColour { juce::Colour::fromRGB (30, 30, 34) };
    };

    void timerCallback() override;

    void configureKnobSlider (juce::Slider& slider, juce::LookAndFeel& lookAndFeel);
    void configureMeterSwitchSlider (MeterSwitchSlider& slider, juce::LookAndFeel& lookAndFeel);
    KFactorAudioProcessor& audioProcessor;

    juce::Image backgroundImage;

    ImageKnobLookAndFeel largeKnobLookAndFeel { KFactorAssets::getKnobImage() };
    ImageKnobLookAndFeel smallKnobLookAndFeel { KFactorAssets::getKnob2Image(),
                                                juce::MathConstants<float>::halfPi };

    SteppedSlider inputGainLeftSlider;
    SteppedSlider inputGainRightSlider;
    SteppedSlider thresholdLeftSlider;
    SteppedSlider thresholdRightSlider;
    SteppedSlider timeConstantLeftSlider;
    SteppedSlider timeConstantRightSlider;
    MeterSwitchSlider meterModeLeftSlider;
    MeterSwitchSlider meterModeRightSlider;
    MeterSwitchSlider agcSlider;

    PowerToggleButton powerToggle { KFactorAssets::getPowerToggleBaseImage(),
                                    KFactorAssets::getPowerToggleTipImage() };

    PowerLedComponent powerLed { KFactorAssets::getPowerButtonImage() };
    ImageButtonComponent fuseButton { KFactorAssets::getFuseButtonImage() };
    juce::Label powerOnLabel;
    juce::Label fuseRatingLabel;
    VUMeterComponent vuMeterLeft { KFactorAssets::getVuFaceImage() };
    VUMeterComponent vuMeterRight { KFactorAssets::getVuFaceImage() };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainLeftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainRightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdLeftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdRightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeConstantLeftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeConstantRightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> meterModeLeftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> meterModeRightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> agcAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KFactorAudioProcessorEditor)
};
