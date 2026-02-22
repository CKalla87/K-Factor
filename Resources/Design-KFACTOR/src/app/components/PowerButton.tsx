import { useState } from 'react';

interface PowerButtonProps {
  isOn: boolean;
  dataUi?: string;
}

export function PowerButton({ isOn, dataUi }: PowerButtonProps) {
  return (
    <div className="flex flex-col items-center justify-center">
      {/* Power LED indicator */}
      <div
        className="relative w-8 h-8 rounded-full transition-all"
        aria-label="Power indicator"
        data-ui={dataUi}
      >
        <div
          className={`absolute inset-0 rounded-full transition-all duration-300 ${
            isOn
              ? 'bg-yellow-500 shadow-[0_0_20px_rgba(234,179,8,0.9),0_0_10px_rgba(250,204,21,0.6)]'
              : 'bg-zinc-700 shadow-none'
          }`}
        >
          {/* Inner bright glow when on */}
          {isOn && <div className="absolute inset-1 rounded-full bg-yellow-400" />}
        </div>
      </div>
    </div>
  );
}
