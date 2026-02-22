import { useState, useRef, useEffect } from 'react';
import smallKnobImage from '../../assets/c81f47cd65b06aeb18cd95074c3e427fa1755782.png';

interface ThreeWaySwitchProps {
  labelLeft?: string;
  labelCenter?: string;
  labelRight?: string;
  defaultValue?: 'left' | 'center' | 'right';
  onChange?: (value: 'left' | 'center' | 'right') => void;
  dataUi?: string;
}

export function ThreeWaySwitch({ 
  labelLeft = 'LEFT',
  labelCenter = 'CENTER',
  labelRight = 'RIGHT',
  defaultValue = 'center',
  onChange,
  dataUi
}: ThreeWaySwitchProps) {
  const [position, setPosition] = useState<'left' | 'center' | 'right'>(defaultValue);
  const [isDragging, setIsDragging] = useState(false);
  const startYRef = useRef(0);
  const startPositionRef = useRef<'left' | 'center' | 'right'>(defaultValue);

  const handleMouseDown = (e: React.MouseEvent) => {
    setIsDragging(true);
    startYRef.current = e.clientY;
    startPositionRef.current = position;
    e.preventDefault();
  };

  const handleClick = (e: React.MouseEvent) => {
    // Cycle through positions on click
    let newPosition: 'left' | 'center' | 'right';
    if (position === 'left') {
      newPosition = 'center';
    } else if (position === 'center') {
      newPosition = 'right';
    } else {
      newPosition = 'left';
    }
    setPosition(newPosition);
    onChange?.(newPosition);
    e.preventDefault();
  };

  useEffect(() => {
    const handleMouseMove = (e: MouseEvent) => {
      if (!isDragging) return;

      const deltaY = startYRef.current - e.clientY;
      
      // Drag up to cycle forward, drag down to cycle backward
      if (deltaY > 30) {
        let newPosition: 'left' | 'center' | 'right';
        if (startPositionRef.current === 'left') {
          newPosition = 'center';
        } else if (startPositionRef.current === 'center') {
          newPosition = 'right';
        } else {
          newPosition = 'left';
        }
        
        if (newPosition !== position) {
          setPosition(newPosition);
          onChange?.(newPosition);
          startPositionRef.current = newPosition;
          startYRef.current = e.clientY;
        }
      } else if (deltaY < -30) {
        let newPosition: 'left' | 'center' | 'right';
        if (startPositionRef.current === 'left') {
          newPosition = 'right';
        } else if (startPositionRef.current === 'center') {
          newPosition = 'left';
        } else {
          newPosition = 'center';
        }
        
        if (newPosition !== position) {
          setPosition(newPosition);
          onChange?.(newPosition);
          startPositionRef.current = newPosition;
          startYRef.current = e.clientY;
        }
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
  }, [isDragging, position, onChange]);

  // Rotation: left = -45 degrees (pointing to IN), center = 0 degrees (pointing up to GR), right = 45 degrees (pointing to OUT)
  const getRotation = () => {
    if (position === 'left') return -45;
    if (position === 'center') return 0;
    return 45;
  };

  const rotation = getRotation();

  return (
    <div className="flex flex-col items-center relative">
      {/* Top labels in arc shape - GR highest, IN and OUT lower */}
      <div className="flex items-start gap-4 mb-0">
        <span className="text-white text-[9px] tracking-wide font-medium" style={{ paddingTop: '6px' }}>{labelLeft}</span>
        <span className="text-white text-[9px] tracking-wide font-bold italic">{labelCenter}</span>
        <span className="text-white text-[9px] tracking-wide font-medium" style={{ paddingTop: '6px' }}>{labelRight}</span>
      </div>
      
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
              alt="Three-way Switch"
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
