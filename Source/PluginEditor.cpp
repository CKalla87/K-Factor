#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <array>
#include <cmath>
#include <vector>

namespace
{
const std::vector<double> kInputGainSteps { 0.0, 0.6, 1.2, 2.0, 3.0, 4.0, 6.0, 8.0, 10.0, 15.0, 20.0, 30.0 };
constexpr float kMeterSwitchAngleIn = juce::degreesToRadians (241.5f);
constexpr float kMeterSwitchAngleGr = juce::degreesToRadians (266.75f);
constexpr float kMeterSwitchAngleOut = juce::degreesToRadians (296.0f);
// AGC 2-position switch: LEFT RIGHT (0) and LAT VERT (1) - shortened angle to align with L-shaped tick marks
constexpr float kAgcAngleLeft = juce::degreesToRadians (263.5f);
constexpr float kAgcAngleRight = juce::degreesToRadians (282.0f);
constexpr float kMeterSwitchDetentSpacing = kMeterSwitchAngleGr - kMeterSwitchAngleIn;
// Offset to align knob pointer with tick marks; tweaked so pointer points directly at detent
constexpr float kMeterSwitchPointerOffset = juce::MathConstants<float>::halfPi - kMeterSwitchDetentSpacing
                                          - juce::degreesToRadians (8.0f);
constexpr float kLargeKnobPointerOffset = juce::MathConstants<float>::halfPi;

float valueToAngle (juce::Slider& slider, float startAngle, float endAngle, double value)
{
    const float proportion = static_cast<float> (slider.valueToProportionOfLength (value));
    return startAngle + proportion * (endAngle - startAngle);
}

void drawKnobWithMarkers (juce::Graphics& g, juce::Slider& slider,
                          juce::Rectangle<float> bounds,
                          float startAngle, float endAngle,
                          const std::vector<double>& markerValues)
{
    const auto centre = bounds.getCentre();
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    for (double value : markerValues)
    {
        const float angle = valueToAngle (slider, startAngle, endAngle, value);

        const float r1 = radius * 0.82f;
        const float r2 = radius * 0.95f;

        juce::Line<float> tick (
            centre.x + std::cos (angle) * r1,
            centre.y + std::sin (angle) * r1,
            centre.x + std::cos (angle) * r2,
            centre.y + std::sin (angle) * r2
        );

        g.drawLine (tick, 2.0f);
    }
}

void drawKnobWithAngles (juce::Graphics& g,
                         juce::Rectangle<float> bounds,
                         const std::vector<float>& angles)
{
    const auto centre = bounds.getCentre();
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    for (float angle : angles)
    {
        const float r1 = radius * 0.82f;
        const float r2 = radius * 0.95f;

        juce::Line<float> tick (
            centre.x + std::cos (angle) * r1,
            centre.y + std::sin (angle) * r1,
            centre.x + std::cos (angle) * r2,
            centre.y + std::sin (angle) * r2
        );

        g.drawLine (tick, 2.0f);
    }
}

juce::Colour averageSampleColour (const juce::Image& image, const juce::Rectangle<int>& bounds)
{
    const std::array<juce::Point<int>, 8> samples {
        juce::Point<int> { bounds.getX() - 4, bounds.getY() + 10 },
        juce::Point<int> { bounds.getX() - 4, bounds.getBottom() - 10 },
        juce::Point<int> { bounds.getRight() + 4, bounds.getY() + 10 },
        juce::Point<int> { bounds.getRight() + 4, bounds.getBottom() - 10 },
        juce::Point<int> { bounds.getX() + 10, bounds.getY() - 4 },
        juce::Point<int> { bounds.getRight() - 10, bounds.getY() - 4 },
        juce::Point<int> { bounds.getX() + 10, bounds.getBottom() + 4 },
        juce::Point<int> { bounds.getRight() - 10, bounds.getBottom() + 4 }
    };

    int count = 0;
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;
    int sumA = 0;

    for (const auto& point : samples)
    {
        if (! image.getBounds().contains (point))
            continue;

        const auto colour = image.getPixelAt (point.x, point.y);
        sumR += colour.getRed();
        sumG += colour.getGreen();
        sumB += colour.getBlue();
        sumA += colour.getAlpha();
        ++count;
    }

    if (count == 0)
        return juce::Colour::fromRGB (30, 30, 34);

    return juce::Colour::fromRGBA (static_cast<juce::uint8> (sumR / count),
                                   static_cast<juce::uint8> (sumG / count),
                                   static_cast<juce::uint8> (sumB / count),
                                   static_cast<juce::uint8> (sumA / count));
}

void replaceNearBlackPixels (juce::Image& image,
                             const juce::Rectangle<int>& bounds,
                             juce::Colour fillColour)
{
    if (! image.isValid())
        return;

    juce::Image::BitmapData data (image, juce::Image::BitmapData::readWrite);
    const auto clipped = bounds.getIntersection (image.getBounds());

    for (int y = clipped.getY(); y < clipped.getBottom(); ++y)
    {
        for (int x = clipped.getX(); x < clipped.getRight(); ++x)
        {
            const auto colour = data.getPixelColour (x, y);
            if (colour.getAlpha() > 180
                && colour.getRed() < 20
                && colour.getGreen() < 20
                && colour.getBlue() < 20)
            {
                data.setPixelColour (x, y, fillColour);
            }
        }
    }
}

void scrubVuBackgroundPatch (juce::Image& image, const juce::Rectangle<int>& bounds)
{
    const auto fill = averageSampleColour (image, bounds);
    auto expanded = bounds.expanded (40);
    expanded.setTop (bounds.getY());
    replaceNearBlackPixels (image, expanded, fill);
}

void scrubKnobBackgroundPatch (juce::Image& image, const juce::Rectangle<int>& bounds)
{
    if (! image.isValid())
        return;

    const auto fill = averageSampleColour (image, bounds);
    juce::Graphics g (image);
    g.setColour (fill);

    const auto innerBounds = bounds.toFloat().withSizeKeepingCentre (bounds.getWidth() * 0.8f,
                                                                     bounds.getHeight() * 0.8f);
    g.fillEllipse (innerBounds);
}

void clearNearBlackEdgeBars (juce::Image& image, int edgeWidth)
{
    if (! image.isValid())
        return;

    const int width = image.getWidth();
    const int height = image.getHeight();
    const int clampedEdge = juce::jmin (edgeWidth, width / 2);

    juce::Image::BitmapData data (image, juce::Image::BitmapData::readWrite);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < clampedEdge; ++x)
        {
            const auto colour = data.getPixelColour (x, y);
            if (colour.getAlpha() > 200
                && colour.getRed() < 10
                && colour.getGreen() < 10
                && colour.getBlue() < 10)
            {
                data.setPixelColour (x, y, colour.withAlpha (0.0f));
            }
        }

