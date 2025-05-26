import React, { useState } from 'react';

const tabStyle = {
  display: 'flex',
  borderBottom: '2px solid #e0e0e0',
  marginBottom: 24,
  background: '#f8f9fa',
  borderRadius: '16px 16px 0 0',
  overflow: 'hidden',
};
const tabButtonStyle = isActive => ({
  flex: 1,
  padding: '16px 0',
  fontWeight: 700,
  fontSize: 18,
  color: isActive ? '#fff' : '#2e7d32',
  background: isActive ? 'linear-gradient(90deg, #43e97b 0%, #38f9d7 100%)' : 'none',
  border: 'none',
  outline: 'none',
  cursor: 'pointer',
  transition: 'all 0.25s',
  boxShadow: isActive ? '0 4px 16px rgba(46,125,50,0.10)' : 'none',
  borderRadius: isActive ? '16px 16px 0 0' : '0',
});

export default function Tabs({ tabs, children }) {
  const [active, setActive] = useState(0);
  return (
    <div>
      <div style={tabStyle}>
        {tabs.map((tab, idx) => (
          <button
            key={tab}
            style={tabButtonStyle(active === idx)}
            onClick={() => setActive(idx)}
            aria-selected={active === idx}
          >
            {tab}
          </button>
        ))}
      </div>
      <div style={{ background: '#fff', borderRadius: '0 0 18px 18px', boxShadow: '0 4px 24px rgba(46,125,50,0.08)', padding: 32, minHeight: 300 }}>
        {Array.isArray(children) ? children[active] : children}
      </div>
    </div>
  );
} 