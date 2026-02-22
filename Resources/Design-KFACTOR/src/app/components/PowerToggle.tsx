import { MetalToggleSwitch } from './MetalToggleSwitch';

interface PowerToggleProps {
  isOn: boolean;
  onToggle: (value: boolean) => void;
  dataUi?: string;
}

export function PowerToggle({ isOn, onToggle, dataUi }: PowerToggleProps) {
  const handleClick = () => {
    onToggle(!isOn);
  };

  return (
    <div className="flex flex-col items-center">
      {/* ON Label */}
      <span className="text-white text-[9px] tracking-wider font-medium -mb-6">ON</span>

      {/* Toggle Switch */}
      <div 
        className="w-24 h-24 cursor-pointer transition-opacity hover:opacity-90"
        onClick={handleClick}
        style={{ background: 'transparent' }}
        data-ui={dataUi}
      >
        <MetalToggleSwitch isOn={isOn} />
      </div>
    </div>
  );
}
