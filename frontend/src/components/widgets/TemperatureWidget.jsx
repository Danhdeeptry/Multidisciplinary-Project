import React, { useState } from 'react';
import AdjustModal from './AdjustModal';
import { FaCog } from 'react-icons/fa';

const TemperatureWidget = ({ temperature, onTemperatureChange, farmId }) => {
  const [modalOpen, setModalOpen] = useState(false);
  const [tempValue, setTempValue] = useState(temperature);

  const handleOpen = () => {
    setTempValue(temperature);
    setModalOpen(true);
  };
  const handleSave = () => {
    onTemperatureChange(tempValue);
    // Cập nhật localStorage và phát event để graph/dashboard cập nhật
    const storageKey = `farmStatus_${farmId?.toUpperCase()}`;
    const local = localStorage.getItem(storageKey);
    let status = { temperature: tempValue };
    if (local) {
      try {
        status = { ...JSON.parse(local), temperature: tempValue };
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
      background: 'linear-gradient(120deg, #ffb347 0%, #ffcc33 100%)',
      borderRadius: 18,
      boxShadow: '0 4px 24px rgba(255,193,7,0.10)',
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
          boxShadow: '0 2px 8px rgba(255,193,7,0.10)',
          cursor: 'pointer',
          zIndex: 2
        }}
        title="Adjust"
      >
        <FaCog color="#ff9800" size={18} />
      </button>
      {/* Left: Info */}
      <div style={{ flex: 1, padding: 24, display: 'flex', flexDirection: 'column', justifyContent: 'center' }}>
        <div style={{ color: '#fff', fontSize: 18, fontWeight: 600, marginBottom: 2 }}>Temperature 🌡️</div>
        <div style={{ color: '#fff', fontSize: 38, fontWeight: 800, lineHeight: 1 }}>{temperature}<span style={{ fontSize: 22, fontWeight: 400 }}>°C</span></div>
        <div style={{ color: '#fff', fontWeight: 500, fontSize: 15, opacity: 0.85, marginTop: 6 }}>SCHEDULED</div>
        <div style={{ color: '#fff', fontWeight: 400, fontSize: 13, opacity: 0.8 }}>Sunscreens between<br/>11:00 - 16:00</div>
      </div>
      <AdjustModal
        open={modalOpen}
        title="Adjust Temperature"
        value={tempValue}
        min={0}
        max={50}
        unit="°C"
        onChange={setTempValue}
        onClose={() => setModalOpen(false)}
        onSave={handleSave}
      />
    </div>
  );
};

export default TemperatureWidget; 