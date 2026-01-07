#!/bin/bash
# Build Release version of AAX plugin

echo "========================================="
echo "Building Release AAX Plugin"
echo "========================================="
echo ""

cd "/Users/christopherkalla/Software Projects/K-Factor/Builds/MacOSX"

echo "Building Release version..."
xcodebuild -project K-Factor.xcodeproj \
    -scheme "K-Factor - AAX" \
    -configuration Release \
    clean build \
    CODE_SIGN_IDENTITY="" \
    CODE_SIGNING_REQUIRED=NO \
    CODE_SIGNING_ALLOWED=NO 2>&1 | tail -20

PLUGIN_PATH="build/Release/K-Factor.aaxplugin"
if [ -d "$PLUGIN_PATH" ]; then
    echo ""
    echo "✓ Release build successful!"
    echo "Plugin at: $PLUGIN_PATH"
    echo ""
    echo "To install:"
    echo "  sudo cp -R \"$PLUGIN_PATH\" /Library/Application\\ Support/Avid/Audio/Plug-Ins/"
    echo "  rm ~/Library/Preferences/Avid/Pro\\ Tools/InstalledAAXPlugIns 2>/dev/null"
    echo "  (Then restart Pro Tools)"
else
    echo ""
    echo "❌ Build failed - plugin not found"
    exit 1
fi


