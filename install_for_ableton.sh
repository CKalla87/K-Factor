#!/bin/bash
# Install K-Factor plugin for Ableton Live

echo "========================================="
echo "Installing K-Factor for Ableton Live"
echo "========================================="
echo ""

PLUGIN_SOURCE="/Users/christopherkalla/Software Projects/K-Factor/Builds/MacOSX/build/Debug"

# VST3 location for Ableton Live
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
VST3_PLUGIN="K-Factor.vst3"

# AU location (if we build it)
AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
AU_PLUGIN="K-Factor.component"

echo "Installing VST3 plugin..."
mkdir -p "$VST3_DIR"

if [ -d "$PLUGIN_SOURCE/$VST3_PLUGIN" ]; then
    # Remove old version if exists
    if [ -d "$VST3_DIR/$VST3_PLUGIN" ]; then
        echo "Removing existing VST3 plugin..."
        rm -rf "$VST3_DIR/$VST3_PLUGIN"
    fi
    
    # Copy VST3
    cp -R "$PLUGIN_SOURCE/$VST3_PLUGIN" "$VST3_DIR/"
    echo "✓ VST3 installed to: $VST3_DIR/$VST3_PLUGIN"
else
    echo "✗ VST3 plugin not found. Building it first..."
    echo "   In Xcode, select 'K-Factor - VST3' scheme and build (Cmd+B)"
    exit 1
fi

echo ""
echo "========================================="
echo "✓ Installation Complete!"
echo "========================================="
echo ""
echo "Next steps:"
echo "1. Open Ableton Live"
echo "2. Go to Preferences → Plug-Ins"
echo "3. Make sure 'Use VST3 Plug-In System Folders' is enabled"
echo "4. Click 'Rescan' if needed"
echo "5. Look for 'K-Factor' in your plugin browser"
echo ""
echo "Plugin location: $VST3_DIR/$VST3_PLUGIN"
echo ""


