import React from 'react';

const AdjustModal = ({
  open,
  title,
  value,
  min = 0,
  max = 100,
  step = 1,
  unit = '',
  onChange,
  onClose,
  onSave
}) => {
  if (!open) return null;
  return (
    <div style={{
      position: 'fixed',
      top: 0,
      left: 0,
      width: '100vw',
      height: '100vh',
      background: 'rgba(0,0,0,0.25)',
      zIndex: 1000,
      display: 'flex',
      alignItems: 'center',
      justifyContent: 'center'
    }}>
      <div style={{
        background: '#fff',
        borderRadius: 16,
        padding: 32,
        minWidth: 320,
        boxShadow: '0 8px 32px rgba(0,0,0,0.18)',
        position: 'relative',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center'
      }}>
        <button onClick={onClose} style={{ position: 'absolute', top: 16, right: 16, background: 'none', border: 'none', fontSize: 22, cursor: 'pointer', color: '#888' }}>×</button>
        <h2 style={{ marginBottom: 24 }}>{title}</h2>
        <input
          type="number"
          min={min}
          max={max}
          step={step}
          value={value}
          onChange={e => onChange(Number(e.target.value))}
          style={{
            border: '1px solid #e0e0e0',
            borderRadius: 6,
            padding: '8px 16px',
            width: 100,
            fontSize: 18,
            marginBottom: 16,
            textAlign: 'center'
          }}
        />
        <div style={{ display: 'flex', alignItems: 'center', width: '100%', maxWidth: 220, marginBottom: 24 }}>
          <input
            type="range"
            min={min}
            max={max}
            step={step}
            value={value}
            onChange={e => onChange(Number(e.target.value))}
            style={{ flex: 1 }}
          />
          <span style={{ minWidth: 48, textAlign: 'right', color: '#2e7d32', fontWeight: 600, marginLeft: 12 }}>
            {value} {unit}
          </span>
        </div>
        <button
          onClick={onSave}
          style={{
            width: '100%',
            background: '#2e7d32',
            color: '#fff',
            border: 'none',
            padding: '12px 0',
            borderRadius: 8,
            fontWeight: 600,
            fontSize: 18,
            boxShadow: '0 2px 8px rgba(46,125,50,0.08)',
            cursor: 'pointer',
            transition: 'background 0.2s'
          }}
        >
          Save Change
        </button>
      </div>
    </div>
  );
};

export default AdjustModal; 