import React from 'react';
import { FaExclamationTriangle, FaCheckCircle, FaInfoCircle } from 'react-icons/fa';

const getSeverityStyle = (type) => {
  switch (type) {
    case 'CRITICAL':
      return { color: '#fff', background: '#e53935', icon: <FaExclamationTriangle style={{ color: '#fff', marginRight: 6 }} /> };
    case 'MAJOR':
      return { color: '#fff', background: '#ffb300', icon: <FaExclamationTriangle style={{ color: '#fff', marginRight: 6 }} /> };
    case 'NORMAL':
      return { color: '#388e3c', background: '#e8f5e9', icon: <FaCheckCircle style={{ color: '#388e3c', marginRight: 6 }} /> };
    case 'LOW':
      return { color: '#1976d2', background: '#e3f2fd', icon: <FaInfoCircle style={{ color: '#1976d2', marginRight: 6 }} /> };
    default:
      return { color: '#333', background: '#f5f5f5', icon: <FaInfoCircle style={{ color: '#888', marginRight: 6 }} /> };
  }
};

const AlarmWidget = ({ alarms }) => {
  return (
    <div style={{
      background: '#fff',
      borderRadius: 16,
      boxShadow: '0 4px 24px rgba(0,0,0,0.10)',
      padding: 0,
      minWidth: 400,
      maxWidth: 600,
      margin: '0 auto',
      overflow: 'hidden'
    }}>
      <div style={{
        padding: '18px 24px',
        borderBottom: '1px solid #eee',
        background: '#f7fafc'
      }}>
        <h2 style={{ fontSize: 20, fontWeight: 800, margin: 0, color: '#2e7d32', letterSpacing: 1 }}>Alarms</h2>
      </div>
      <table style={{ width: '100%', borderCollapse: 'separate', borderSpacing: 0 }}>
        <thead>
          <tr>
            <th style={{ padding: '12px 8px', fontWeight: 700, color: '#888', background: '#f7fafc', textAlign: 'left' }}>Time</th>
            <th style={{ padding: '12px 8px', fontWeight: 700, color: '#888', background: '#f7fafc', textAlign: 'left' }}>Type</th>
            <th style={{ padding: '12px 8px', fontWeight: 700, color: '#888', background: '#f7fafc', textAlign: 'left' }}>Value</th>
            <th style={{ padding: '12px 8px', fontWeight: 700, color: '#888', background: '#f7fafc', textAlign: 'left' }}>Message</th>
          </tr>
        </thead>
        <tbody>
          {alarms && alarms.length > 0 ? alarms.map((alarm, idx) => {
            const sev = getSeverityStyle(alarm.alert_type);
            return (
              <tr
                key={idx}
                style={{
                  background: sev.background,
                  color: sev.color,
                  transition: 'background 0.2s',
                  borderBottom: '1px solid #f0f0f0',
                  cursor: 'pointer'
                }}
                onMouseOver={e => e.currentTarget.style.background = '#f1f8e9'}
                onMouseOut={e => e.currentTarget.style.background = sev.background}
              >
                <td style={{ padding: '12px 8px', fontWeight: 500 }}>
                  {alarm.alert_timestamp ? new Date(Number(alarm.alert_timestamp)).toLocaleString() : ''}
                </td>
                <td style={{ padding: '12px 8px', fontWeight: 700, display: 'flex', alignItems: 'center' }}>
                  {sev.icon}
                  {alarm.alert_type}
                </td>
                <td style={{ padding: '12px 8px', fontWeight: 700, color: sev.color, fontSize: 16 }}>
                  {alarm.alert_value}
                </td>
                <td style={{ padding: '12px 8px', fontWeight: 500 }}>
                  {alarm.alert_message}
                </td>
              </tr>
            );
          }) : (
            <tr>
              <td colSpan={4} style={{ textAlign: 'center', color: '#888', padding: 20 }}>No alarms</td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
};

export default AlarmWidget; 