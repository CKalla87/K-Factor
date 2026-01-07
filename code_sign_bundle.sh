#!/bin/bash
# Code Sign Plugin Bundle
# Signs all plugins in the distribution package

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Code Sign Plugin Bundle ===${NC}"
echo ""

# Get signing identity
read -p "Enter your code signing identity (e.g., 'Developer ID Application: Your Name'): " SIGNING_IDENTITY

if [ -z "$SIGNING_IDENTITY" ]; then
    echo -e "${RED}Error: Signing identity required${NC}"
    echo "Example: 'Developer ID Application: Your Name'"
    exit 1
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUNDLE_NAME="NebulaEQ_K-Factor_Bundle"

# Find the latest distribution directory
LATEST_DIST=$(ls -td dist/${BUNDLE_NAME}_v* 2>/dev/null | head -1)

if [ -z "$LATEST_DIST" ]; then
    echo -e "${RED}Error: No distribution package found${NC}"
    echo "Run ./package_bundle.sh first"
    exit 1
fi

echo "Signing plugins in: $LATEST_DIST"
echo ""

# Function to sign a plugin
sign_plugin() {
    local PLUGIN_PATH=$1
    local PLUGIN_NAME=$(basename "$PLUGIN_PATH")
    
    if [ ! -e "$PLUGIN_PATH" ]; then
        return
    fi
    
    echo -e "${YELLOW}Signing $PLUGIN_NAME...${NC}"
    codesign --force --deep --sign "$SIGNING_IDENTITY" "$PLUGIN_PATH"
    
    # Verify signature
    if codesign --verify --verbose "$PLUGIN_PATH" 2>/dev/null; then
        echo -e "  ${GREEN}✓ $PLUGIN_NAME signed successfully${NC}"
    else
        echo -e "  ${RED}✗ $PLUGIN_NAME signing failed${NC}"
    fi
}

# Sign all plugins
echo "Signing K-Factor plugins..."
if [ -d "$LATEST_DIST/macOS/K-Factor" ]; then
    cd "$LATEST_DIST/macOS/K-Factor"
    
    sign_plugin "K-Factor.vst3"
    sign_plugin "K-Factor.component"
    sign_plugin "K-Factor.aaxplugin"
    sign_plugin "K-Factor.app"
fi

echo ""
echo "Signing NebulaEQ plugins..."
if [ -d "$LATEST_DIST/macOS/NebulaEQ" ]; then
    cd "$LATEST_DIST/macOS/NebulaEQ"
    
    sign_plugin "NebulaEQ.vst3"
    sign_plugin "NebulaEQ.component"
    sign_plugin "NebulaEQ.aaxplugin"
    sign_plugin "NebulaEQ.app"
fi

echo ""
echo -e "${GREEN}=== Code signing complete! ===${NC}"
echo ""
echo "All plugins have been signed with: $SIGNING_IDENTITY"
echo ""
echo "To verify signatures, run:"
echo "  codesign --verify --verbose [plugin_path]"

