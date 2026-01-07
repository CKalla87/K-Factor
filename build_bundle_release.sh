#!/bin/bash
# Build Release Script for Plugin Bundle
# Builds both NebulaEQ and K-Factor in Release mode

set -e  # Exit on error

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Building Plugin Bundle (Release Mode) ===${NC}"
echo ""

# Get project directories
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
K_FACTOR_DIR="$SCRIPT_DIR"
NEBULAEQ_DIR="$(dirname "$SCRIPT_DIR")/NebulaEQ"

# Check if NebulaEQ exists
if [ ! -d "$NEBULAEQ_DIR" ]; then
    echo -e "${RED}Error: NebulaEQ directory not found at $NEBULAEQ_DIR${NC}"
    exit 1
fi

echo "K-Factor directory: $K_FACTOR_DIR"
echo "NebulaEQ directory: $NEBULAEQ_DIR"
echo ""

# Function to build a plugin
build_plugin() {
    local PLUGIN_NAME=$1
    local PLUGIN_DIR=$2
    local SCHEME=$3
    
    echo -e "${YELLOW}Building $PLUGIN_NAME...${NC}"
    
    cd "$PLUGIN_DIR/Builds/MacOSX"
    
    # Find the actual project file
    PROJECT_FILE=$(ls -d *.xcodeproj 2>/dev/null | head -1)
    if [ -z "$PROJECT_FILE" ]; then
        echo -e "  ${RED}✗ No Xcode project found${NC}"
        return
    fi
    
    # Get available schemes
    AVAILABLE_SCHEMES=$(xcodebuild -project "$PROJECT_FILE" -list 2>/dev/null | grep -A 50 "Schemes:" | grep -v "Schemes:" | xargs)
    
    # Build VST3
    if echo "$AVAILABLE_SCHEMES" | grep -q "VST3"; then
        echo "  Building VST3..."
        if xcodebuild -project "$PROJECT_FILE" \
            -scheme "${SCHEME} - VST3" \
            -configuration Release \
            clean build 2>&1 | grep -q "BUILD SUCCEEDED"; then
            echo -e "  ${GREEN}✓ VST3 built${NC}"
        else
            echo -e "  ${RED}✗ VST3 build failed${NC}"
        fi
    else
        echo -e "  ${YELLOW}⚠ VST3 scheme not found${NC}"
    fi
    
    # Build AU
    if echo "$AVAILABLE_SCHEMES" | grep -q "AU"; then
        echo "  Building AU..."
        if xcodebuild -project "$PROJECT_FILE" \
            -scheme "${SCHEME} - AU" \
            -configuration Release \
            clean build 2>&1 | grep -q "BUILD SUCCEEDED"; then
            echo -e "  ${GREEN}✓ AU built${NC}"
        else
            echo -e "  ${RED}✗ AU build failed${NC}"
        fi
    else
        echo -e "  ${YELLOW}⚠ AU scheme not found${NC}"
    fi
    
    # Build Standalone (check for various scheme names)
    STANDALONE_SCHEME=""
    if echo "$AVAILABLE_SCHEMES" | grep -q "Standalone Plugin"; then
        STANDALONE_SCHEME="${SCHEME} - Standalone Plugin"
    elif echo "$AVAILABLE_SCHEMES" | grep -q "Standalone"; then
        STANDALONE_SCHEME="${SCHEME} - Standalone"
    fi
    
    if [ -n "$STANDALONE_SCHEME" ]; then
        echo "  Building Standalone..."
        if xcodebuild -project "$PROJECT_FILE" \
            -scheme "$STANDALONE_SCHEME" \
            -configuration Release \
            clean build 2>&1 | grep -q "BUILD SUCCEEDED"; then
            echo -e "  ${GREEN}✓ Standalone built${NC}"
        else
            echo -e "  ${YELLOW}⚠ Standalone build failed${NC}"
        fi
    else
        echo -e "  ${YELLOW}⚠ Standalone scheme not found (skipping)${NC}"
    fi
    
    # Build AAX
    if echo "$AVAILABLE_SCHEMES" | grep -q "AAX"; then
        echo "  Building AAX..."
        if xcodebuild -project "$PROJECT_FILE" \
            -scheme "${SCHEME} - AAX" \
            -configuration Release \
            clean build 2>&1 | grep -q "BUILD SUCCEEDED"; then
            echo -e "  ${GREEN}✓ AAX built${NC}"
        else
            echo -e "  ${YELLOW}⚠ AAX build failed${NC}"
        fi
    else
        echo -e "  ${YELLOW}⚠ AAX scheme not found (skipping)${NC}"
    fi
    
    echo -e "  ${GREEN}✓ $PLUGIN_NAME build complete${NC}"
    echo ""
}

# Build K-Factor
build_plugin "K-Factor" "$K_FACTOR_DIR" "K-Factor"

# Build NebulaEQ (uses MyVSTPlugin as scheme name)
build_plugin "NebulaEQ" "$NEBULAEQ_DIR" "MyVSTPlugin"

echo -e "${GREEN}=== All plugins built successfully! ===${NC}"
echo ""
echo "Release builds are located in:"
echo "  K-Factor: $K_FACTOR_DIR/Builds/MacOSX/build/Release/"
echo "  NebulaEQ: $NEBULAEQ_DIR/Builds/MacOSX/build/Release/"
echo ""
echo "Next step: Run ./package_bundle.sh to create distribution package"