        for (int x = width - clampedEdge; x < width; ++x)
        {
            const auto colour = data.getPixelColour (x, y);
            if (colour.getAlpha() > 200
                && colour.getRed() < 10
                && colour.getGreen() < 10
                && colour.getBlue() < 10)
            {
                data.setPixelColour (x, y, colour.withAlpha (0.0f));
            }
        }
    }
}

void clearNearBlackBorder (juce::Image& image, int borderWidth)
{
    if (! image.isValid())
        return;

    const int width = image.getWidth();
    const int height = image.getHeight();
    const int clampedBorder = juce::jmin (borderWidth, juce::jmin (width / 2, height / 2));

    juce::Image::BitmapData data (image, juce::Image::BitmapData::readWrite);

    for (int y = 0; y < height; ++y)
    {
        const bool topOrBottom = (y < clampedBorder) || (y >= height - clampedBorder);
        for (int x = 0; x < width; ++x)
        {
            const bool leftOrRight = (x < clampedBorder) || (x >= width - clampedBorder);
            if (! topOrBottom && ! leftOrRight)
                continue;

            const auto colour = data.getPixelColour (x, y);
            const auto r = colour.getRed();
            const auto g = colour.getGreen();
            const auto b = colour.getBlue();
            const auto maxChannel = juce::jmax (r, juce::jmax (g, b));
            const auto minChannel = juce::jmin (r, juce::jmin (g, b));
            const bool lowSaturation = (maxChannel - minChannel) < 18;
            const bool darkPixel = maxChannel < 28;
            if (colour.getAlpha() > 160 && darkPixel && lowSaturation)
            {
                data.setPixelColour (x, y, colour.withAlpha (0.0f));
            }
        }
    }
}

void applyMatteToTransparentPixels (juce::Image& image, juce::Colour matteColour, juce::uint8 alphaThreshold)
{
    if (! image.isValid() || matteColour.getAlpha() == 0)
        return;

    juce::Image::BitmapData data (image, juce::Image::BitmapData::readWrite);
    const int width = image.getWidth();
    const int height = image.getHeight();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const auto colour = data.getPixelColour (x, y);
            if (colour.getAlpha() == 0 || colour.getAlpha() > alphaThreshold)
                continue;

            data.setPixelColour (x, y, juce::Colour::fromRGBA (matteColour.getRed(),
                                                              matteColour.getGreen(),
                                                              matteColour.getBlue(),
                                                              colour.getAlpha()));
        }
    }
}
}

