import React from 'react';
import { FaLeaf } from 'react-icons/fa';

const LeafHealthWidget = ({ health, color, status }) => {
  return (
    <div style={{
      background: '#f8f9fa',
      borderRadius: 16,
      boxShadow: '0 4px 12px rgba(0,0,0,0.08)',
      border: '2px solid #e0e0e0',
      padding: 24,
      position: 'relative',
      textAlign: 'center'
    }}>
      <div style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', gap: 16, marginBottom: 12 }}>
        <div style={{
          width: 48,
          height: 48,
          borderRadius: '50%',
          background: color,
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          color: '#fff',
          fontSize: 28,
          boxShadow: '0 2px 8px rgba(0,0,0,0.10)'
        }}>
          <FaLeaf />
        </div>
        <div>
          <div style={{ fontWeight: 600, color: '#333', fontSize: 16 }}>Leaf Health</div>
          <div style={{ color: color, fontWeight: 700, fontSize: 18 }}>{status}</div>
        </div>
      </div>
      <div style={{
        background: '#fff',
        padding: '10px 20px',
        borderRadius: 24,
        fontWeight: 600,
        color: color,
        fontSize: 18,
        boxShadow: '0 2px 8px rgba(0,0,0,0.05)',
        display: 'inline-block',
        marginBottom: 12
      }}>{health}%</div>
      <div style={{
        height: 12,
        background: '#e0e0e0',
        borderRadius: 6,
        overflow: 'hidden',
        margin: '0 0 0 0'
      }}>
        <div style={{
          width: `${health}%`,
          height: '100%',
          background: color,
          transition: 'width 0.3s ease',
          borderRadius: 6
        }} />
      </div>
    </div>
  );
};

export default LeafHealthWidget; 