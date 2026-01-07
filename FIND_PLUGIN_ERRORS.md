# Finding Why K-Factor Doesn't Appear in Pro Tools

## Critical: Check Console.app for Errors

The plugin structure is correct and the category is set to Dynamics (2), so Pro Tools must be rejecting it for another reason. We need to see the error messages.

### Step 1: Open Console.app
1. Open **Console.app** (Applications → Utilities → Console)
2. In the search box (top right), type: **Pro Tools**
3. Make sure you're viewing "All Messages" (not just errors)

### Step 2: Launch Pro Tools and Watch
1. **Quit Pro Tools completely** (if running)
2. **Launch Pro Tools**
3. **Watch the Console** as Pro Tools starts up
4. Look for any messages containing:
   - `K-Factor`
   - `KFactor`
   - `com.YourCompany.KFactor`
   - `AAX`
   - `Failed`
   - `Error`
   - `Invalid`
   - `Rejected`

### Step 3: Common Issues to Look For

**Code Signing Issues:**
- "code signature invalid"
- "not properly signed"
- "adhoc signature"

**Validation Errors:**
- "plugin validation failed"
- "invalid plugin format"
- "AAX validation error"

**Loading Errors:**
- "failed to load"
- "could not instantiate"
- "missing symbol"
- "undefined symbol"

**Category/Role Issues:**
- "invalid category"
- "category mismatch"

### Step 4: Alternative - Check Pro Tools Logs Directly

Pro Tools also writes logs to:
```
~/Library/Logs/Avid/Pro Tools/
```

Look for files with recent timestamps and search for "K-Factor" or "AAX" errors.

### Step 5: Try AAX Validator (if available)

If you downloaded the AAX Developer Tools, you can use the validator:
```bash
# If you have the validator installed
/Path/To/AAXValidator K-Factor.aaxplugin
```

## What to Share

Please share:
1. **Any red error messages** from Console.app
2. **Any messages containing "K-Factor" or "AAX"**
3. **The exact Pro Tools version** you're using
4. **Whether you see the plugin in "All Plug-Ins"** category (not just Dynamics)

## Quick Test: Try "Effect" Category

The plugin might work better in the "Effect" category. Try changing the category to 8192 (Effect) temporarily to see if it appears:

In Projucer, change:
- `pluginAAXCategory="8192"` (Effect category)
- Or `JucePlugin_AAXCategory="8192"` in JUCEOPTIONS

Then rebuild and test.


