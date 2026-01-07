#!/bin/bash
# Package Plugin Bundle Script
# Packages both NebulaEQ and K-Factor for distribution

set -e  # Exit on error

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Plugin Bundle Packaging Script ===${NC}"
echo ""

# Get project directories
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
K_FACTOR_DIR="$SCRIPT_DIR"
NEBULAEQ_DIR="$(dirname "$SCRIPT_DIR")/NebulaEQ"
BUNDLE_NAME="NebulaEQ_K-Factor_Bundle"

# Get version
read -p "Enter version number (e.g., 1.0.0): " VERSION
VERSION=${VERSION:-1.0.0}

DIST_DIR="dist/${BUNDLE_NAME}_v${VERSION}"
BUILD_DIR_K="Builds/MacOSX/build/Release"
BUILD_DIR_N="$(dirname "$SCRIPT_DIR")/NebulaEQ/Builds/MacOSX/build/Release"

# Check if Release builds exist (warn but continue if some are missing)
if [ ! -d "$K_FACTOR_DIR/$BUILD_DIR_K" ]; then
    echo -e "${YELLOW}Warning: K-Factor Release build directory not found${NC}"
    echo "Run ./build_bundle_release.sh first"
    echo "Continuing anyway to package what's available..."
    mkdir -p "$K_FACTOR_DIR/$BUILD_DIR_K"
fi

if [ ! -d "$NEBULAEQ_DIR/$BUILD_DIR_N" ]; then
    echo -e "${YELLOW}Warning: NebulaEQ Release build directory not found${NC}"
    echo "Run ./build_bundle_release.sh first"
    echo "Continuing anyway to package what's available..."
    mkdir -p "$NEBULAEQ_DIR/$BUILD_DIR_N"
fi

# Create distribution directory structure (user-friendly drag-and-drop)
echo "Creating distribution directory..."
rm -rf "$DIST_DIR"
mkdir -p "${DIST_DIR}/VST3"
mkdir -p "${DIST_DIR}/AU"
mkdir -p "${DIST_DIR}/AAX"
mkdir -p "${DIST_DIR}/Standalone"

# Copy K-Factor plugins
echo ""
echo -e "${BLUE}Packaging K-Factor...${NC}"
cd "$K_FACTOR_DIR"

if [ -d "$BUILD_DIR_K/K-Factor.vst3" ]; then
    cp -R "$BUILD_DIR_K/K-Factor.vst3" "${DIST_DIR}/VST3/"
    echo "  ✓ K-Factor VST3"
else
    echo -e "  ${YELLOW}⚠ K-Factor VST3 not found${NC}"
fi

if [ -d "$BUILD_DIR_K/K-Factor.component" ]; then
    cp -R "$BUILD_DIR_K/K-Factor.component" "${DIST_DIR}/AU/"
    echo "  ✓ K-Factor AU"
else
    echo -e "  ${YELLOW}⚠ K-Factor AU not found${NC}"
fi

if [ -d "$BUILD_DIR_K/K-Factor.aaxplugin" ]; then
    cp -R "$BUILD_DIR_K/K-Factor.aaxplugin" "${DIST_DIR}/AAX/"
    echo "  ✓ K-Factor AAX"
else
    echo -e "  ${YELLOW}⚠ K-Factor AAX not found${NC}"
fi

if [ -d "$BUILD_DIR_K/K-Factor.app" ]; then
    cp -R "$BUILD_DIR_K/K-Factor.app" "${DIST_DIR}/Standalone/"
    echo "  ✓ K-Factor Standalone"
fi

# Copy NebulaEQ plugins (check for both NebulaEQ and MyVSTPlugin names)
echo ""
echo -e "${BLUE}Packaging NebulaEQ...${NC}"
cd "$NEBULAEQ_DIR"

# Try NebulaEQ name first, then MyVSTPlugin
NEBULA_VST3=""
NEBULA_AU=""
NEBULA_AAX=""
NEBULA_APP=""

if [ -d "$BUILD_DIR_N/NebulaEQ.vst3" ]; then
    NEBULA_VST3="NebulaEQ.vst3"
elif [ -d "$BUILD_DIR_N/MyVSTPlugin.vst3" ]; then
    NEBULA_VST3="MyVSTPlugin.vst3"
fi

if [ -d "$BUILD_DIR_N/NebulaEQ.component" ]; then
    NEBULA_AU="NebulaEQ.component"
elif [ -d "$BUILD_DIR_N/MyVSTPlugin.component" ]; then
    NEBULA_AU="MyVSTPlugin.component"
fi

if [ -d "$BUILD_DIR_N/NebulaEQ.aaxplugin" ]; then
    NEBULA_AAX="NebulaEQ.aaxplugin"
elif [ -d "$BUILD_DIR_N/MyVSTPlugin.aaxplugin" ]; then
    NEBULA_AAX="MyVSTPlugin.aaxplugin"
fi

if [ -d "$BUILD_DIR_N/NebulaEQ.app" ]; then
    NEBULA_APP="NebulaEQ.app"
elif [ -d "$BUILD_DIR_N/MyVSTPlugin.app" ]; then
    NEBULA_APP="MyVSTPlugin.app"
fi

