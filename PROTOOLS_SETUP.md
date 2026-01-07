# Pro Tools AAX Setup Guide

This guide will help you set up and test the K-Factor plugin in Pro Tools.

## Prerequisites

1. **AAX SDK**: You need the AAX SDK from Avid. You can get it from:
   - Avid Developer Connection: https://developer.avid.com/
   - You'll need to create an account and download the AAX SDK

2. **JUCE Projucer**: Make sure you have JUCE Projucer installed

## Step 1: Configure AAX in Projucer

1. **Open the project in Projucer**:
   ```bash
   open K-Factor.jucer
   ```
   (Or open it in JUCE Projucer application)

2. **Set AAX SDK Path**:
   - In Projucer, go to **File → Global Paths** (or **Projucer → Global Paths** on Mac)
   - Set the **AAX SDK Path** to where you installed the AAX SDK
   - Example: `/Users/YourName/AAX_SDK` or wherever you extracted it

3. **Enable AAX Format**:
   - In the main Projucer window, look for the **Plugin Formats** section
   - Make sure **AAX** is checked/enabled
   - The project file has been updated to include AAX, but you may need to verify in Projucer

4. **Save and Re-export**:
   - Click **Save Project** in Projucer
   - Click **Save and Open in IDE** to regenerate the Xcode project with AAX support

## Step 2: Build the AAX Plugin

### Option A: Build from Xcode

1. **Open Xcode project**:
   ```bash
   open "Builds/MacOSX/K-Factor.xcodeproj"
   ```

2. **Select the AAX target**:
   - In Xcode, look for the scheme selector (top toolbar)
   - Select **"K-Factor - AAX"** scheme

3. **Build**:
   - Press `Cmd+B` or go to **Product → Build**
   - The AAX plugin will be built to: `Builds/MacOSX/build/Debug/K-Factor.aaxplugin`

### Option B: Build from Terminal

```bash
cd "Builds/MacOSX"
xcodebuild -project K-Factor.xcodeproj -scheme "K-Factor - AAX" -configuration Debug
```

## Step 3: Install the AAX Plugin

1. **Locate the built plugin**:
   - Debug build: `Builds/MacOSX/build/Debug/K-Factor.aaxplugin`
   - Release build: `Builds/MacOSX/build/Release/K-Factor.aaxplugin`

2. **Copy to Pro Tools plugin directory**:
   ```bash
   # For system-wide installation (requires admin):
   sudo cp -R "Builds/MacOSX/build/Debug/K-Factor.aaxplugin" \
     "/Library/Application Support/Avid/Audio/Plug-Ins/"
   
   # Or for user-only installation:
   mkdir -p ~/Library/Application\ Support/Avid/Audio/Plug-Ins/
   cp -R "Builds/MacOSX/build/Debug/K-Factor.aaxplugin" \
     ~/Library/Application\ Support/Avid/Audio/Plug-Ins/
   ```

## Step 4: Test in Pro Tools

1. **Quit Pro Tools** (if it's running) - Important! Pro Tools scans plugins on startup

2. **Launch Pro Tools**

3. **Create a new session or open an existing one**

4. **Add the plugin**:
   - Create a new audio track
   - In the Mix or Edit window, click on an empty **Insert** slot
   - Navigate through the plugin menu: **Multi-Channel Plug-In → Dynamics → K-Factor**
   - Or search for "K-Factor" in the plugin browser

5. **Test the plugin**:
   - Load some audio onto the track
   - Adjust the compressor parameters:
     - Threshold
     - Ratio
     - Attack
     - Release
     - Makeup Gain
   - Play back and verify the compression is working

## Troubleshooting

### Plugin doesn't appear in Pro Tools

1. **Rescan plugins**:
   ```bash
   # Quit Pro Tools first, then:
   rm ~/Library/Preferences/Avid/Pro\ Tools/InstalledAAXPlugIns
   # Restart Pro Tools - it will rescan all plugins
   ```

2. **Check plugin location**:
   - Make sure the `.aaxplugin` file is in the correct directory
   - Verify the file permissions are correct

3. **Check Pro Tools version**:
   - Pro Tools 11+ requires 64-bit AAX plugins
   - Make sure you built a 64-bit version

4. **Check console/logs**:
   - Open Console.app and filter for "Pro Tools" or "AAX" to see error messages

### Build errors related to AAX SDK

- Make sure the AAX SDK path is correctly set in Projucer
- Verify the AAX SDK version is compatible with your JUCE version
- Check that all AAX SDK files are present in the SDK directory

### Plugin crashes in Pro Tools

- Make sure you're using a Debug build for testing (easier to debug)
- Check Console.app for crash logs
- Verify all JUCE modules are correctly linked

## Quick Build & Install Script

You can create a script to automate building and installing:

```bash
#!/bin/bash
# build_and_install_aax.sh

cd "$(dirname "$0")/.."
cd Builds/MacOSX

# Build AAX
echo "Building AAX plugin..."
xcodebuild -project K-Factor.xcodeproj \
  -scheme "K-Factor - AAX" \
  -configuration Debug \
  clean build

# Install to user directory
PLUGIN_PATH="build/Debug/K-Factor.aaxplugin"
INSTALL_DIR="$HOME/Library/Application Support/Avid/Audio/Plug-Ins"

if [ -d "$PLUGIN_PATH" ]; then
  echo "Installing plugin to $INSTALL_DIR..."
  mkdir -p "$INSTALL_DIR"
  cp -R "$PLUGIN_PATH" "$INSTALL_DIR/"
  echo "✓ Plugin installed! Restart Pro Tools to load it."
else
  echo "✗ Build failed - plugin not found at $PLUGIN_PATH"
  exit 1
fi
```

Save this as `build_and_install_aax.sh`, make it executable:
```bash
chmod +x build_and_install_aax.sh
```

Then run:
```bash
./build_and_install_aax.sh
```

## Notes

- **Development vs Release**: Use Debug builds for development/testing. Release builds are optimized but harder to debug.
- **Code Signing**: For distribution, AAX plugins need to be properly code-signed. For development/testing, this may not be required.
- **AAX SDK License**: Make sure you comply with Avid's licensing terms for AAX development.



