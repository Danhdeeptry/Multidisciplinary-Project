import React, { useState } from 'react';
import AdjustModal from './AdjustModal';
import { FaCog } from 'react-icons/fa';

const HumidityWidget = ({ humidity, onHumidityChange, farmId }) => {
  const [modalOpen, setModalOpen] = useState(false);
  const [humValue, setHumValue] = useState(humidity);

  const handleOpen = () => {
    setHumValue(humidity);
    setModalOpen(true);
  };
  const handleSave = () => {
    onHumidityChange(humValue);
    // Cập nhật localStorage và phát event để graph/dashboard cập nhật
    const storageKey = `farmStatus_${farmId?.toUpperCase()}`;
    const local = localStorage.getItem(storageKey);
    let status = { humidity: humValue };
    if (local) {
      try {
        status = { ...JSON.parse(local), humidity: humValue };
      } catch {
        // Nếu lỗi parse thì giữ nguyên status mặc định
      }
    }
    localStorage.setItem(storageKey, JSON.stringify(status));
    window.dispatchEvent(new Event('farmStatusChanged'));
    setModalOpen(false);
  };

  return (
    <div style={{
      background: 'linear-gradient(120deg, #2193b0 0%, #6dd5ed 100%)',
      borderRadius: 18,
      boxShadow: '0 4px 24px rgba(33,147,176,0.10)',
      padding: 0,
      minHeight: 140,
      display: 'flex',
      alignItems: 'stretch',
      overflow: 'hidden',
      position: 'relative'
    }}>
      {/* Nút bánh răng */}
      <button
        onClick={handleOpen}
        style={{
          position: 'absolute',
          top: 14,
          right: 14,
          background: 'rgba(255,255,255,0.85)',
          border: 'none',
          borderRadius: '50%',
          width: 34,
          height: 34,
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          boxShadow: '0 2px 8px rgba(33,147,176,0.10)',
          cursor: 'pointer',
          zIndex: 2
        }}
        title="Adjust"
      >
        <FaCog color="#2193b0" size={18} />
      </button>
      {/* Left: Info */}
      <div style={{ flex: 1, padding: 24, display: 'flex', flexDirection: 'column', justifyContent: 'center' }}>
        <div style={{ color: '#fff', fontSize: 18, fontWeight: 600, marginBottom: 2 }}>Humidity 💧</div>
        <div style={{ color: '#fff', fontSize: 38, fontWeight: 800, lineHeight: 1 }}>{humidity}<span style={{ fontSize: 22, fontWeight: 400 }}>%</span></div>
        <div style={{ color: '#fff', fontWeight: 500, fontSize: 15, opacity: 0.85, marginTop: 6 }}>AUTOMATIC</div>
        <div style={{ color: '#fff', fontWeight: 400, fontSize: 13, opacity: 0.8 }}>Keeping around<br/>10%</div>
      </div>
      <AdjustModal
        open={modalOpen}
        title="Adjust Humidity"
        value={humValue}
        min={0}
        max={100}
        unit="%"
        onChange={setHumValue}
        onClose={() => setModalOpen(false)}
        onSave={handleSave}
      />
    </div>
  );
};

export default HumidityWidget; 