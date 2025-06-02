import React, { useState } from 'react';
import { FaLightbulb, FaRegLightbulb, FaCog } from 'react-icons/fa';
import AdjustModal from './AdjustModal';

const LightWidget = ({ sunlight, isLightOn, isAutoMode, onSunlightChange, onLightToggle, onAutoModeToggle, onManualLightToggle, farmId }) => {
  const [modalOpen, setModalOpen] = useState(false);
  const [lightValue, setLightValue] = useState(sunlight);

  const handleOpen = () => {
    setLightValue(sunlight);
    setModalOpen(true);
  };
  const handleSave = () => {
    onSunlightChange(lightValue);
    // Cập nhật localStorage và phát event để graph/dashboard cập nhật
    const storageKey = `farmStatus_${farmId?.toUpperCase()}`;
    const local = localStorage.getItem(storageKey);
    let status = { sunlight: lightValue };
    if (local) {
      try {
        status = { ...JSON.parse(local), sunlight: lightValue };
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
      background: 'linear-gradient(120deg, #56ab2f 0%, #a8e063 100%)',
      borderRadius: 18,
      boxShadow: '0 4px 24px rgba(86,171,47,0.10)',
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
          boxShadow: '0 2px 8px rgba(86,171,47,0.10)',
          cursor: 'pointer',
          zIndex: 2
        }}
        title="Adjust"
      >
        <FaCog color="#56ab2f" size={18} />
      </button>
      {/* Left: Info */}
      <div style={{ flex: 1, padding: 24, display: 'flex', flexDirection: 'column', justifyContent: 'center' }}>
        <div style={{ color: '#fff', fontSize: 18, fontWeight: 600, marginBottom: 2 }}>Light ☀️</div>
        <div style={{ color: '#fff', fontSize: 38, fontWeight: 800, lineHeight: 1 }}>{sunlight}<span style={{ fontSize: 22, fontWeight: 400 }}> lux</span></div>
        <div style={{ color: '#fff', fontWeight: 500, fontSize: 15, opacity: 0.85, marginTop: 6 }}>{isLightOn ? 'MANUAL' : 'OFF'}</div>
        <div style={{ color: '#fff', fontWeight: 400, fontSize: 13, opacity: 0.8 }}>{isLightOn ? 'Warnings & fail-safe\nactions on' : 'Light is off'}</div>
        <button
          type="button"
          onClick={onLightToggle}
          style={{
            marginTop: 10,
            background: isLightOn ? 'linear-gradient(90deg, #ffe259 0%, #ffa751 100%)' : '#e0e0e0',
            color: isLightOn ? '#ffb300' : '#888',
            border: 'none',
            borderRadius: 24,
            padding: '6px 18px',
            fontWeight: 700,
            fontSize: 15,
            boxShadow: isLightOn ? '0 2px 8px rgba(255,193,7,0.15)' : 'none',
            cursor: 'pointer',
            transition: 'all 0.3s',
            display: 'inline-flex',
            alignItems: 'center',
            gap: 8
          }}
        >
          {isLightOn ? <FaLightbulb size={18} style={{ filter: 'drop-shadow(0 0 6px #ffe259)' }} /> : <FaRegLightbulb size={18} />}
          {isLightOn ? 'Light ON' : 'Light OFF'}
        </button>
        {/* Only keep the Light ON/OFF button at the bottom */}
        <div style={{ display: 'flex', gap: 10, marginTop: 14 }}>
          <button
            onClick={onManualLightToggle}
            disabled={isAutoMode}
            style={{
              flex: 1,
              background: isLightOn ? '#ffb300' : '#e0e0e0',
              color: isLightOn ? '#fff' : '#666',
              border: 'none',
              borderRadius: 8,
              padding: '8px 0',
              fontWeight: 600,
              fontSize: 15,
              cursor: isAutoMode ? 'not-allowed' : 'pointer',
              opacity: isAutoMode ? 0.6 : 1,
              transition: 'all 0.2s',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              gap: 8
            }}
          >
            {isLightOn ? '💡 Light ON' : '💡 Light OFF'}
          </button>
        </div>
      </div>
      <AdjustModal
        open={modalOpen}
        title="Adjust Light"
        value={lightValue}
        min={0}
        max={1600}
        unit="lux"
        onChange={setLightValue}
        onClose={() => setModalOpen(false)}
        onSave={handleSave}
      />
    </div>
  );
};

export default LightWidget; 