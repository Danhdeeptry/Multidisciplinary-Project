import React from 'react';

const SliderControl = ({ label, icon, value, min, max, step = 1, unit, onChange }) => (
  <div style={{ marginBottom: 28 }}>
    <label style={{ fontWeight: 600, color: '#333', marginBottom: 8, display: 'block', fontSize: 15 }}>
      {icon} {label} {unit && <span style={{ color: '#888', fontWeight: 400, fontSize: 13 }}>({unit})</span>}
    </label>
    <div style={{ display: 'flex', alignItems: 'center', gap: 16 }}>
      <input
        type="number"
        min={min}
        max={max}
        step={step}
        value={value}
        onChange={e => onChange(Number(e.target.value))}
        style={{ border: '1px solid #e0e0e0', borderRadius: 6, padding: '6px 12px', width: 80, fontSize: 16 }}
      />
      <input
        type="range"
        min={min}
        max={max}
        step={step}
        value={value}
        onChange={e => onChange(Number(e.target.value))}
        style={{ flex: 1 }}
      />
      <span style={{ minWidth: 32, textAlign: 'right', color: '#2e7d32', fontWeight: 600 }}>{value}{unit}</span>
    </div>
  </div>
);

export default SliderControl; 