KFactorAudioProcessorEditor::ImageKnobLookAndFeel::ImageKnobLookAndFeel (juce::Image imageToUse,
                                                                        float pointerOffsetRadians)
    : knobImage (std::move (imageToUse)),
      pointerAngleOffsetRadians (pointerOffsetRadians)
{
}

void KFactorAudioProcessorEditor::ImageKnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y,
                                                                          int width, int height, float sliderPos,
                                                                          float rotaryStartAngle, float rotaryEndAngle,
                                                                          juce::Slider& slider)
{
    juce::ignoreUnused (sliderPos);
    const auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                                static_cast<float> (width), static_cast<float> (height));
    const auto drawBounds = bounds.withSizeKeepingCentre (bounds.getWidth(), bounds.getHeight());
    const float sliderProportion = static_cast<float> (slider.valueToProportionOfLength (slider.getValue()));
    float angle = rotaryStartAngle + sliderProportion * (rotaryEndAngle - rotaryStartAngle);

    if (auto* meterSwitch = dynamic_cast<MeterSwitchSlider*> (&slider))
    {
        float detentAngle = 0.0f;
        if (meterSwitch->getAngleForValue (slider.getValue(), detentAngle))
            angle = detentAngle;

        const auto& detents = meterSwitch->getDetentAngles();
        if (meterSwitch->getDrawDetentMarks() && ! detents.empty())
        {
            g.setColour (juce::Colours::white);
            drawKnobWithAngles (g, drawBounds, detents);
        }
    }
    else if (auto* stepped = dynamic_cast<SteppedSlider*> (&slider))
    {
        if (stepped->getDrawMarkers())
        {
            const auto& markers = stepped->getAllowedValues();
            if (! markers.empty())
            {
                g.setColour (juce::Colours::white.withAlpha (0.65f));
                drawKnobWithMarkers (g, slider, drawBounds, rotaryStartAngle, rotaryEndAngle, markers);
            }
        }
    }

    angle += pointerAngleOffsetRadians;

    if (knobImage.isValid())
    {
        const float scale = juce::jmin (drawBounds.getWidth() / static_cast<float> (knobImage.getWidth()),
                                        drawBounds.getHeight() / static_cast<float> (knobImage.getHeight()));
        const float centerX = drawBounds.getCentreX();
        const float centerY = drawBounds.getCentreY();

        juce::AffineTransform transform = juce::AffineTransform::translation (-knobImage.getWidth() * 0.5f,
                                                                              -knobImage.getHeight() * 0.5f)
                                              .scaled (scale)
                                              .rotated (angle)
                                              .translated (centerX, centerY);
        g.drawImageTransformed (knobImage, transform, false);
    }
}

KFactorAudioProcessorEditor::SpriteToggleButton::SpriteToggleButton (juce::Image spriteImageToUse)
    : spriteImage (std::move (spriteImageToUse))
{
    setClickingTogglesState (true);

    if (spriteImage.isValid())
    {
        spriteImage = spriteImage.convertedToFormat (juce::Image::PixelFormat::ARGB);
        const int halfWidth = spriteImage.getWidth() / 2;
        const int height = spriteImage.getHeight();
        offImage = spriteImage.getClippedImage (juce::Rectangle<int> (0, 0, halfWidth, height));

        const auto onRaw = spriteImage.getClippedImage (juce::Rectangle<int> (halfWidth, 0, halfWidth, height));
        onImage = juce::Image (spriteImage.getFormat(), halfWidth, height, true);

        juce::Image::BitmapData src (onRaw, juce::Image::BitmapData::readOnly);
        juce::Image::BitmapData dst (onImage, juce::Image::BitmapData::writeOnly);

        for (int y = 0; y < height; ++y)
        {
            const int flippedY = height - 1 - y;
            for (int x = 0; x < halfWidth; ++x)
                dst.setPixelColour (x, y, src.getPixelColour (x, flippedY));
        }
    }
}

