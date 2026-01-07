# Setting JUCE Module Paths in Projucer

## Method 1: Global JUCE Path (Recommended)

1. **Open JUCE Projucer**
2. **Go to File → Global Paths...** (or **Projucer → Settings → Global Paths** on Mac)
3. Set the **JUCE Modules** path:
   - Click the folder icon next to "JUCE Modules"
   - Navigate to your JUCE installation
   - Select the `modules` folder inside your JUCE directory
   - For example: `/Users/christopherkalla/Software Projects/NebulaEQ/JUCE/modules`
4. Click **OK**

Now all your projects will use this path by default.

## Method 2: Per-Project Paths

If you want to set paths for just this project:

1. **Open your .jucer file** in Projucer
2. **Click on "K-Factor"** in the left sidebar (the main project node)
3. **Look at the right panel** - you'll see project settings
4. **Scroll down to "Module Paths"** section
5. For each module, you can set a custom path or leave it as "default" (which uses Global Paths)

## Method 3: Using Relative Paths in .jucer File

The .jucer file uses XML to define paths. You can edit it directly or use Projucer's GUI. The paths look like:
```xml
<MODULEPATH id="juce_dsp" path="../NebulaEQ/JUCE/modules"/>
```

## Quick Setup for K-Factor

Since you have JUCE in NebulaEQ, you have two options:

### Option A: Use Global Paths (Easiest)
1. Open Projucer
2. File → Global Paths
3. Set JUCE Modules to: `/Users/christopherkalla/Software Projects/NebulaEQ/JUCE/modules`
4. Save
5. Open K-Factor.jucer
6. Click "Save and Open in IDE"

### Option B: Recreate .jucer File
If the .jucer file was deleted, you can:
1. Open Projucer
2. File → New Project → Audio Plug-in
3. Configure:
   - Name: "K-Factor"
   - Plugin Format: VST3, AU, Standalone
   - Plugin Type: Effect
4. Add your Source files:
   - Right-click "Source" folder → Add Existing Files
   - Select PluginProcessor.h, PluginProcessor.cpp, PluginEditor.h, PluginEditor.cpp
5. Set module paths (use Global Paths or set per-project)
6. Save as K-Factor.jucer

## Verifying Paths

After setting paths:
1. In Projucer, click on "Modules" in the left sidebar
2. You should see all JUCE modules listed
3. If any show an error (red), the path is wrong

## Common Issues

- **"Module not found"**: Check that the path points to the `modules` folder, not the JUCE root
- **"Invalid path"**: Make sure the path exists and you have read permissions
- **Relative paths not working**: Use absolute paths or set Global Paths

Once paths are set correctly, click "Save and Open in IDE" to generate your Xcode/Visual Studio project!


