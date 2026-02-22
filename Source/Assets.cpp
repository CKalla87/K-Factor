#include "Assets.h"

namespace KFactorAssets
{
#include "Assets/knob_png.inc"
#include "Assets/knob2_png.inc"
#include "Assets/vu_cover_png.inc"
#include "Assets/kfactor_background_png.inc"
#include "Assets/toggle_sprite_png.inc"
#include "Assets/kfactor_vu_face_png.inc"
#include "Assets/power_button_png.inc"
#include "Assets/fuse_button_png.inc"
#include "Assets/toggle_base_png.inc"
#include "Assets/power_toggle_tip_png.inc"

const void* getKnobPngData()
{
    return knob_png;
}

size_t getKnobPngSize()
{
    return static_cast<size_t> (knob_png_len);
}

const void* getKnob2PngData()
{
    return knob2_png;
}

size_t getKnob2PngSize()
{
    return static_cast<size_t> (knob2_png_len);
}

const void* getVuCoverPngData()
{
    return vu_cover_png;
}

size_t getVuCoverPngSize()
{
    return static_cast<size_t> (vu_cover_png_len);
}

const void* getBackgroundPngData()
{
    return kfactor_background_png;
}

size_t getBackgroundPngSize()
{
    return static_cast<size_t> (kfactor_background_png_len);
}

const void* getToggleSpritePngData()
{
    return toggle_sprite_png;
}

size_t getToggleSpritePngSize()
{
    return static_cast<size_t> (toggle_sprite_png_len);
}

const void* getVuFacePngData()
{
    return kfactor_vu_face_png;
}

size_t getVuFacePngSize()
{
    return static_cast<size_t> (kfactor_vu_face_png_len);
}

const void* getPowerButtonPngData()
{
    return power_button_png;
}

size_t getPowerButtonPngSize()
{
    return static_cast<size_t> (power_button_png_len);
}

const void* getFuseButtonPngData()
{
    return fuse_button_png;
}

size_t getFuseButtonPngSize()
{
    return static_cast<size_t> (fuse_button_png_len);
}

const void* getPowerToggleBasePngData()
{
    return toggle_base_png;
}

size_t getPowerToggleBasePngSize()
{
    return static_cast<size_t> (toggle_base_png_len);
}

const void* getPowerToggleTipPngData()
{
    return power_toggle_tip_png;
}

size_t getPowerToggleTipPngSize()
{
    return static_cast<size_t> (power_toggle_tip_png_len);
}

juce::Image getKnobImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getKnobPngData(), getKnobPngSize());
    return image;
}

juce::Image getKnob2Image()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getKnob2PngData(), getKnob2PngSize());
    return image;
}

juce::Image getVuCoverImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getVuCoverPngData(), getVuCoverPngSize());
    return image;
}

juce::Image getBackgroundImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getBackgroundPngData(), getBackgroundPngSize());
    return image;
}

juce::Image getToggleSpriteImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getToggleSpritePngData(), getToggleSpritePngSize());
    return image;
}

juce::Image getVuFaceImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getVuFacePngData(), getVuFacePngSize());
    return image;
}

juce::Image getPowerButtonImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getPowerButtonPngData(), getPowerButtonPngSize());
    return image;
}

juce::Image getFuseButtonImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getFuseButtonPngData(), getFuseButtonPngSize());
    return image;
}

juce::Image getPowerToggleBaseImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getPowerToggleBasePngData(), getPowerToggleBasePngSize());
    return image;
}

juce::Image getPowerToggleTipImage()
{
    static juce::Image image = juce::ImageCache::getFromMemory (getPowerToggleTipPngData(), getPowerToggleTipPngSize());
    return image;
}
}