void KFactorAudioProcessorEditor::SpriteToggleButton::paintButton (juce::Graphics& g, bool, bool)
{
    if (! spriteImage.isValid())
        return;

    const auto bounds = getLocalBounds().toFloat();
    const bool isOn = getToggleState();
    const auto& frame = isOn ? onImage : offImage;

    const int frameWidth = frame.isValid() ? frame.getWidth() : 0;
    const int frameHeight = frame.isValid() ? frame.getHeight() : 0;
    if (frameWidth <= 0 || frameHeight <= 0)
        return;

    const float scale = juce::jmin (bounds.getWidth() / static_cast<float> (frameWidth),
                                    bounds.getHeight() / static_cast<float> (frameHeight));
    const float drawWidth = static_cast<float> (frameWidth) * scale;
    const float drawHeight = static_cast<float> (frameHeight) * scale;
    const auto dest = bounds.withSizeKeepingCentre (drawWidth, drawHeight);

    g.drawImage (frame, dest.getX(), dest.getY(), dest.getWidth(), dest.getHeight(),
                 0.0f, 0.0f, static_cast<float> (frameWidth), static_cast<float> (frameHeight));
}

KFactorAudioProcessorEditor::PowerLedComponent::PowerLedComponent (juce::Image buttonImageToUse)
    : buttonImage (std::move (buttonImageToUse))
{
}

void KFactorAudioProcessorEditor::PowerLedComponent::setPowerOn (bool isOn)
{
    if (isPowerOn != isOn)
    {
        isPowerOn = isOn;
        repaint();
    }
}

void KFactorAudioProcessorEditor::PowerLedComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    if (buttonImage.isValid())
    {
        const float scale = juce::jmin (bounds.getWidth() / static_cast<float> (buttonImage.getWidth()),
                                        bounds.getHeight() / static_cast<float> (buttonImage.getHeight()));
        const float drawW = static_cast<float> (buttonImage.getWidth()) * scale;
        const float drawH = static_cast<float> (buttonImage.getHeight()) * scale;
        const auto drawBounds = bounds.withSizeKeepingCentre (drawW, drawH);
        g.drawImage (buttonImage, drawBounds.getX(), drawBounds.getY(), drawBounds.getWidth(), drawBounds.getHeight(),
                     0.0f, 0.0f, static_cast<float> (buttonImage.getWidth()),
                     static_cast<float> (buttonImage.getHeight()));
    }

    if (! isPowerOn)
        return;

    const float glowDiameter = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.32f;
    const auto glowBounds = bounds.withSizeKeepingCentre (glowDiameter, glowDiameter);
    juce::ColourGradient glow (juce::Colour::fromRGB (255, 210, 110), glowBounds.getCentreX(), glowBounds.getCentreY(),
                               juce::Colour::fromRGB (255, 120, 0), glowBounds.getX(), glowBounds.getY(), true);
    g.setGradientFill (glow);
    g.setOpacity (0.55f);
    g.fillEllipse (glowBounds);
    g.setOpacity (1.0f);
}

KFactorAudioProcessorEditor::ImageButtonComponent::ImageButtonComponent (juce::Image imageToUse)
    : image (std::move (imageToUse))
{
    if (image.isValid())
    {
        image = image.convertedToFormat (juce::Image::PixelFormat::ARGB);
        clearNearBlackBorder (image, 10);
    }
}

void KFactorAudioProcessorEditor::ImageButtonComponent::setMatteColour (juce::Colour newColour)
{
    matteColour = newColour;
    if (image.isValid())
        applyMatteToTransparentPixels (image, matteColour, 80);
    repaint();
}

void KFactorAudioProcessorEditor::ImageButtonComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    if (image.isValid())
    {
        juce::Graphics::ScopedSaveState saveState (g);
        g.setImageResamplingQuality (juce::Graphics::lowResamplingQuality);

        const float srcInset = 8.0f;
        const float maxInset = 0.45f * juce::jmin (static_cast<float> (image.getWidth()),
                                                   static_cast<float> (image.getHeight()));
        const float safeInset = juce::jmin (srcInset, maxInset);
        const float srcW = static_cast<float> (image.getWidth()) - 2.0f * safeInset;
        const float srcH = static_cast<float> (image.getHeight()) - 2.0f * safeInset;

        const float scale = juce::jmin (bounds.getWidth() / static_cast<float> (image.getWidth()),
                                        bounds.getHeight() / static_cast<float> (image.getHeight()));
        const float drawW = static_cast<float> (image.getWidth()) * scale;
        const float drawH = static_cast<float> (image.getHeight()) * scale;
        const auto drawBounds = bounds.withSizeKeepingCentre (drawW, drawH);
        const float destInset = safeInset * scale;
        const auto dest = drawBounds.reduced (destInset);
        g.drawImage (image, dest.getX(), dest.getY(),
                     dest.getWidth(), dest.getHeight(),
                     safeInset, safeInset, srcW, srcH);
    }
}

