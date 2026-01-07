#!/bin/bash
# Install K-Factor to system directory (requires admin password)

echo "========================================="
echo "Installing K-Factor to System Directory"
echo "========================================="
echo ""

PLUGIN_SOURCE="/Users/christopherkalla/Software Projects/K-Factor/Builds/MacOSX/build/Debug/K-Factor.aaxplugin"
SYSTEM_DIR="/Library/Application Support/Avid/Audio/Plug-Ins"

if [ ! -d "$PLUGIN_SOURCE" ]; then
    echo "❌ Plugin not found at: $PLUGIN_SOURCE"
    echo "   Please build the plugin first in Xcode"
    exit 1
fi

echo "Installing to: $SYSTEM_DIR"
echo "(This requires your admin password)"
echo ""

# Remove old version if exists
if [ -d "$SYSTEM_DIR/K-Factor.aaxplugin" ]; then
    echo "Removing existing plugin..."
    sudo rm -rf "$SYSTEM_DIR/K-Factor.aaxplugin"
fi

# Copy to system directory
sudo cp -R "$PLUGIN_SOURCE" "$SYSTEM_DIR/"

if [ -d "$SYSTEM_DIR/K-Factor.aaxplugin" ]; then
    echo ""
    echo "✓ Plugin installed to system directory!"
    echo ""
    echo "Next steps:"
    echo "1. Quit Pro Tools completely"
    echo "2. Clear cache: rm ~/Library/Preferences/Avid/Pro\\ Tools/InstalledAAXPlugIns"
    echo "3. Launch Pro Tools"
    echo "4. Look for K-Factor in: Multi-Channel Plug-In → Dynamics"
else
    echo "❌ Installation failed"
    exit 1
fi


