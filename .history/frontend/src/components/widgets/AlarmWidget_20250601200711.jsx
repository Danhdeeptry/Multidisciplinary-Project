import React from 'react';

const AlarmWidget = ({ alarms }) => {
  return (
    <div style={{
      background: '#fff',
      borderRadius: 12,
      boxShadow: '0 2px 12px rgba(0,0,0,0.08)',
      padding: 24,
      minWidth: 400,
      maxWidth: 600,
      margin: '0 auto'
    }}>
      <h2 style={{ fontSize: 20, fontWeight: 700, marginBottom: 16 }}>Alarms</h2>
      <table style={{ width: '100%', borderCollapse: 'collapse' }}>
        <thead>
          <tr style={{ background: '#f5f5f5' }}>
            <th style={{ padding: 8, fontWeight: 600 }}>Time</th>
            <th style={{ padding: 8, fontWeight: 600 }}>Type</th>
            <th style={{ padding: 8, fontWeight: 600 }}>Value</th>
            <th style={{ padding: 8, fontWeight: 600 }}>Message</th>
          </tr>
        </thead>
        <tbody>
          {alarms && alarms.length > 0 ? alarms.map((alarm, idx) => (
            <tr key={idx} style={{ borderBottom: '1px solid #eee' }}>
              <td style={{ padding: 8, color: '#333', fontSize: 14 }}>
                {alarm.alert_timestamp ? new Date(Number(alarm.alert_timestamp)).toLocaleString() : ''}
              </td>
              <td style={{ padding: 8, color: alarm.alert_type === 'CRITICAL' ? 'red' : (alarm.alert_type === 'MAJOR' ? 'orange' : '#333'), fontWeight: 600 }}>
                {alarm.alert_type}
              </td>
              <td style={{ padding: 8, color: '#2196f3', fontWeight: 600 }}>
                {alarm.alert_value}
              </td>
              <td style={{ padding: 8, color: '#555' }}>
                {alarm.alert_message}
              </td>
            </tr>
          )) : (
            <tr>
              <td colSpan={4} style={{ textAlign: 'center', color: '#888', padding: 16 }}>No alarms</td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
};

export default AlarmWidget; 