KFactorAudioProcessorEditor::PowerToggleButton::PowerToggleButton (juce::Image baseImageToUse,
                                                                   juce::Image tipImageToUse)
    : baseImage (std::move (baseImageToUse)),
      tipImage (std::move (tipImageToUse))
{
    setClickingTogglesState (true);
    setLookAndFeel (&noToggleLookAndFeel);
}

KFactorAudioProcessorEditor::PowerToggleButton::~PowerToggleButton()
{
    setLookAndFeel (nullptr);
}

void KFactorAudioProcessorEditor::PowerToggleButton::paintButton (juce::Graphics& g, bool, bool)
{
    const auto bounds = getLocalBounds().toFloat();

    const bool hasBase = baseImage.isValid();
    const bool hasTip = tipImage.isValid();
    if (! hasBase && ! hasTip)
        return;

    const float baseWidth = hasBase ? static_cast<float> (baseImage.getWidth())
                                    : static_cast<float> (tipImage.getWidth());
    const float baseHeight = hasBase ? static_cast<float> (baseImage.getHeight())
                                     : static_cast<float> (tipImage.getHeight());
    const float scale = (baseWidth > 0.0f && baseHeight > 0.0f)
        ? juce::jmin (bounds.getWidth() / baseWidth, bounds.getHeight() / baseHeight)
        : 1.0f;
    const float drawW = baseWidth * scale;
    const float drawH = baseHeight * scale;
    const auto baseDrawBounds = bounds.withSizeKeepingCentre (drawW, drawH);

    if (hasBase)
    {
        g.drawImage (baseImage, baseDrawBounds.getX(), baseDrawBounds.getY(),
                     baseDrawBounds.getWidth(), baseDrawBounds.getHeight(),
                     0.0f, 0.0f, baseWidth, baseHeight);
    }

    if (! hasTip)
        return;

    const float angleDegrees = getToggleState() ? 25.0f : -25.0f;
    const float angle = juce::degreesToRadians (angleDegrees);

    const auto pivot = baseDrawBounds.getCentre();
    const float tipWidth = static_cast<float> (tipImage.getWidth());
    const float tipHeight = static_cast<float> (tipImage.getHeight());

    juce::AffineTransform transform = juce::AffineTransform::translation (-tipWidth * 0.5f, -tipHeight * 0.5f)
                                          .scaled (scale)
                                          .rotated (angle)
                                          .translated (pivot.x, pivot.y);

    g.drawImageTransformed (tipImage, transform, false);
}

KFactorAudioProcessorEditor::VUMeterComponent::VUMeterComponent (juce::Image faceImageToUse)
    : faceImage (std::move (faceImageToUse))
{
    if (faceImage.isValid())
    {
        faceImage = faceImage.convertedToFormat (juce::Image::PixelFormat::ARGB);
        clearNearBlackEdgeBars (faceImage, 30);
        clearNearBlackBorder (faceImage, 40);
    }
}

void KFactorAudioProcessorEditor::VUMeterComponent::setSurroundColour (juce::Colour newColour)
{
    surroundColour = newColour;
    repaint();
}

void KFactorAudioProcessorEditor::VUMeterComponent::setValueDb (float newValueDb)
{
    valueDb = newValueDb;
    repaint();
}

void KFactorAudioProcessorEditor::VUMeterComponent::setPowerOn (bool isOn)
{
    isPowerOn = isOn;
    repaint();
}

