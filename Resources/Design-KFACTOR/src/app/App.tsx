import { useState, useEffect, useRef } from 'react';
import html2canvas from 'html2canvas';
import { VUMeter } from './components/VUMeter';
import { RotaryKnob } from './components/RotaryKnob';
import { ToggleSwitch } from './components/ToggleSwitch';
import { ThreeWaySwitch } from './components/ThreeWaySwitch';
import { SmallKnob } from './components/SmallKnob';
import { PowerButton } from './components/PowerButton';
import { PowerToggle } from './components/PowerToggle';
import { Download } from 'lucide-react';

export default function App() {
  const [leftVU, setLeftVU] = useState(0);
  const [rightVU, setRightVU] = useState(0);
  const [isPowerOn, setIsPowerOn] = useState(true);
  const pluginRef = useRef<HTMLDivElement>(null);
  const exportMode = typeof window !== 'undefined' && new URLSearchParams(window.location.search).has('export');

  const handleExport = async () => {
    if (!pluginRef.current) return;
    
    try {
      const canvas = await html2canvas(pluginRef.current, {
        backgroundColor: '#18181b',
        scale: 2,
        logging: false,
        useCORS: true
      });
      
      const link = document.createElement('a');
      link.download = 'k-factor-plugin.png';
      link.href = canvas.toDataURL('image/png');
      link.click();
    } catch (error) {
      console.error('Error exporting image:', error);
    }
  };

  // Simulate VU meter movement
  useEffect(() => {
    if (!isPowerOn) {
      setLeftVU(0);
      setRightVU(0);
      return;
    }
    
    const interval = setInterval(() => {
      setLeftVU(prev => {
        const target = 30 + Math.random() * 50;
        return prev + (target - prev) * 0.15;
      });
      setRightVU(prev => {
        const target = 30 + Math.random() * 50;
        return prev + (target - prev) * 0.15;
      });
    }, 100);

    return () => clearInterval(interval);
  }, [isPowerOn]);

  const inputGainSteps = ['0', '.6', '1.2', '2', '3', '4', '6', '8', '10', '15', '20', '30'];
  const thresholdSteps = ['1', '2', '3', '4', '5', '6'];
  const timeConstantSteps = ['.05', '.1', '.2', '.3', '.4', '.6', '.8', '1'];
  const timeConstantStepsNoDots = ['05', '1', '2', '3', '4', '6', '8', '10'];

  return (
    <div className="size-full flex flex-col items-center justify-center bg-zinc-900 p-8 overflow-auto gap-4">
      {/* Export Button */}
      <button
        onClick={handleExport}
        className="flex items-center gap-2 px-4 py-2 bg-zinc-800 hover:bg-zinc-700 text-white rounded-lg border border-zinc-600 transition-colors shadow-lg"
      >
        <Download size={18} />
        Export as Image
      </button>
      
      {/* Main Plugin Container */}
      <div ref={pluginRef} id="kfactor-plugin" className="bg-gradient-to-br from-zinc-800 via-zinc-900 to-black rounded-2xl shadow-[0_20px_60px_rgba(0,0,0,0.9)] px-16 py-5 border-4 border-zinc-950 relative" style={{
        backgroundImage: 'linear-gradient(135deg, rgba(82, 82, 91, 0.15) 0%, rgba(39, 39, 42, 0.3) 25%, rgba(24, 24, 27, 0.5) 50%, rgba(9, 9, 11, 0.7) 75%, rgba(0, 0, 0, 0.9) 100%), radial-gradient(ellipse at 30% 30%, rgba(113, 113, 122, 0.08) 0%, transparent 50%), radial-gradient(ellipse at 70% 70%, rgba(63, 63, 70, 0.06) 0%, transparent 50%)'
      }}>
        
        {/* Rack Mount Screws */}
        <div className="absolute top-4 left-4 w-5 h-5 rounded-full bg-gradient-to-br from-zinc-600 to-zinc-800 shadow-[inset_0_2px_3px_rgba(0,0,0,0.8)]">
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="w-3 h-0.5 bg-zinc-900 absolute" />
            <div className="w-0.5 h-3 bg-zinc-900 absolute" />
          </div>
        </div>
        <div className="absolute top-4 right-4 w-5 h-5 rounded-full bg-gradient-to-br from-zinc-600 to-zinc-800 shadow-[inset_0_2px_3px_rgba(0,0,0,0.8)]">
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="w-3 h-0.5 bg-zinc-900 absolute" />
            <div className="w-0.5 h-3 bg-zinc-900 absolute" />
          </div>
        </div>
        <div className="absolute bottom-4 left-4 w-5 h-5 rounded-full bg-gradient-to-br from-zinc-600 to-zinc-800 shadow-[inset_0_2px_3px_rgba(0,0,0,0.8)]">
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="w-3 h-0.5 bg-zinc-900 absolute" />
            <div className="w-0.5 h-3 bg-zinc-900 absolute" />
          </div>
        </div>
        <div className="absolute bottom-4 right-4 w-5 h-5 rounded-full bg-gradient-to-br from-zinc-600 to-zinc-800 shadow-[inset_0_2px_3px_rgba(0,0,0,0.8)]">
          <div className="absolute inset-0 flex items-center justify-center">
            <div className="w-3 h-0.5 bg-zinc-900 absolute" />
            <div className="w-0.5 h-3 bg-zinc-900 absolute" />
          </div>
        </div>

        {/* Main Content */}
        <div className="flex gap-6">
          {/* Power controls column */}
          <div className="relative flex flex-col justify-center">
            {/* Power Toggle - aligned with top VU */}
            <div className="flex items-center justify-center h-[220px] -mt-16 mb-6">
              {exportMode ? (
                <div style={{ width: '96px', height: '96px' }} />
              ) : (
                <PowerToggle isOn={isPowerOn} onToggle={setIsPowerOn} dataUi="powerToggle" />
              )}
            </div>
            
            {/* Power Glow - centered between VU meters */}
            <div className="flex items-center justify-center mb-6" style={{ marginTop: '40px' }}>
              {exportMode ? (
                <div style={{ width: '32px', height: '32px' }} />
              ) : (
                <PowerButton isOn={isPowerOn} dataUi="powerLed" />
              )}
            </div>
            
            {/* Spacer - aligned with bottom VU */}
            <div className="h-[220px]"></div>
          </div>

          <div className="flex items-center gap-8">
            {/* Left side - Two rows of controls */}
            <div className="flex flex-col gap-6">
              {/* Top Row - Left Channel */}
              <div className="flex items-center gap-8">
                {/* Left VU Meter Section */}
                <div className="rounded-2xl p-4 relative">
                  <div className="w-[280px] h-[220px]" data-ui="vuLeft">
                    <VUMeter value={leftVU} label="K-FACTOR" isPowerOn={isPowerOn} dataUi="vuLeftNeedle" />
                  </div>
                </div>

                {/* LEFT-LAT Metering */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center" style={{ marginBottom: '12px' }}>
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">LEFT-</span><span className="text-red-600">LAT</span><br/>
                      <span className="text-white">METERING</span>
                    </span>
                  </div>
                  <ThreeWaySwitch 
                    labelLeft="IN"
                    labelCenter="GR"
                    labelRight="OUT"
                    defaultValue="left"
                    dataUi="meterLeft"
                  />
                </div>

                {/* Left Channel Input Gain */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-6">
                    <span className="text-white text-[10px] tracking-wide text-center leading-tight">LEFT CHANNEL<br/>INPUT GAIN</span>
                  </div>
                  <RotaryKnob 
                    steps={inputGainSteps}
                    size="large"
                    dataUi="inputGainLeft"
                  />
                </div>

                {/* Left-Lat Threshold */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-6">
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">LEFT-</span><span className="text-red-600">LAT</span><br/>
                      <span className="text-white">THRESHOLD</span>
                    </span>
                  </div>
                  <RotaryKnob 
                    steps={thresholdSteps}
                    size="large"
                    dataUi="thresholdLeft"
                  />
                </div>

                {/* Left-Lat Time Constant */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-6">
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">LEFT-</span><span className="text-red-600">LAT</span><br/>
                      <span className="text-white">TIME CONSTANT</span>
                    </span>
                  </div>
                  <RotaryKnob 
                    steps={timeConstantStepsNoDots}
                    size="large"
                    dataUi="timeConstantLeft"
                  />
                </div>
              </div>

              {/* Bottom Row - Right Channel */}
              <div className="flex items-center gap-8">
                {/* Right VU Meter Section */}
                <div className="rounded-2xl p-4 relative">
                  <div className="w-[280px] h-[220px]" data-ui="vuRight">
                    <VUMeter value={rightVU} label="K-FACTOR" isPowerOn={isPowerOn} dataUi="vuRightNeedle" />
                  </div>
                </div>

                {/* RIGHT-VERT Metering */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center" style={{ marginBottom: '12px' }}>
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">RIGHT-</span><span className="text-red-600">VERT</span><br/>
                      <span className="text-white">METERING</span>
                    </span>
                  </div>
                  <ThreeWaySwitch 
                    labelLeft="IN"
                    labelCenter="GR"
                    labelRight="OUT"
                    defaultValue="left"
                    dataUi="meterRight"
                  />
                </div>

                {/* Right Channel Input Gain */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-6">
                    <span className="text-white text-[10px] tracking-wide text-center leading-tight">RIGHT CHANNEL<br/>INPUT GAIN</span>
                  </div>
                  <RotaryKnob 
                    steps={inputGainSteps}
                    size="large"
                    dataUi="inputGainRight"
                  />
                </div>

                {/* Right-Vert Threshold */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-6">
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">RIGHT-</span><span className="text-red-600">VERT</span><br/>
                      <span className="text-white">THRESHOLD</span>
                    </span>
                  </div>
                  <RotaryKnob 
                    steps={thresholdSteps}
                    size="large"
                    dataUi="thresholdRight"
                  />
                </div>

                {/* Right-Vert Time Constant */}
                <div className="flex flex-col items-center">
                  <div className="flex flex-col items-center mb-3">
                    <span className="text-[10px] tracking-wide text-center leading-tight">
                      <span className="text-white">RIGHT-</span><span className="text-red-600">VERT</span><br/>
                      <span className="text-white">TIME CONSTANT</span>
                    </span>
                  </div>
                  <RotaryKnob 
                    steps={['1', '2', '3', '4', '5', '6']}
                    size="large"
                    dataUi="timeConstantRight"
                  />
                </div>
              </div>
            </div>

            {/* AGC Toggle - Centered vertically between rows */}
            <div className="flex flex-col items-center justify-center self-center" style={{ marginLeft: '-80px', marginTop: '20px' }}>
              <div className="flex flex-col items-center mb-3">
                <span className="text-white text-[10px] tracking-wide text-center leading-tight">AGC</span>
              </div>
              <ToggleSwitch labelLeft="LEFT/RIGHT" labelRight="LAT/VERT" dataUi="agcToggle" />
            </div>
          </div>
        </div>

        {/* Bottom Branding */}
        <div className="flex justify-end items-center gap-4 mt-6">
          <span className="text-red-600 font-bold text-2xl italic tracking-wide">K-FACTOR</span>
          <span className="text-white text-[10px] tracking-widest">CK AUDIO DESIGN INC.</span>
          <span className="text-white text-[10px] tracking-widest">TEMPE, ARIZONA</span>
          <span className="text-white text-[10px] tracking-widest">MODEL 670</span>
        </div>
      </div>
    </div>
  );
}
