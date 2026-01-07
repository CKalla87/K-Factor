# AAX SDK Setup Instructions

## Step 1: Extract the AAX SDK

1. **Download the files**:
   - `AAX SDK 2.9.0` (zip file)
   - `AAX Developer Tools Beta 22.R4.0.1 arm64 (Mac)` (zip file) - optional but recommended

2. **Create a directory for the AAX SDK**:
   ```bash
   mkdir -p ~/AAX_SDK
   ```

3. **Extract the AAX SDK**:
   - Double-click the `AAX SDK 2.9.0.zip` file, or
   - Extract it to `~/AAX_SDK/` using:
     ```bash
     cd ~/Downloads
     unzip "AAX SDK 2.9.0.zip" -d ~/AAX_SDK/
     ```

4. **Verify the extraction**:
   After extraction, you should have a structure like:
   ```
   ~/AAX_SDK/
   └── AAX_SDK_2.9.0/
       ├── AAX_SDK/
       │   ├── Interfaces/
       │   ├── Libs/
       │   ├── Utilities/
       │   └── ...
       └── ...
   ```

   The important path is: `~/AAX_SDK/AAX_SDK_2.9.0/AAX_SDK/`

## Step 2: Extract AAX Developer Tools (Optional)

1. **Extract the Developer Tools**:
   ```bash
   cd ~/Downloads
   unzip "AAX Developer Tools Beta 22.R4.0.1 arm64 (Mac).zip" -d ~/AAX_SDK/
   ```

2. **This will add developer utilities** (not required for basic building, but useful for testing)

## Step 3: Configure Projucer

1. **Open Projucer**:
   - Launch JUCE Projucer application

2. **Set the AAX SDK Path**:
   - Go to **Projucer → Global Paths** (on Mac) or **File → Global Paths** (on Windows)
   - Find the **AAX SDK** field
   - Click the folder icon or browse button
   - Navigate to and select: `~/AAX_SDK/AAX_SDK_2.9.0/AAX_SDK/`
     - Or the full path: `/Users/christopherkalla/AAX_SDK/AAX_SDK_2.9.0/AAX_SDK/`

3. **Verify the path**:
   - The path should point to the directory that contains:
     - `Interfaces/` folder
     - `Libs/` folder
     - `Utilities/` folder

## Step 4: Configure Your Project

1. **Open your project in Projucer**:
   ```bash
   open K-Factor.jucer
   ```
   (Or open it from within Projucer)

2. **Verify AAX is enabled**:
   - In the main Projucer window, look for **Plugin Formats** section
   - Make sure **AAX** checkbox is checked/enabled
   - (The project file has already been updated to include AAX)

3. **Save and regenerate**:
   - Click **Save Project**
   - Click **Save and Open in IDE** to regenerate the Xcode project with AAX support

## Step 5: Verify Setup

After regenerating, you should see:
- A new **"K-Factor - AAX"** scheme in Xcode
- The AAX target should build without SDK path errors

## Troubleshooting

### "AAX SDK not found" error

- Make sure you're pointing to the `AAX_SDK/` subdirectory, not the version folder
- Correct: `~/AAX_SDK/AAX_SDK_2.9.0/AAX_SDK/`
- Wrong: `~/AAX_SDK/AAX_SDK_2.9.0/`

### Can't find the AAX SDK path in Projucer

- Make sure you're using a recent version of JUCE Projucer
- The path might be under **File → Global Paths** instead of **Projucer → Global Paths**

### Build errors about missing AAX files

- Verify the SDK path in Projucer is correct
- Check that `Interfaces/` and `Libs/` folders exist in the SDK directory
- Try regenerating the project (Save and Open in IDE again)

## Quick Setup Script

You can also use this script to set up the SDK automatically:

```bash
#!/bin/bash
# setup_aax_sdk.sh

SDK_DIR="$HOME/AAX_SDK"
SDK_ZIP="$HOME/Downloads/AAX SDK 2.9.0.zip"

if [ ! -f "$SDK_ZIP" ]; then
    echo "❌ AAX SDK zip file not found at: $SDK_ZIP"
    echo "   Please download it first and place it in ~/Downloads/"
    exit 1
fi

echo "Setting up AAX SDK..."
mkdir -p "$SDK_DIR"
cd "$SDK_DIR"
unzip -q "$SDK_ZIP"

echo "✓ AAX SDK extracted to: $SDK_DIR"
echo ""
echo "Next steps:"
echo "1. Open Projucer"
echo "2. Go to Projucer → Global Paths"
echo "3. Set AAX SDK path to: $SDK_DIR/AAX_SDK_2.9.0/AAX_SDK/"
echo "4. Open K-Factor.jucer and click 'Save and Open in IDE'"
```



