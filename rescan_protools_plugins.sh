#!/bin/bash
# Force Pro Tools to rescan AAX plugins

echo "========================================="
echo "Pro Tools Plugin Rescan Script"
echo "========================================="
echo ""

# Check if plugin exists
PLUGIN_PATH="$HOME/Library/Application Support/Avid/Audio/Plug-Ins/K-Factor.aaxplugin"
if [ ! -d "$PLUGIN_PATH" ]; then
    echo "❌ Plugin not found at: $PLUGIN_PATH"
    echo "   Please install the plugin first"
    exit 1
fi

echo "✓ Plugin found at: $PLUGIN_PATH"
echo ""

# Remove Pro Tools plugin cache
CACHE_FILE="$HOME/Library/Preferences/Avid/Pro Tools/InstalledAAXPlugIns"
if [ -f "$CACHE_FILE" ]; then
    echo "Removing Pro Tools plugin cache..."
    rm "$CACHE_FILE"
    echo "✓ Cache file removed: $CACHE_FILE"
else
    echo "ℹ No cache file found (this is okay)"
fi

echo ""
echo "========================================="
echo "Next Steps:"
echo "========================================="
echo ""
echo "1. Make sure Pro Tools is COMPLETELY QUIT"
echo "   (Check Activity Monitor if needed)"
echo ""
echo "2. Launch Pro Tools"
echo "   (It will automatically rescan all plugins)"
echo ""
echo "3. Look for K-Factor in:"
echo "   Multi-Channel Plug-In → Dynamics → K-Factor"
echo ""
echo "If the plugin still doesn't appear:"
echo "- Check Console.app for Pro Tools errors"
echo "- Try installing to system directory:"
echo "  sudo cp -R \"$PLUGIN_PATH\" /Library/Application\\ Support/Avid/Audio/Plug-Ins/"
echo ""