# Copy and rename to NebulaEQ
if [ -n "$NEBULA_VST3" ]; then
    cp -R "$BUILD_DIR_N/$NEBULA_VST3" "${DIST_DIR}/VST3/NebulaEQ.vst3"
    echo "  ✓ NebulaEQ VST3"
else
    echo -e "  ${YELLOW}⚠ NebulaEQ VST3 not found${NC}"
fi

if [ -n "$NEBULA_AU" ]; then
    cp -R "$BUILD_DIR_N/$NEBULA_AU" "${DIST_DIR}/AU/NebulaEQ.component"
    echo "  ✓ NebulaEQ AU"
else
    echo -e "  ${YELLOW}⚠ NebulaEQ AU not found${NC}"
fi

if [ -n "$NEBULA_AAX" ]; then
    cp -R "$BUILD_DIR_N/$NEBULA_AAX" "${DIST_DIR}/AAX/NebulaEQ.aaxplugin"
    echo "  ✓ NebulaEQ AAX"
else
    echo -e "  ${YELLOW}⚠ NebulaEQ AAX not found${NC}"
fi

if [ -n "$NEBULA_APP" ]; then
    cp -R "$BUILD_DIR_N/$NEBULA_APP" "${DIST_DIR}/Standalone/NebulaEQ.app"
    echo "  ✓ NebulaEQ Standalone"
fi

# Create simple README with drag-and-drop instructions
echo ""
echo "Creating README..."
cat > "${DIST_DIR}/README.txt" << EOF
NebulaEQ & K-Factor Plugin Bundle v${VERSION}
============================================

SIMPLE INSTALLATION - DRAG AND DROP
------------------------------------

Just drag the plugins from the folders to your plugin directories:

1. VST3 Plugins:
   - Open the "VST3" folder
   - Drag all .vst3 files to:
     ~/Library/Audio/Plug-Ins/VST3/
   
   (Or press Cmd+Shift+G in Finder, paste: ~/Library/Audio/Plug-Ins/VST3/)

2. AU Plugins:
   - Open the "AU" folder
   - Drag all .component files to:
     ~/Library/Audio/Plug-Ins/Components/
   
   (Or press Cmd+Shift+G in Finder, paste: ~/Library/Audio/Plug-Ins/Components/)

3. AAX Plugins (Pro Tools):
   - Open the "AAX" folder
   - Drag all .aaxplugin files to:
     ~/Library/Application Support/Avid/Audio/Plug-Ins/
   
   (Or press Cmd+Shift+G in Finder, paste: ~/Library/Application Support/Avid/Audio/Plug-Ins/)

4. Standalone Apps:
   - Open the "Standalone" folder
   - Drag .app files to your Applications folder

5. After copying:
   - Restart your DAW
   - Rescan plugins in your DAW's preferences

PLUGIN DESCRIPTIONS
-------------------

NebulaEQ:
A powerful parametric equalizer with intuitive controls and professional sound quality.

K-Factor:
A fast-attack FET-style compressor with vintage character and modern features.
Features include:
- Ultra-fast attack times (0.01ms - 100ms)
- Full compressor controls (Threshold, Ratio, Attack, Release, Makeup Gain)
- Professional-grade audio processing

SYSTEM REQUIREMENTS
-------------------

- macOS 10.13 or later
- Compatible DAW:
  * Logic Pro X
  * Ableton Live
  * Pro Tools
  * Reaper
  * Studio One
  * FL Studio
  * And other VST3/AU compatible hosts

TROUBLESHOOTING
---------------

If plugins don't appear in your DAW:

1. Make sure you copied plugins to the correct folders
2. Restart your DAW completely
3. Rescan plugins in your DAW's preferences
4. Check System Preferences → Security & Privacy → General
   (may need to allow unsigned plugins)

For AAX plugins in Pro Tools:
- Pro Tools Ultimate requires code-signed plugins
- Pro Tools Dev/First allows unsigned plugins for development

SUPPORT
-------

For support, visit: www.example.com

Copyright © 2025 Your Company
All rights reserved.

EOF

echo "  ✓ README created"

# Create LICENSE placeholder
cat > "${DIST_DIR}/LICENSE.txt" << EOF
NebulaEQ & K-Factor Plugin Bundle
Copyright © 2025 Your Company

All rights reserved.

[Add your license terms here]
EOF

echo "  ✓ LICENSE created"

echo ""
echo -e "${GREEN}✓ Packaging complete!${NC}"
echo ""
echo "Distribution package: ${DIST_DIR}"
echo ""
echo "Package contents:"
echo "  - VST3/ (all VST3 plugins)"
echo "  - AU/ (all AU plugins)"
echo "  - AAX/ (all AAX plugins)"
echo "  - Standalone/ (standalone apps)"
echo "  - README.txt (simple drag-and-drop instructions)"
echo "  - LICENSE.txt"
echo ""
echo "Users can simply:"
echo "  1. Unzip the package"
echo "  2. Drag plugins from folders to their plugin directories"
echo "  3. Restart their DAW"
echo ""
echo "Next steps:"
echo "1. Test the package on a clean machine"
echo "2. Code sign the plugins (recommended for macOS distribution)"
echo "3. Create DMG installer: ./create_dmg.sh"
echo "4. Upload to your distribution platform"