void KFactorAudioProcessorEditor::VUMeterComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto faceBounds = bounds;

    if (isPowerOn)
    {
        // Warm glow behind the yellow meter face.
        const auto glowBounds = faceBounds.withSizeKeepingCentre (faceBounds.getWidth() * 0.9f,
                                                                  faceBounds.getHeight() * 0.7f)
                                              .translated (0.0f, -faceBounds.getHeight() * 0.05f);
        juce::ColourGradient glow (juce::Colour::fromRGBA (255, 220, 140, 110),
                                   glowBounds.getCentreX(), glowBounds.getCentreY(),
                                   juce::Colour::fromRGBA (255, 170, 60, 0),
                                   glowBounds.getX(), glowBounds.getY(), true);
        g.setGradientFill (glow);
        g.fillEllipse (glowBounds);
    }

    if (faceImage.isValid())
    {
        const float faceInset = 40.0f;
        const auto dest = bounds;
        const float radius = juce::jmin (dest.getWidth(), dest.getHeight()) * 0.08f;
        const float srcInset = juce::jmin (faceInset, 0.5f * juce::jmin (static_cast<float> (faceImage.getWidth()),
                                                                        static_cast<float> (faceImage.getHeight())));

        juce::Graphics::ScopedSaveState clipState (g);
        juce::Path clipPath;
        clipPath.addRoundedRectangle (dest, radius);
        g.reduceClipRegion (clipPath);

        g.drawImage (faceImage,
                     dest.getX(), dest.getY(), dest.getWidth(), dest.getHeight(),
                     srcInset, srcInset,
                     static_cast<float> (faceImage.getWidth()) - 2.0f * srcInset,
                     static_cast<float> (faceImage.getHeight()) - 2.0f * srcInset);

    }

    constexpr float baseWidth = 280.0f;
    constexpr float baseHeight = 180.0f;
    const float scaleX = faceBounds.getWidth() / baseWidth;
    const float scaleY = faceBounds.getHeight() / baseHeight;

    juce::Graphics::ScopedSaveState save (g);
    g.addTransform (juce::AffineTransform::translation (faceBounds.getX(), faceBounds.getY())
                        .scaled (scaleX, scaleY));

    const float meterCenterX = baseWidth * 0.5f;
    const float meterCenterY = baseHeight - 40.0f;
    const float arcRadius = baseHeight * 0.62f;
    const float needlePivotY = baseHeight - 65.0f;

    const float startAngle = juce::degreesToRadians (210.0f);
    const float endAngle = juce::degreesToRadians (330.0f);

    if (isPowerOn)
    {
        juce::ColourGradient glow (juce::Colour::fromRGB (255, 215, 120), meterCenterX, baseHeight * 0.42f,
                                   juce::Colour::fromRGB (255, 180, 60), meterCenterX, baseHeight * 0.15f, true);
        g.setGradientFill (glow);
        g.setOpacity (0.05f);
        g.fillEllipse (meterCenterX - arcRadius * 0.75f, baseHeight * 0.16f, arcRadius * 1.5f, arcRadius * 0.9f);
        g.setOpacity (1.0f);
    }

    const float minDb = -20.0f;
    const float maxDb = 3.0f;
    const float effectiveDb = isPowerOn
        ? ((valueDb <= minDb + 0.1f) ? -10.0f : valueDb)
        : minDb;
    const float clampedDb = juce::jlimit (minDb, maxDb, effectiveDb);
    const float normalized = (clampedDb - minDb) / (maxDb - minDb);
    const float needleAngle = startAngle + normalized * (endAngle - startAngle);

    const float needleLength = arcRadius - 35.0f;
    const float needleTargetX = meterCenterX + std::cos (needleAngle) * needleLength;
    const float needleTargetY = needlePivotY + std::sin (needleAngle) * needleLength;

    g.setColour (juce::Colour::fromRGB (0, 0, 0));
    g.drawLine (meterCenterX, needlePivotY, needleTargetX, needleTargetY, 2.5f);
    g.fillEllipse (meterCenterX - 3.0f, needlePivotY - 3.0f, 6.0f, 6.0f);
}

