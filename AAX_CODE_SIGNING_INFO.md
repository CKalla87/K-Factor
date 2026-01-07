# AAX Plugin Code Signing Requirement

## The Problem

**Pro Tools Ultimate (commercial version) requires AAX plugins to be digitally signed** using PACE Anti-Piracy signing tools. Your plugin is currently built with an "adhoc" signature, which Pro Tools Ultimate will silently reject.

This is why:
- The plugin doesn't appear in Pro Tools
- There are no errors in Console (Pro Tools silently skips unsigned plugins)
- The plugin structure is correct but Pro Tools won't load it

## Solutions

### Option 1: Use Pro Tools Dev (Recommended for Development)

Pro Tools Dev (developer version) allows unsigned plugins for testing:

1. **Download Pro Tools Dev** from Avid Developer Connection:
   - Go to: https://developer.avid.com/
   - Download: "Pro Tools Dev" (not regular Pro Tools)
   - Install it (can coexist with regular Pro Tools)

2. **Test your plugin in Pro Tools Dev**:
   - Launch Pro Tools Dev instead of Pro Tools Ultimate
   - Your unsigned plugin should work there

### Option 2: Get Code Signing Tools (For Distribution)

If you need to test in Pro Tools Ultimate:

1. **Get PACE Signing Tools**:
   - Contact Avid Developer Support
   - Request access to PACE Anti-Piracy signing tools
   - These are provided free for AAX developers

2. **Sign your plugin**:
   - Use the PACE tools to sign your `.aaxplugin`
   - Then Pro Tools Ultimate will accept it

### Option 3: Test with Other Formats First

While you work on AAX signing:
- Test with **Standalone** plugin (works without signing)
- Test with **AU** or **VST3** in other DAWs
- Verify the plugin works correctly before dealing with AAX signing

## Quick Check: Do You Have Pro Tools Dev?

Check if you have Pro Tools Dev installed:
```bash
ls -la /Applications/ | grep -i "pro.*tools.*dev"
```

If you see "Pro Tools Dev.app", use that instead of regular Pro Tools!

## Next Steps

1. **For immediate testing**: Download and use Pro Tools Dev
2. **For distribution**: Contact Avid to get PACE signing tools
3. **For now**: Test your plugin as Standalone or AU/VST3 to verify it works

## References

- Avid Developer Connection: https://developer.avid.com/
- AAX SDK Documentation mentions code signing requirements
- JUCE README mentions: "AAX plug-ins need to be digitally signed using PACE Anti-Piracy's signing tools before they will run in commercially available versions of Pro Tools"


