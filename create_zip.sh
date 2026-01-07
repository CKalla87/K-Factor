#!/bin/bash
# Create ZIP Archive for Plugin Bundle
# Creates a simple zip file that users can unzip and drag plugins

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Create ZIP Archive ===${NC}"
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
ZIP_NAME="${BUNDLE_NAME}_v${VERSION}.zip"
ZIP_PATH="dist/${ZIP_NAME}"

echo "Found distribution: $LATEST_DIST"
echo "Version: $VERSION"
echo ""

# Remove old ZIP if it exists
if [ -f "$ZIP_PATH" ]; then
    echo "Removing old ZIP..."
    rm "$ZIP_PATH"
fi

# Create ZIP
echo "Creating ZIP archive..."
cd dist
zip -r "$ZIP_NAME" "$(basename "$LATEST_DIST")" > /dev/null
cd ..

echo ""
echo -e "${GREEN}✓ ZIP created successfully!${NC}"
echo ""
echo "ZIP location: $ZIP_PATH"
echo "ZIP size: $(du -h "$ZIP_PATH" | cut -f1)"
echo ""
echo "Users can simply:"
echo "  1. Download and unzip the file"
echo "  2. Open the folders (VST3, AU, AAX, Standalone)"
echo "  3. Drag plugins to their plugin directories"
echo "  4. Restart their DAW"
echo ""
echo "Next steps:"
echo "1. Test the ZIP on a clean machine"
echo "2. Code sign the plugins before zipping (recommended)"
echo "3. Upload to your distribution platform"

