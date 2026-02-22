import { useEffect, useRef, useState } from 'react';
import vuMeterImage from '../../assets/7234cb3517b2e95b789fe268891ce6db456eb43a.png';

interface VUMeterProps {
  value?: number; // 0 to 100 (VU scale)
  label?: string;
  isPowerOn?: boolean;
  dataUi?: string;
}

export function VUMeter({ value = 50, label = 'K-FACTOR', isPowerOn = true, dataUi }: VUMeterProps) {
  const exportMode = typeof window !== 'undefined' && new URLSearchParams(window.location.search).has('export');
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [animatedValue, setAnimatedValue] = useState(value);
  const imageRef = useRef<HTMLImageElement | null>(null);

  // Preload the image once
  useEffect(() => {
    const img = new Image();
    img.src = vuMeterImage;
    imageRef.current = img;
  }, []);

  useEffect(() => {
    // Smooth animation towards target value
    const interval = setInterval(() => {
      setAnimatedValue(prev => {
        const diff = value - prev;
        if (Math.abs(diff) < 0.1) return value;
        return prev + diff * 0.1;
      });
    }, 16);

    return () => clearInterval(interval);
  }, [value]);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    // Set canvas size
    const width = 280;
    const height = 180;
    canvas.width = width;
    canvas.height = height;

    // Clear canvas
    ctx.clearRect(0, 0, width, height);

    const drawCanvas = () => {
      if (!imageRef.current) return;
      
      // Draw the background image first (with internal outline)
      ctx.drawImage(imageRef.current, 0, 0, width, height);
      
      // Draw K-FACTOR text below markers but before needle
      drawKFactorText(ctx);
      
      if (!exportMode) {
        // Draw the needle and scale on top of the image
        drawNeedle(ctx);
      }
    };

    // Draw immediately if image is loaded, otherwise wait for it
    if (imageRef.current?.complete) {
      drawCanvas();
    } else if (imageRef.current) {
      imageRef.current.onload = drawCanvas;
    }
    
    function drawKFactorText(ctx: CanvasRenderingContext2D) {
      // Position text in the yellow area, below the scale numbers
      const textX = width / 2;
      const textY = height * 0.50; // Higher in the yellow dial face area
      
      ctx.fillStyle = '#000000';
      ctx.font = 'italic bold 16px serif';
      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.fillText('K-FACTOR', textX, textY);
    }
    
    function drawNeedle(ctx: CanvasRenderingContext2D) {
      // VU meter arc setup - centered horizontally
      const meterCenterX = width / 2; // Horizontal center (140)
      
      // Position pivot for scale markers
      const meterCenterY = height - 40; // Pivot for scale
      
      // Smaller arc radius to fit within the meter window
      const arcRadius = height * 0.62;
      
      // Separate positioning for needle - higher up
      const needlePivotY = height - 65; // Needle pivot higher than scale
      
      // 160° sweep for vintage broadcast VU meter
      // Start angle at bottom left, end at bottom right
      const sweepDegrees = 160;
      const sweepRadians = sweepDegrees * (Math.PI / 180);
      
      // Center the 160° sweep with 0 mark slightly right of center
      // Start at 190° (bottom left), end at 350° (bottom right)
      const startAngle = (190 * Math.PI) / 180;
      const endAngle = (350 * Math.PI) / 180;
      
      // Define VU scale positions with compressed spacing on left side
      // Using non-linear mapping to compress left side spacing
      const scaleValues = [
        { position: 0.15, label: '-5' },
        { position: 0.30, label: '-4' },
        { position: 0.47, label: '-3' },
        { position: 0.65, label: '-2' },
        { position: 0.82, label: '-1' }
      ];
      
      // Draw tick marks with compressed spacing on left
      // Skip first and last few ticks to stay within yellow area
      const totalTicks = 32;
      for (let i = 2; i <= totalTicks - 2; i++) {
        // Apply non-linear spacing - compressed on left, stretched on right
        const linearPos = i / totalTicks;
        const compressedPos = Math.pow(linearPos, 0.85); // Slight compression curve
        
        const angle = startAngle + sweepRadians * compressedPos;
        
        // Major ticks every 4 positions
        const isMajorTick = (i % 4) === 0;
        const tickLength = isMajorTick ? 12 : 6;
        const innerRadius = arcRadius - tickLength;
        const outerRadius = arcRadius;

        const x1 = meterCenterX + Math.cos(angle) * innerRadius;
        const y1 = meterCenterY + Math.sin(angle) * innerRadius;
        const x2 = meterCenterX + Math.cos(angle) * outerRadius;
        const y2 = meterCenterY + Math.sin(angle) * outerRadius;

        ctx.beginPath();
        ctx.strokeStyle = '#000000';
        ctx.lineWidth = isMajorTick ? 2.5 : 1.5;
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.stroke();
      }

      // Draw engraved-style serif numbers
      scaleValues.forEach(({ position, label }) => {
        const angle = startAngle + sweepRadians * position;
        const textRadius = arcRadius - 24; // Position above the tick marks
        const textX = meterCenterX + Math.cos(angle) * textRadius;
        const textY = meterCenterY + Math.sin(angle) * textRadius;

        ctx.fillStyle = '#000000';
        ctx.font = 'bold 14px Georgia, "Times New Roman", serif';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(label, textX, textY);
      });

      // Calculate needle angle based on value (0-100 maps to scale)
      const needleValue = Math.max(0, Math.min(100, animatedValue));
      const needlePos = needleValue / 100;
      
      // Apply same compression curve to needle movement
      const compressedNeedlePos = Math.pow(needlePos, 0.85);
      const needleAngle = startAngle + sweepRadians * compressedNeedlePos;
      
      // Needle length - shorter than scale radius
      const needleLength = arcRadius - 35;
      const needlePivotX = meterCenterX;
      // Use separate needle pivot Y position
      const needleTargetX = meterCenterX + Math.cos(needleAngle) * needleLength;
      const needleTargetY = needlePivotY + Math.sin(needleAngle) * needleLength;

      // Draw main needle in black
      ctx.beginPath();
      ctx.moveTo(needlePivotX, needlePivotY);
      ctx.lineTo(needleTargetX, needleTargetY);
      ctx.strokeStyle = '#000000';
      ctx.lineWidth = 2.5;
      ctx.lineCap = 'round';
      ctx.stroke();
    }

  }, [animatedValue, label]);

  return (
    <div className="relative w-full h-full flex items-center justify-center">
      <canvas
        ref={canvasRef}
        className="w-full h-full relative"
        style={{ zIndex: 10 }}
        data-ui={dataUi}
      />
      {/* Subtle glow effect on the yellow dial area when powered on */}
      {isPowerOn && !exportMode && (
        <div 
          className="absolute"
          style={{
            top: '8%',
            left: '12%',
            width: '76%',
            height: '65%',
            background: 'radial-gradient(ellipse 80% 65% at 50% 45%, rgba(255, 230, 100, 0.5) 0%, rgba(255, 200, 50, 0.35) 35%, rgba(255, 180, 30, 0.15) 60%, transparent 80%)',
            pointerEvents: 'none',
            zIndex: 15,
            mixBlendMode: 'screen'
          }}
        />
      )}
    </div>
  );
}
