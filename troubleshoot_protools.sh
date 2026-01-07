#!/bin/bash
# Comprehensive Pro Tools plugin troubleshooting

echo "========================================="
echo "Pro Tools Plugin Troubleshooting"
echo "========================================="
echo ""

PLUGIN_PATH="$HOME/Library/Application Support/Avid/Audio/Plug-Ins/K-Factor.aaxplugin"
SYSTEM_PLUGIN_PATH="/Library/Application Support/Avid/Audio/Plug-Ins/K-Factor.aaxplugin"

# Check if plugin exists
echo "1. Checking plugin installation..."
if [ -d "$PLUGIN_PATH" ]; then
    echo "   ✓ Plugin found in user directory: $PLUGIN_PATH"
    USER_INSTALLED=true
else
    echo "   ✗ Plugin NOT found in user directory"
    USER_INSTALLED=false
fi

if [ -d "$SYSTEM_PLUGIN_PATH" ]; then
    echo "   ✓ Plugin found in system directory: $SYSTEM_PLUGIN_PATH"
    SYSTEM_INSTALLED=true
else
    echo "   ✗ Plugin NOT found in system directory"
    SYSTEM_INSTALLED=false
fi

if [ "$USER_INSTALLED" = false ] && [ "$SYSTEM_INSTALLED" = false ]; then
    echo ""
    echo "❌ Plugin is not installed! Please install it first."
    exit 1
fi

echo ""
echo "2. Checking plugin structure..."
if [ -f "$PLUGIN_PATH/Contents/MacOS/K-Factor" ]; then
    echo "   ✓ Executable found"
    file "$PLUGIN_PATH/Contents/MacOS/K-Factor"
else
    echo "   ✗ Executable NOT found"
fi

echo ""
echo "3. Checking plugin Info.plist..."
if [ -f "$PLUGIN_PATH/Contents/Info.plist" ]; then
    echo "   ✓ Info.plist found"
    echo "   Bundle Name: $(/usr/libexec/PlistBuddy -c 'Print :CFBundleName' "$PLUGIN_PATH/Contents/Info.plist" 2>/dev/null)"
    echo "   Bundle ID: $(/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' "$PLUGIN_PATH/Contents/Info.plist" 2>/dev/null)"
else
    echo "   ✗ Info.plist NOT found"
fi

echo ""
echo "4. Checking Pro Tools cache..."
CACHE_FILE="$HOME/Library/Preferences/Avid/Pro Tools/InstalledAAXPlugIns"
if [ -f "$CACHE_FILE" ]; then
    echo "   ✓ Cache file exists"
    if grep -q "K-Factor\|KFactor" "$CACHE_FILE" 2>/dev/null; then
        echo "   ✓ Plugin found in cache"
    else
        echo "   ⚠ Plugin NOT in cache (may need rescan)"
    fi
else
    echo "   ℹ No cache file (Pro Tools will rescan on next launch)"
fi

echo ""
echo "5. Checking for Pro Tools processes..."
if pgrep -x "Pro Tools" > /dev/null; then
    echo "   ⚠ Pro Tools is currently running"
    echo "   → Please quit Pro Tools before continuing"
else
    echo "   ✓ Pro Tools is not running"
fi

echo ""
echo "========================================="
echo "Recommendations:"
echo "========================================="
echo ""

if [ "$SYSTEM_INSTALLED" = false ]; then
    echo "→ Try installing to system directory:"
    echo "  sudo cp -R \"$PLUGIN_PATH\" /Library/Application\\ Support/Avid/Audio/Plug-Ins/"
    echo ""
fi

echo "→ Clear cache and rescan:"
echo "  rm \"$CACHE_FILE\" 2>/dev/null; echo 'Cache cleared'"
echo "  (Then restart Pro Tools)"
echo ""

echo "→ Check Console.app for errors:"
echo "  Open Console.app → Filter for 'Pro Tools' or 'AAX'"
echo "  Look for any errors related to K-Factor"
echo ""

echo "→ Try searching in Pro Tools:"
echo "  In Pro Tools, use the plugin search/filter"
echo "  Type 'K-Factor' or 'KFactor'"
echo ""

echo "→ Check all plugin categories:"
echo "  Look in: Multi-Channel Plug-In →"
echo "  - Dynamics"
echo "  - Effect"
echo "  - All Plug-Ins"
echo ""



