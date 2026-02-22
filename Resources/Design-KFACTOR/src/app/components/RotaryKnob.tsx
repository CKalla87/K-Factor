import { useState, useRef, useEffect } from 'react';
import knobImage from '../../assets/e718f5da746f4b568e9f5f9b1f310d63161780f6.png';

interface RotaryKnobProps {
  value?: number; // 0 to 1
  onChange?: (value: number) => void;
  size?: 'small' | 'medium' | 'large';
  steps?: string[];
  dataUi?: string;
}

export function RotaryKnob({ 
  value = 0.5, 
  onChange,
  size = 'large',
  steps = [],
  dataUi
}: RotaryKnobProps) {
  const [currentValue, setCurrentValue] = useState(value);
  const [isDragging, setIsDragging] = useState(false);
  const knobRef = useRef<HTMLDivElement>(null);
  const startYRef = useRef(0);
  const startValueRef = useRef(0);

  const sizeClasses = {
    small: 'w-10 h-10',
    medium: 'w-14 h-14',
    large: 'w-16 h-16'
  };

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    startYRef.current = e.clientY;
    startValueRef.current = currentValue;
    e.preventDefault();
  };

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging) return;

      const deltaY = startYRef.current - e.clientY;
      const sensitivity = 0.003;
      let newValue = Math.max(0, Math.min(1, startValueRef.current + deltaY * sensitivity));
      
      // Snap to steps if steps are defined
      if (steps.length > 0) {
        const stepCount = steps.length;
        const stepSize = 1 / (stepCount - 1);
        const nearestStep = Math.round(newValue / stepSize);
        newValue = nearestStep * stepSize;
      }
      
      setCurrentValue(newValue);
      onChange?.(newValue);
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
  }, [isDragging, onChange, steps.length]);

  const rotation = -135 + currentValue * 270; // -135° to +135°

  return (
    <div className="flex flex-col items-center relative">
      <div className="relative" style={{ width: '110px', height: '110px' }} data-ui={dataUi}>
        {/* Value labels around knob */}
        {steps.length > 0 && (
          <div className="absolute inset-0 pointer-events-none">
            {steps.map((step, index) => {
              const totalSteps = steps.length;
              const angle = -135 + (270 / (totalSteps - 1)) * index;
              const radians = (angle * Math.PI) / 180;
              const labelRadius = 44;
              const x = 50 + Math.cos(radians) * labelRadius;
              const y = 50 + Math.sin(radians) * labelRadius;

              return (
                <span
                  key={index}
                  className="absolute text-white text-xs font-bold select-none whitespace-nowrap"
                  style={{
                    left: `${x}%`,
                    top: `${y}%`,
                    transform: 'translate(-50%, -50%)',
                  }}
                >
                  {step}
                </span>
              );
            })}
          </div>
        )}

        {/* Knob body - centered - using actual knob image */}
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2">
          <div
            ref={knobRef}
            className="cursor-pointer rounded-full overflow-hidden"
            onMouseDown={handleMouseDown}
            style={{
              width: '85px',
              height: '85px',
              transform: `rotate(${rotation}deg)`,
              transition: isDragging ? 'none' : 'transform 0.05s ease-out'
            }}
          >
            <img 
              src={knobImage} 
              alt="Knob"
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
