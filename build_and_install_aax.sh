#!/bin/bash
# Build and install AAX plugin for Pro Tools

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/Builds/MacOSX"
PLUGIN_NAME="K-Factor.aaxplugin"
INSTALL_DIR="$HOME/Library/Application Support/Avid/Audio/Plug-Ins"

echo "========================================="
echo "K-Factor AAX Build & Install Script"
echo "========================================="
echo ""

# Check if Xcode project exists
if [ ! -d "$BUILD_DIR/K-Factor.xcodeproj" ]; then
    echo "❌ Error: Xcode project not found at $BUILD_DIR"
    echo "   Please open K-Factor.jucer in Projucer and click 'Save and Open in IDE' first"
    exit 1
fi

cd "$BUILD_DIR"

# Check if AAX scheme exists
echo "Checking for AAX target..."
if ! xcodebuild -list -project K-Factor.xcodeproj 2>/dev/null | grep -q "AAX"; then
    echo "⚠️  Warning: AAX target not found in Xcode project"
    echo "   You may need to:"
    echo "   1. Open K-Factor.jucer in Projucer"
    echo "   2. Set the AAX SDK path in Projucer → Global Paths"
    echo "   3. Enable AAX in Plugin Formats"
    echo "   4. Click 'Save and Open in IDE' to regenerate the project"
    echo ""
    read -p "Continue anyway? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Build configuration
CONFIG="${1:-Debug}"
SCHEME="K-Factor - AAX"

echo "Building $SCHEME in $CONFIG configuration..."
echo ""

# Build the plugin
xcodebuild -project K-Factor.xcodeproj \
    -scheme "$SCHEME" \
    -configuration "$CONFIG" \
    clean build \
    CODE_SIGN_IDENTITY="" \
    CODE_SIGNING_REQUIRED=NO \
    CODE_SIGNING_ALLOWED=NO

# Check if build succeeded
PLUGIN_PATH="build/$CONFIG/$PLUGIN_NAME"
if [ ! -d "$PLUGIN_PATH" ]; then
    echo ""
    echo "❌ Build failed - plugin not found at $PLUGIN_PATH"
    echo "   Check the build output above for errors"
    exit 1
fi

echo ""
echo "✓ Build successful!"
echo ""

# Install the plugin
echo "Installing plugin to: $INSTALL_DIR"
mkdir -p "$INSTALL_DIR"

# Remove old version if it exists
if [ -d "$INSTALL_DIR/$PLUGIN_NAME" ]; then
    echo "Removing existing plugin..."
    rm -rf "$INSTALL_DIR/$PLUGIN_NAME"
fi

# Copy new plugin
cp -R "$PLUGIN_PATH" "$INSTALL_DIR/"
echo "✓ Plugin installed!"
echo ""

# Verify installation
if [ -d "$INSTALL_DIR/$PLUGIN_NAME" ]; then
    echo "========================================="
    echo "✓ Installation complete!"
    echo "========================================="
    echo ""
    echo "Next steps:"
    echo "1. Quit Pro Tools if it's running"
    echo "2. Launch Pro Tools"
    echo "3. Create a new track and add K-Factor from the plugin menu"
    echo "   (Multi-Channel Plug-In → Dynamics → K-Factor)"
    echo ""
    echo "If the plugin doesn't appear, you may need to rescan:"
    echo "  rm ~/Library/Preferences/Avid/Pro\\ Tools/InstalledAAXPlugIns"
    echo "  (Then restart Pro Tools)"
else
    echo "❌ Installation failed - plugin not found in install directory"
    exit 1
fi



