import React from 'react';

const StatusCard = ({ icon, label, value, unit, color = '#2e7d32', progress }) => (
  <div style={{
    background: '#fff',
    borderRadius: 16,
    boxShadow: '0 4px 16px rgba(46,125,50,0.08)',
    padding: 20,
    minWidth: 120,
    textAlign: 'center',
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'center',
    gap: 8
  }}>
    <div style={{ fontSize: 32, color }}>{icon}</div>
    <div style={{ color: '#333', fontWeight: 600, fontSize: 16 }}>{label}</div>
    <div style={{ color, fontWeight: 700, fontSize: 22 }}>
      {value}{unit && <span style={{ fontSize: 16, color: '#555', marginLeft: 2 }}>{unit}</span>}
    </div>
    {typeof progress === 'number' && (
      <div style={{ width: '100%', marginTop: 8 }}>
        <div style={{
          height: 8,
          background: '#e0e0e0',
          borderRadius: 4,
          overflow: 'hidden',
        }}>
          <div style={{
            width: `${progress}%`,
            height: '100%',
            background: color,
            borderRadius: 4,
            transition: 'width 0.3s ease'
          }} />
        </div>
      </div>
    )}
  </div>
);

export default StatusCard; 