# K-Factor Compressor Plugin

A fast-attack FET-style compressor plugin with a dark, spooky aesthetic and colorful controls.

## Features

- **Fast Attack**: 1176-style ultra-fast attack times (0.01ms - 100ms)
- **Full Compressor Controls**:
  - Threshold (-60dB to 0dB)
  - Ratio (1:1 to 20:1)
  - Attack (0.01ms to 100ms) - optimized for fast FET-style response
  - Release (1ms to 2000ms)
  - Makeup Gain (0dB to 24dB)

- **Spooky Dark UI**: Dark purple/black background with colorful accent controls
  - Red accents for threshold
  - Orange accents for ratio and makeup
  - Cyan accents for attack
  - Purple accents for release

## Setup

1. **Open in JUCE Projucer**:
   - Open `K-Factor.jucer` in JUCE Projucer
   - The project is configured to use JUCE modules from the NebulaEQ project
   - If needed, update the module paths to point to your JUCE installation

2. **Build**:
   - Click "Save and Open in IDE" in Projucer
   - Build in Xcode (macOS) or Visual Studio (Windows)

3. **Run**:
   - Use the "K-Factor - Standalone Plugin" scheme to run directly
   - Or load as VST3/AU in your DAW

## Technical Details

- Uses JUCE's DSP Compressor module for core processing
- Fast attack times optimized for 1176-style FET compressor characteristics
- Colorful Sound Toys-inspired UI with dark spooky theme

Enjoy the compression! 🎃

## License

Copyright 2025 CK Audio Design
