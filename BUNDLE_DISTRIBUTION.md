# NebulaEQ & K-Factor Plugin Bundle - Distribution Guide

This guide explains how to build, package, and distribute the NebulaEQ & K-Factor plugin bundle for commercial distribution.

## Quick Start

1. **Build both plugins in Release mode:**
   ```bash
   ./build_bundle_release.sh
   ```

2. **Package the bundle:**
   ```bash
   ./package_bundle.sh
   ```

3. **Create DMG installer (optional):**
   ```bash
   ./create_dmg.sh
   ```

4. **Code sign plugins (recommended):**
   ```bash
   ./code_sign_bundle.sh
   ```

## Detailed Workflow

### Step 1: Build Release Versions

The `build_bundle_release.sh` script builds both plugins in Release mode:

```bash
./build_bundle_release.sh
```

This will:
- Build K-Factor in Release mode (VST3, AU, Standalone, AAX if configured)
- Build NebulaEQ in Release mode (VST3, AU, Standalone, AAX if configured)
- Output builds to `Builds/MacOSX/build/Release/` in each project directory

**Note:** Make sure both projects are properly configured in Projucer before building.

### Step 2: Package the Bundle

The `package_bundle.sh` script creates a distribution package:

```bash
./package_bundle.sh
```

You'll be prompted for:
- Version number (e.g., 1.0.0)

This creates:
- `dist/NebulaEQ_K-Factor_Bundle_v[VERSION]/`
  - `macOS/` folder with all plugin formats
  - `README.txt` with installation instructions
  - `LICENSE.txt` (placeholder - add your license)
  - `macOS/install.sh` - automated installation script

### Step 3: Create DMG Installer (macOS)

Create a professional DMG installer:

```bash
./create_dmg.sh
```

This will:
- Find the latest distribution package
- Create a DMG file: `dist/NebulaEQ_K-Factor_Bundle_v[VERSION].dmg`
- Include installation instructions
- Ready for distribution

### Step 4: Code Signing (Recommended)

For macOS distribution, code signing is highly recommended:

```bash
./code_sign_bundle.sh
```

You'll need:
- A valid Apple Developer account
- Code signing certificate installed in Keychain
- Signing identity (e.g., "Developer ID Application: Your Name")

**To get your signing identity:**
```bash
security find-identity -v -p codesigning
```

### Step 5: Notarization (Optional but Recommended)

For distribution outside the App Store, notarization is recommended:

1. **Create a zip of your signed DMG:**
   ```bash
   cd dist
   zip NebulaEQ_K-Factor_Bundle_v1.0.0.dmg.zip NebulaEQ_K-Factor_Bundle_v1.0.0.dmg
   ```

2. **Submit for notarization:**
   ```bash
   xcrun notarytool submit NebulaEQ_K-Factor_Bundle_v1.0.0.dmg.zip \
     --keychain-profile "AC_PASSWORD" \
     --wait
   ```

3. **Staple the ticket:**
   ```bash
   xcrun stapler staple NebulaEQ_K-Factor_Bundle_v1.0.0.dmg
   ```

## Distribution Package Structure

```
dist/NebulaEQ_K-Factor_Bundle_v1.0.0/
├── macOS/
│   ├── NebulaEQ/
│   │   ├── NebulaEQ.vst3
│   │   ├── NebulaEQ.component
│   │   ├── NebulaEQ.aaxplugin (if built)
│   │   └── NebulaEQ.app (if built)
│   ├── K-Factor/
│   │   ├── K-Factor.vst3
│   │   ├── K-Factor.component
│   │   ├── K-Factor.aaxplugin (if built)
│   │   └── K-Factor.app (if built)
│   └── install.sh
├── README.txt
└── LICENSE.txt
```

## Installation Instructions for End Users

The package includes a comprehensive README.txt with installation instructions. Users can:

1. **Manual Installation:**
   - Copy plugins to appropriate directories (see README.txt)

2. **Automated Installation:**
   - Run `macOS/install.sh` script

3. **DMG Installation:**
   - Open DMG
   - Run install.sh or drag plugins to appropriate locations

## Plugin Formats Included

### VST3
- **Location:** `~/Library/Audio/Plug-Ins/VST3/` (user) or `/Library/Audio/Plug-Ins/VST3/` (system)
- **Compatibility:** Works in most DAWs (Ableton Live, Reaper, Studio One, etc.)

### AU (Audio Unit)
- **Location:** `~/Library/Audio/Plug-Ins/Components/` (user) or `/Library/Audio/Plug-Ins/Components/` (system)
- **Compatibility:** Native macOS format (Logic Pro, GarageBand, etc.)

### AAX (Pro Tools)
- **Location:** `~/Library/Application Support/Avid/Audio/Plug-Ins/` (user) or `/Library/Application Support/Avid/Audio/Plug-Ins/` (system)
- **Compatibility:** Pro Tools
- **Note:** Pro Tools Ultimate requires code-signed plugins

### Standalone
- **Location:** Anywhere (Applications folder recommended)
- **Compatibility:** Runs as independent application

## Pre-Distribution Checklist

- [ ] Both plugins built in Release mode
- [ ] All plugin formats tested
- [ ] Version numbers updated in Projucer
- [ ] README.txt reviewed and accurate
- [ ] LICENSE.txt added with your license terms
- [ ] Plugins code signed (recommended)
- [ ] DMG created and tested
- [ ] Tested on clean machine (without development tools)
- [ ] Tested in multiple DAWs
- [ ] Tested on different macOS versions (if possible)

## Version Management

Update version numbers in:
1. **Projucer files:**
   - `K-Factor.jucer`
   - `NebulaEQ.jucer`
   - Set version in project settings

2. **When packaging:**
   - Enter version when prompted by `package_bundle.sh`

3. **Version format:**
   - Recommended: `MAJOR.MINOR.PATCH` (e.g., 1.0.0)

## Troubleshooting

### Build Errors
- Ensure both projects are properly configured in Projucer
- Check that JUCE paths are correct
- Verify Xcode project files are up to date (regenerate in Projucer if needed)

### Missing Plugins
- Check that Release builds completed successfully
- Verify plugin formats are enabled in Projucer
- Check build logs for errors

### Code Signing Issues
- Verify certificate is installed in Keychain
- Check signing identity matches exactly
- Ensure certificate hasn't expired

### DMG Creation Issues
- Ensure distribution package exists
- Check disk space
- Verify hdiutil is available (standard on macOS)

## Distribution Platforms

Consider distributing through:
- **Your own website** - Full control
- **GitHub Releases** - Free for open source
- **Plugin Boutique** - Commercial marketplace
- **KVR Audio** - Plugin database and marketplace
- **Direct sales** - Your own e-commerce

## Support

Include support information in:
- README.txt
- Your website
- Plugin documentation

## Legal Considerations

- **License:** Add your license terms to LICENSE.txt
- **Copyright:** Update copyright notices in Projucer
- **Trademarks:** Ensure plugin names don't conflict with existing trademarks
- **Terms of Service:** Consider adding EULA for commercial distribution

## Next Steps

1. Test the complete workflow
2. Set up automated builds (CI/CD if desired)
3. Create marketing materials
4. Set up distribution platform
5. Prepare user documentation
6. Plan update/release strategy

---

**Good luck with your plugin distribution!** 🎵