//==============================================================================
KFactorAudioProcessorEditor::KFactorAudioProcessorEditor (KFactorAudioProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      largeKnobLookAndFeel (KFactorAssets::getKnobImage(), kLargeKnobPointerOffset),
      smallKnobLookAndFeel (KFactorAssets::getKnob2Image(), kMeterSwitchPointerOffset)
{
    backgroundImage = KFactorAssets::getBackgroundImage();
    if (backgroundImage.isValid())
    {
        backgroundImage = backgroundImage.createCopy();
        scrubVuBackgroundPatch (backgroundImage, LayoutConstants::kVUMeterLeftBounds);
        scrubVuBackgroundPatch (backgroundImage, LayoutConstants::kVUMeterRightBounds);
        // Left vert metering, right vert metering, and AGC: do not scrub (removes visible circles)
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kInputGainLeftBounds);
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kInputGainRightBounds);
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kThresholdLeftBounds);
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kThresholdRightBounds);
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kTimeConstantLeftBounds);
        scrubKnobBackgroundPatch (backgroundImage, LayoutConstants::kTimeConstantRightBounds);

        vuMeterLeft.setSurroundColour (averageSampleColour (backgroundImage, LayoutConstants::kVUMeterLeftBounds));
        vuMeterRight.setSurroundColour (averageSampleColour (backgroundImage, LayoutConstants::kVUMeterRightBounds));

        fuseButton.setMatteColour (averageSampleColour (backgroundImage, LayoutConstants::kFuseButtonBounds));
    }

    setSize (LayoutConstants::kUIW, LayoutConstants::kUIH);
    setResizable (false, false);

    configureKnobSlider (inputGainLeftSlider, largeKnobLookAndFeel);
    configureKnobSlider (inputGainRightSlider, largeKnobLookAndFeel);
    configureKnobSlider (thresholdLeftSlider, largeKnobLookAndFeel);
    configureKnobSlider (thresholdRightSlider, largeKnobLookAndFeel);
    configureKnobSlider (timeConstantLeftSlider, largeKnobLookAndFeel);
    configureKnobSlider (timeConstantRightSlider, largeKnobLookAndFeel);

    inputGainLeftSlider.setAllowedValues (kInputGainSteps);
    inputGainRightSlider.setAllowedValues (kInputGainSteps);
    inputGainLeftSlider.setDrawMarkers (false);
    inputGainRightSlider.setDrawMarkers (false);
    thresholdLeftSlider.setRange (1.0, 6.0, 1.0);
    thresholdRightSlider.setRange (1.0, 6.0, 1.0);
    timeConstantLeftSlider.setRange (1.0, 6.0, 1.0);
    timeConstantRightSlider.setRange (1.0, 6.0, 1.0);

    configureMeterSwitchSlider (meterModeLeftSlider, smallKnobLookAndFeel);
    configureMeterSwitchSlider (meterModeRightSlider, smallKnobLookAndFeel);
    meterModeLeftSlider.setDrawDetentMarks (false);
    meterModeRightSlider.setDrawDetentMarks (false);
    const std::vector<double> meterModeSteps { 0.0, 1.0, 2.0 };
    meterModeLeftSlider.setAllowedValues (meterModeSteps);
    meterModeRightSlider.setAllowedValues (meterModeSteps);
    const std::vector<float> meterModeAngles { kMeterSwitchAngleIn, kMeterSwitchAngleGr, kMeterSwitchAngleOut };
    meterModeLeftSlider.setDetentAngles (meterModeAngles);
    meterModeRightSlider.setDetentAngles (meterModeAngles);

    configureMeterSwitchSlider (agcSlider, smallKnobLookAndFeel);
    agcSlider.setDrawDetentMarks (false);
    const std::vector<double> agcSteps { 0.0, 1.0 };
    agcSlider.setAllowedValues (agcSteps);
    const std::vector<float> agcAngles { kAgcAngleLeft, kAgcAngleRight };
    agcSlider.setDetentAngles (agcAngles);

    auto configureTextLabel = [] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setFont (juce::Font (12.0f, juce::Font::bold));
        label.setColour (juce::Label::textColourId, juce::Colours::whitesmoke);
        label.setInterceptsMouseClicks (false, false);
    };

    configureTextLabel (powerOnLabel, "ON");
    configureTextLabel (fuseRatingLabel, "5A");


    addAndMakeVisible (agcSlider);
    addAndMakeVisible (powerToggle);
    addAndMakeVisible (powerLed);
    addAndMakeVisible (fuseButton);
    addAndMakeVisible (powerOnLabel);
    addAndMakeVisible (fuseRatingLabel);
    addAndMakeVisible (vuMeterLeft);
    addAndMakeVisible (vuMeterRight);

    inputGainLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "INPUT_GAIN_L", inputGainLeftSlider);
    inputGainRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "INPUT_GAIN_R", inputGainRightSlider);
    thresholdLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "THRESHOLD_L", thresholdLeftSlider);
    thresholdRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "THRESHOLD_R", thresholdRightSlider);
    timeConstantLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "TIME_CONSTANT_L", timeConstantLeftSlider);
    timeConstantRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "TIME_CONSTANT_R", timeConstantRightSlider);
    meterModeLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "METER_MODE_L", meterModeLeftSlider);
    meterModeRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "METER_MODE_R", meterModeRightSlider);
    agcAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "AGC_MODE", agcSlider);
    powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "POWER", powerToggle);

    startTimerHz (30);
}

