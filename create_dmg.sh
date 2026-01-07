#!/bin/bash
# Create DMG Installer for Plugin Bundle

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Create DMG Installer ===${NC}"
echo ""

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUNDLE_NAME="NebulaEQ_K-Factor_Bundle"

# Find the latest distribution directory
LATEST_DIST=$(ls -td dist/${BUNDLE_NAME}_v* 2>/dev/null | head -1)

if [ -z "$LATEST_DIST" ]; then
    echo -e "${RED}Error: No distribution package found${NC}"
    echo "Run ./package_bundle.sh first"
    exit 1
fi

VERSION=$(basename "$LATEST_DIST" | sed 's/.*_v//')
DMG_NAME="${BUNDLE_NAME}_v${VERSION}.dmg"
DMG_PATH="dist/${DMG_NAME}"

echo "Found distribution: $LATEST_DIST"
echo "Version: $VERSION"
echo ""

# Remove old DMG if it exists
if [ -f "$DMG_PATH" ]; then
    echo "Removing old DMG..."
    rm "$DMG_PATH"
fi

# Create a temporary directory for DMG contents
TEMP_DMG_DIR="dist/temp_dmg"
rm -rf "$TEMP_DMG_DIR"
mkdir -p "$TEMP_DMG_DIR"

# Copy distribution contents
echo "Preparing DMG contents..."
cp -R "$LATEST_DIST"/* "$TEMP_DMG_DIR/"

# Create Applications folder link (for standalone apps)
ln -s /Applications "$TEMP_DMG_DIR/Applications"

# Create VST3 installation folder link
VST3_TARGET="$HOME/Library/Audio/Plug-Ins/VST3"
mkdir -p "$VST3_TARGET"
ln -s "$VST3_TARGET" "$TEMP_DMG_DIR/Install VST3 Plugins Here"

# Create AU installation folder link
AU_TARGET="$HOME/Library/Audio/Plug-Ins/Components"
mkdir -p "$AU_TARGET"
ln -s "$AU_TARGET" "$TEMP_DMG_DIR/Install AU Plugins Here"

# Create AAX installation folder link
AAX_TARGET="$HOME/Library/Application Support/Avid/Audio/Plug-Ins"
mkdir -p "$AAX_TARGET"
ln -s "$AAX_TARGET" "$TEMP_DMG_DIR/Install AAX Plugins Here"

# Create simple instructions file
cat > "$TEMP_DMG_DIR/📖 INSTALL INSTRUCTIONS.txt" << EOF
SIMPLE INSTALLATION
===================

1. Drag plugins from the folders to the installation folders:

   VST3 Plugins:
   → Drag from "VST3" folder to "Install VST3 Plugins Here"
   
   AU Plugins:
   → Drag from "AU" folder to "Install AU Plugins Here"
   
   AAX Plugins (Pro Tools):
   → Drag from "AAX" folder to "Install AAX Plugins Here"
   
   Standalone Apps:
   → Drag from "Standalone" folder to "Applications"

2. Restart your DAW

3. Rescan plugins in your DAW's preferences

That's it! The plugins will appear in your DAW.

For more details, see README.txt
EOF

# Create DMG with custom layout
echo "Creating DMG..."
hdiutil create -volname "${BUNDLE_NAME} v${VERSION}" \
    -srcfolder "$TEMP_DMG_DIR" \
    -ov -format UDZO \
    -fs HFS+ \
    "$DMG_PATH"

# Mount the DMG to set custom view options
echo "Setting up DMG layout..."
DMG_MOUNT=$(hdiutil attach "$DMG_PATH" -nobrowse -noverify -noautoopen | grep "Volumes" | awk '{print $3}')

if [ -n "$DMG_MOUNT" ]; then
    # Set view options for better user experience
    osascript <<EOF
    tell application "Finder"
        tell disk "${BUNDLE_NAME} v${VERSION}"
            open
            set current view of container window to icon view
            set toolbar visible of container window to false
            set statusbar visible of container window to false
            set bounds of container window to {200, 200, 1000, 600}
            set viewOptions to the icon view options of container window
            set arrangement of viewOptions to not arranged
            set icon size of viewOptions to 64
            close
        end tell
    end tell
EOF
    
    # Unmount
    hdiutil detach "$DMG_MOUNT" > /dev/null
fi

# Clean up
rm -rf "$TEMP_DMG_DIR"

echo ""
echo -e "${GREEN}✓ DMG created successfully!${NC}"
echo ""
echo "DMG location: $DMG_PATH"
echo ""
echo "DMG size: $(du -h "$DMG_PATH" | cut -f1)"
echo ""
echo "Next steps:"
echo "1. Test the DMG on a clean machine"
echo "2. Code sign the DMG (optional but recommended):"
echo "   codesign --force --deep --sign \"Developer ID Application: Your Name\" \"$DMG_PATH\""
echo "3. Notarize the DMG (for distribution outside App Store)"
echo "4. Upload to your distribution platform"

