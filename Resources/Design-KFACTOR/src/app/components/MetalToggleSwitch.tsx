interface MetalToggleSwitchProps {
  isOn: boolean;
}

export function MetalToggleSwitch({ isOn }: MetalToggleSwitchProps) {
  const angle = isOn ? -30 : 30; // negative for up, positive for down
  
  return (
    <svg 
      width="100%" 
      height="100%" 
      viewBox="0 0 512 512" 
      fill="none" 
      xmlns="http://www.w3.org/2000/svg"
      style={{ display: 'block' }}
    >
      <defs>
        {/* Polished chrome base with realistic reflections */}
        <radialGradient id="chromeBase" cx="45%" cy="35%">
          <stop offset="0%" stopColor="#f5f5f5" />
          <stop offset="20%" stopColor="#e0e0e0" />
          <stop offset="45%" stopColor="#a8a8a8" />
          <stop offset="70%" stopColor="#c8c8c8" />
          <stop offset="90%" stopColor="#888888" />
          <stop offset="100%" stopColor="#707070" />
        </radialGradient>
        
        {/* Beveled edge gradient */}
        <linearGradient id="bevelGradient" x1="0%" y1="0%" x2="0%" y2="100%">
          <stop offset="0%" stopColor="#ffffff" />
          <stop offset="50%" stopColor="#b8b8b8" />
          <stop offset="100%" stopColor="#606060" />
        </linearGradient>
        
        {/* Recessed area gradient */}
        <radialGradient id="recessGradient" cx="50%" cy="50%">
          <stop offset="0%" stopColor="#9a9a9a" />
          <stop offset="60%" stopColor="#888888" />
          <stop offset="100%" stopColor="#707070" />
        </radialGradient>
        
        {/* Chrome lever with realistic reflections */}
        <linearGradient id={`leverChrome-${isOn}`} x1="0%" y1="0%" x2="100%" y2="0%">
          <stop offset="0%" stopColor="#3a3a3a" />
          <stop offset="15%" stopColor="#ffffff" />
          <stop offset="25%" stopColor="#f5f5f5" />
          <stop offset="35%" stopColor="#2a2a2a" />
          <stop offset="50%" stopColor="#e0e0e0" />
          <stop offset="65%" stopColor="#1a1a1a" />
          <stop offset="80%" stopColor="#d8d8d8" />
          <stop offset="100%" stopColor="#5a5a5a" />
        </linearGradient>
        
        {/* Ball tip chrome */}
        <radialGradient id={`ballChrome-${isOn}`} cx="30%" cy="30%">
          <stop offset="0%" stopColor="#ffffff" />
          <stop offset="25%" stopColor="#f8f8f8" />
          <stop offset="45%" stopColor="#3a3a3a" />
          <stop offset="60%" stopColor="#e0e0e0" />
          <stop offset="75%" stopColor="#2a2a2a" />
          <stop offset="90%" stopColor="#b0b0b0" />
          <stop offset="100%" stopColor="#6a6a6a" />
        </radialGradient>
        
        {/* Mounting hole shadow */}
        <radialGradient id="holeShadow" cx="50%" cy="50%">
          <stop offset="0%" stopColor="#000000" />
          <stop offset="60%" stopColor="#1a1a1a" />
          <stop offset="85%" stopColor="#3a3a3a" />
          <stop offset="100%" stopColor="#4a4a4a" />
        </radialGradient>
        
        {/* Enhanced shadow */}
        <filter id="shadow" x="-50%" y="-50%" width="200%" height="200%">
          <feGaussianBlur in="SourceAlpha" stdDeviation="6"/>
          <feOffset dx="0" dy="6" result="offsetblur"/>
          <feComponentTransfer>
            <feFuncA type="linear" slope="0.6"/>
          </feComponentTransfer>
          <feMerge>
            <feMergeNode/>
            <feMergeNode in="SourceGraphic"/>
          </feMerge>
        </filter>
        
        {/* Contact shadow under lever - stronger for realism */}
        <filter id="contactShadow" x="-100%" y="-100%" width="300%" height="300%">
          <feGaussianBlur in="SourceAlpha" stdDeviation="5"/>
          <feOffset dx="0" dy="8" result="offsetblur"/>
          <feComponentTransfer>
            <feFuncA type="linear" slope="0.9"/>
          </feComponentTransfer>
          <feMerge>
            <feMergeNode/>
            <feMergeNode in="SourceGraphic"/>
          </feMerge>
        </filter>
        
        {/* Additional cast shadow for depth */}
        <filter id="leverCastShadow" x="-150%" y="-150%" width="400%" height="400%">
          <feGaussianBlur in="SourceAlpha" stdDeviation="8"/>
          <feOffset dx="0" dy="12" result="offsetblur"/>
          <feComponentTransfer>
            <feFuncA type="linear" slope="0.5"/>
          </feComponentTransfer>
          <feMerge>
            <feMergeNode/>
            <feMergeNode in="SourceGraphic"/>
          </feMerge>
        </filter>
      </defs>
      
      {/* Base plate assembly */}
      <g>
        {/* Drop shadow */}
        <ellipse 
          cx="256" 
          cy="261" 
          rx="90" 
          ry="86"
          fill="#000000"
          opacity="0.2"
          style={{ filter: 'blur(8px)' }}
        />
        
        {/* Main chrome base plate */}
        <circle 
          cx="256" 
          cy="256" 
          r="88" 
          fill="url(#chromeBase)"
        />
        
        {/* Specular highlight - simulates light reflection */}
        <ellipse 
          cx="235" 
          cy="235" 
          rx="45" 
          ry="40"
          fill="#ffffff"
          opacity="0.3"
        />
        
        {/* Outer beveled ring - raised edge */}
        <circle 
          cx="256" 
          cy="256" 
          r="88" 
          fill="none"
          stroke="url(#bevelGradient)"
          strokeWidth="6"
          opacity="0.5"
        />
        
        {/* Top edge shine */}
        <circle 
          cx="256" 
          cy="256" 
          r="90" 
          fill="none"
          stroke="#ffffff"
          strokeWidth="1"
          opacity="0.4"
        />
        
        {/* Inner step - creates depth */}
        <circle 
          cx="256" 
          cy="256" 
          r="82" 
          fill="none"
          stroke="#303030"
          strokeWidth="1.5"
          opacity="0.25"
        />
        
        {/* Recessed mounting area */}
        <circle 
          cx="256" 
          cy="256" 
          r="50" 
          fill="url(#recessGradient)"
        />
        
        {/* Recess top shadow */}
        <circle 
          cx="256" 
          cy="254" 
          r="50" 
          fill="#000000"
          opacity="0.12"
        />
        
        {/* Recess edge definition */}
        <circle 
          cx="256" 
          cy="256" 
          r="50" 
          fill="none"
          stroke="#404040"
          strokeWidth="1.5"
          opacity="0.35"
        />
        
        {/* Recess inner highlight */}
        <circle 
          cx="256" 
          cy="256" 
          r="48" 
          fill="none"
          stroke="#a0a0a0"
          strokeWidth="0.5"
          opacity="0.3"
        />
        
        {/* Center mounting hole */}
        <circle 
          cx="256" 
          cy="256" 
          r="14" 
          fill="url(#holeShadow)"
        />
        
        {/* Hole rim light reflection */}
        <circle 
          cx="256" 
          cy="256" 
          r="14.5" 
          fill="none"
          stroke="#606060"
          strokeWidth="0.8"
          opacity="0.6"
        />
        
        {/* Inner hole shadow */}
        <circle 
          cx="256" 
          cy="254" 
          r="13" 
          fill="#000000"
          opacity="0.3"
        />
      </g>
      
      {/* Toggle lever - rotates around center */}
      <g transform={`translate(256, 256) rotate(${angle})`}>
        
        {/* Large diffuse shadow underneath */}
        <ellipse
          cx="0"
          cy="12"
          rx="32"
          ry="12"
          fill="#000000"
          opacity="0.35"
          style={{ filter: 'blur(10px)' }}
        />
        
        {/* Contact shadow at base - sharper and darker */}
        <ellipse
          cx="0"
          cy="4"
          rx="22"
          ry="10"
          fill="#000000"
          opacity="0.7"
          style={{ filter: 'blur(4px)' }}
        />
        
        {/* Hard contact shadow - right at the base */}
        <ellipse
          cx="0"
          cy="2"
          rx="18"
          ry="7"
          fill="#000000"
          opacity="0.8"
        />
        
        {/* Lever shaft - cylindrical chrome */}
        <rect
          x="-15"
          y="-110"
          width="30"
          height="110"
          fill={`url(#leverChrome-${isOn})`}
        />
        
        {/* Left edge shine */}
        <rect
          x="-15"
          y="-110"
          width="8"
          height="110"
          fill="#ffffff"
          opacity="0.3"
        />
        
        {/* Center reflection strip */}
        <rect
          x="-3"
          y="-110"
          width="6"
          height="110"
          fill="#ffffff"
          opacity="0.15"
        />
        
        {/* Right edge shadow */}
        <rect
          x="7"
          y="-110"
          width="8"
          height="110"
          fill="#000000"
          opacity="0.4"
        />
        
        {/* Ball tip shadow */}
        <ellipse 
          cx="2" 
          cy="-108" 
          rx="24" 
          ry="22"
          fill="#000000"
          opacity="0.3"
        />
        
        {/* Ball tip */}
        <circle 
          cx="0" 
          cy="-110" 
          r="20" 
          fill={`url(#ballChrome-${isOn})`}
        />
        
        {/* Ball specular highlight */}
        <ellipse 
          cx="-6" 
          cy="-116" 
          rx="10" 
          ry="8"
          fill="#ffffff"
          opacity="0.9"
        />
        
        {/* Ball secondary highlight */}
        <ellipse 
          cx="7" 
          cy="-112" 
          rx="4" 
          ry="6"
          fill="#ffffff"
          opacity="0.4"
        />
        
        {/* Ball rim edge */}
        <circle 
          cx="0" 
          cy="-110" 
          r="20" 
          fill="none"
          stroke="#1a1a1a"
          strokeWidth="0.5"
          opacity="0.3"
        />
        
        {/* Base connection - chrome cylinder */}
        <ellipse
          cx="0"
          cy="0"
          rx="18"
          ry="6"
          fill={`url(#ballChrome-${isOn})`}
        />
        
        {/* Connection highlight */}
        <ellipse
          cx="-4"
          cy="0"
          rx="8"
          ry="4"
          fill="#ffffff"
          opacity="0.5"
        />
        
        {/* Connection shadow */}
        <ellipse
          cx="0"
          cy="1"
          rx="18"
          ry="5"
          fill="#000000"
          opacity="0.2"
        />
      </g>
    </svg>
  );
}
