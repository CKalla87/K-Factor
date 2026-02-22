import { useState, useRef, useEffect } from 'react';
import smallKnobImage from 'figma:asset/c81f47cd65b06aeb18cd95074c3e427fa1755782.png';

interface SmallKnobProps {
  label: string;
  defaultValue?: number; // 0 to 1
  onChange?: (value: number) => void;
  labels?: string[]; // Optional labels around the knob
}

export function SmallKnob({ label, defaultValue = 0.5, onChange, labels = [] }: SmallKnobProps) {
  const [value, setValue] = useState(defaultValue);
  const [isDragging, setIsDragging] = useState(false);
  const startYRef = useRef(0);
  const startValueRef = useRef(0);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    startYRef.current = e.clientY;
    startValueRef.current = value;
    e.preventDefault();
  };

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging) return;

      const deltaY = startYRef.current - e.clientY;
      const sensitivity = 0.003;
      let newValue = Math.max(0, Math.min(1, startValueRef.current + deltaY * sensitivity));
      
      // Snap to labels if labels are defined
      if (labels.length > 0) {
        const stepCount = labels.length;
        const stepSize = 1 / (stepCount - 1);
        const nearestStep = Math.round(newValue / stepSize);
        newValue = nearestStep * stepSize;
      }
      
      setValue(newValue);
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
  }, [isDragging, onChange, labels.length]);

  const rotation = -135 + value * 270;

  return (
    <div className="flex flex-col items-center gap-2 relative">
      <span className="text-white text-[9px] tracking-wide font-medium">{label}</span>
      <div className="relative" style={{ width: '80px', height: '80px' }}>
        {/* Labels around knob if provided */}
        {labels.length > 0 && (
          <div className="absolute inset-0 pointer-events-none">
            {labels.map((labelText, index) => {
              const totalLabels = labels.length;
              const angle = -90 + (360 / totalLabels) * index;
              const radians = (angle * Math.PI) / 180;
              const radius = 55;
              const x = 50 + Math.cos(radians) * radius;
              const y = 50 + Math.sin(radians) * radius;

              // Check if this is a circle indicator
              const isCircle = labelText === '•';

              return isCircle ? (
                <div
                  key={index}
                  className="absolute w-1.5 h-1.5 rounded-full bg-white select-none"
                  style={{
                    left: `${x}%`,
                    top: `${y}%`,
                    transform: 'translate(-50%, -50%)',
                  }}
                />
              ) : (
                <span
                  key={index}
                  className="absolute text-white text-[10px] font-medium select-none"
                  style={{
                    left: `${x}%`,
                    top: `${y}%`,
                    transform: 'translate(-50%, -50%)',
                  }}
                >
                  {labelText}
                </span>
              );
            })}
          </div>
        )}

        {/* Knob */}
        <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2">
          <div
            className="cursor-pointer rounded-full overflow-hidden"
            onMouseDown={handleMouseDown}
            style={{
              width: '60px',
              height: '60px',
              transform: `rotate(${rotation}deg)`,
              transition: isDragging ? 'none' : 'transform 0.05s ease-out'
            }}
          >
            <img 
              src={smallKnobImage} 
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