KFactorAudioProcessorEditor::~KFactorAudioProcessorEditor()
{
    inputGainLeftSlider.setLookAndFeel (nullptr);
    inputGainRightSlider.setLookAndFeel (nullptr);
    thresholdLeftSlider.setLookAndFeel (nullptr);
    thresholdRightSlider.setLookAndFeel (nullptr);
    timeConstantLeftSlider.setLookAndFeel (nullptr);
    timeConstantRightSlider.setLookAndFeel (nullptr);
    meterModeLeftSlider.setLookAndFeel (nullptr);
    meterModeRightSlider.setLookAndFeel (nullptr);
    agcSlider.setLookAndFeel (nullptr);
}

void KFactorAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (backgroundImage.isValid())
        g.drawImageAt (backgroundImage, 0, 0);
}

void KFactorAudioProcessorEditor::resized()
{
    inputGainLeftSlider.setBounds (LayoutConstants::kInputGainLeftBounds);
    inputGainRightSlider.setBounds (LayoutConstants::kInputGainRightBounds);
    thresholdLeftSlider.setBounds (LayoutConstants::kThresholdLeftBounds);
    thresholdRightSlider.setBounds (LayoutConstants::kThresholdRightBounds);
    timeConstantLeftSlider.setBounds (LayoutConstants::kTimeConstantLeftBounds);
    timeConstantRightSlider.setBounds (LayoutConstants::kTimeConstantRightBounds);

    meterModeLeftSlider.setBounds (LayoutConstants::kMeterSwitchLeftBounds);
    meterModeRightSlider.setBounds (LayoutConstants::kMeterSwitchRightBounds);

    powerToggle.setBounds (LayoutConstants::kPowerToggleBounds);
    powerLed.setBounds (LayoutConstants::kPowerLedBounds);
    fuseButton.setBounds (LayoutConstants::kFuseButtonBounds);

    const auto powerBounds = LayoutConstants::kPowerToggleBounds;
    const auto fuseBounds = LayoutConstants::kFuseButtonBounds;
    const int labelHeight = 12;
    const int powerLabelPadding = -8;
    const int fuseLabelPadding = 1;

    powerOnLabel.setBounds (powerBounds.getX(),
                            powerBounds.getY() - labelHeight - powerLabelPadding,
                            powerBounds.getWidth(),
                            labelHeight);
    fuseRatingLabel.setBounds (fuseBounds.getX(),
                               fuseBounds.getBottom() + fuseLabelPadding,
                               fuseBounds.getWidth(),
                               labelHeight);

    vuMeterLeft.setBounds (LayoutConstants::kVUMeterLeftBounds);
    vuMeterRight.setBounds (LayoutConstants::kVUMeterRightBounds);

    agcSlider.setBounds (LayoutConstants::kAgcToggleBounds);
}

void KFactorAudioProcessorEditor::timerCallback()
{
    const bool powered = powerToggle.getToggleState();
    powerLed.setPowerOn (powered);
    vuMeterLeft.setPowerOn (powered);
    vuMeterRight.setPowerOn (powered);

    const int leftMode = static_cast<int> (meterModeLeftSlider.getValue());
    const int rightMode = static_cast<int> (meterModeRightSlider.getValue());

    auto selectValue = [this] (int mode, bool left)
    {
        if (mode == static_cast<int> (KFactorAudioProcessor::MeterMode::input))
            return left ? audioProcessor.getMeterInputLeft() : audioProcessor.getMeterInputRight();
        if (mode == static_cast<int> (KFactorAudioProcessor::MeterMode::gainReduction))
            return -(left ? audioProcessor.getMeterGainReductionLeft() : audioProcessor.getMeterGainReductionRight());
        return left ? audioProcessor.getMeterOutputLeft() : audioProcessor.getMeterOutputRight();
    };

    vuMeterLeft.setValueDb (selectValue (leftMode, true));
    vuMeterRight.setValueDb (selectValue (rightMode, false));
}

void KFactorAudioProcessorEditor::configureKnobSlider (juce::Slider& slider, juce::LookAndFeel& lookAndFeel)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRotaryParameters (-juce::MathConstants<float>::pi * 0.75f,
                                juce::MathConstants<float>::pi * 0.75f, true);
    slider.setLookAndFeel (&lookAndFeel);
    addAndMakeVisible (slider);
}

void KFactorAudioProcessorEditor::configureMeterSwitchSlider (MeterSwitchSlider& slider, juce::LookAndFeel& lookAndFeel)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

    constexpr float startAngle = kMeterSwitchAngleIn;
    constexpr float endAngle = kMeterSwitchAngleOut;
    slider.setRotaryParameters (startAngle, endAngle, true);
    slider.setLookAndFeel (&lookAndFeel);
    addAndMakeVisible (slider);
}

