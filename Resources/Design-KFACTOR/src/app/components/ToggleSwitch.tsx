import { useState, useRef, useEffect } from 'react';
import smallKnobImage from '../../assets/c81f47cd65b06aeb18cd95074c3e427fa1755782.png';

interface ToggleSwitchProps {
  labelTop?: string;
  labelBottom?: string;
  labelLeft?: string;
  labelRight?: string;
  defaultValue?: boolean;
  onChange?: (value: boolean) => void;
  dataUi?: string;
}

export function ToggleSwitch({ 
  labelTop = 'ON', 
  labelBottom = 'OFF',
  labelLeft,
  labelRight,
  defaultValue = false,
  onChange,
  dataUi
}: ToggleSwitchProps) {
  const [isOn, setIsOn] = useState(defaultValue);
  const [isDragging, setIsDragging] = useState(false);
  const startYRef = useRef(0);
  const startStateRef = useRef(false);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    startYRef.current = e.clientY;
    startStateRef.current = isOn;
    e.preventDefault();
  };

  const handleClick = (e: React.MouseEvent) => {
    // Toggle on click
    const newState = !isOn;
    setIsOn(newState);
    onChange?.(newState);
    e.preventDefault();
  };

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging) return;

      const deltaY = startYRef.current - e.clientY;
      
      // If dragged up significantly, turn on; if dragged down, turn off
      if (deltaY > 20 && !startStateRef.current) {
        setIsOn(true);
        onChange?.(true);
        startStateRef.current = true;
        startYRef.current = e.clientY;
      } else if (deltaY < -20 && startStateRef.current) {
        setIsOn(false);
        onChange?.(false);
        startStateRef.current = false;
        startYRef.current = e.clientY;
      }
    };

    const handleMouseUp = () => {
      setIsDragging(false);
    };

    if (isDragging) {
      window.addEventListener('mousemove', handleMouseMove);
      window.addEventListener('mouseup', handleMouseUp);
    }

    return () => {
      window.removeEventListener('mousemove', handleMouseMove);
      window.removeEventListener('mouseup', handleMouseUp);
    };
  }, [isDragging, isOn, onChange]);

  // Rotation: 90 degrees points RIGHT (to labelTop), 270 degrees points LEFT (to labelBottom)
  // Knob pointer strictly limited to these two positions only
  const rotation = isOn ? 90 : 270;

  return (
    <div className="flex flex-col items-center relative">
      {/* Labels positioned above the knob */}
      {labelLeft && labelRight && (
        <div className="flex items-center gap-3" style={{ marginBottom: '-4px' }}>
          <span className="text-white text-[9px] tracking-wide font-medium">{labelLeft}</span>
          <span className="text-red-600 text-[9px] tracking-wide font-bold italic">{labelRight}</span>
        </div>
      )}
      
      {!labelLeft && !labelRight && (
        <div className="flex items-center gap-3 mb-2">
          {/* Left label (OFF/RIGHT/VERT) - in red */}
          <span className="text-red-600 text-[11px] tracking-wide font-bold italic">{labelBottom}</span>
          
          {/* Right label (ON/LEFT/LAT) - in white */}
          <span className="text-white text-[11px] tracking-wide font-medium">{labelTop}</span>
        </div>
      )}
      
      {/* Knob */}
      <div className="relative" style={{ width: '110px', height: '110px' }} data-ui={dataUi}>
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2">
          <div
            className="cursor-pointer rounded-full overflow-hidden"
            onMouseDown={handleMouseDown}
            onClick={handleClick}
            style={{
              width: '110px',
              height: '110px',
              transform: `rotate(${rotation}deg)`,
              transition: isDragging ? 'none' : 'transform 0.15s ease-out'
            }}
          >
            <img 
              src={smallKnobImage} 
              alt="Toggle Knob"
              style={{
                width: '100%',
                height: '100%',
                objectFit: 'contain',
                pointerEvents: 'none',
                userSelect: 'none'
              }}
              draggable={false}
            />
          </div>
        </div>
      </div>
    </div>
  );
}
