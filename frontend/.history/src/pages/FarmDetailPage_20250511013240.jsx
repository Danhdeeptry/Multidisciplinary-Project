import { useParams } from "react-router-dom";
import { useState } from "react";

export default function FarmDetailPage() {
  const { farmId } = useParams();
  // Mock data, sau này sẽ lấy từ API
  const [temperature, setTemperature] = useState(25);
  const [humidity, setHumidity] = useState(60);
  const [sunlight, setSunlight] = useState(50);

  return (
    <div style={{
      minHeight: "100vh",
      background: "#f5f8f5",
      display: "flex",
      justifyContent: "center",
      alignItems: "center"
    }}>
      <div style={{
        background: "#fff",
        borderRadius: 16,
        boxShadow: "0 4px 16px rgba(0,0,0,0.10)",
        padding: 40,
        minWidth: 400,
        maxWidth: 480
      }}>
        <h1 style={{
          fontSize: 28,
          fontWeight: 700,
          color: "#2e7d32",
          marginBottom: 32,
          textAlign: "center"
        }}>Farm: {farmId?.toUpperCase()}</h1>
        <div style={{ marginBottom: 32 }}>
          <label style={{ fontWeight: 600, color: "#333", marginBottom: 8, display: "block" }}>🌡️ Temperature (°C)</label>
          <div style={{ display: "flex", alignItems: "center", gap: 16 }}>
            <input type="number" min={0} max={50} value={temperature} onChange={e => setTemperature(Number(e.target.value))} style={{ border: "1px solid #e0e0e0", borderRadius: 6, padding: "6px 12px", width: 80, fontSize: 16 }} />
            <input type="range" min={0} max={50} value={temperature} onChange={e => setTemperature(Number(e.target.value))} style={{ flex: 1 }} />
            <span style={{ minWidth: 32, textAlign: "right", color: "#2e7d32", fontWeight: 600 }}>{temperature}°C</span>
          </div>
        </div>
        <div style={{ marginBottom: 32 }}>
          <label style={{ fontWeight: 600, color: "#333", marginBottom: 8, display: "block" }}>💧 Humidity (%)</label>
          <div style={{ display: "flex", alignItems: "center", gap: 16 }}>
            <input type="number" min={0} max={100} value={humidity} onChange={e => setHumidity(Number(e.target.value))} style={{ border: "1px solid #e0e0e0", borderRadius: 6, padding: "6px 12px", width: 80, fontSize: 16 }} />
            <input type="range" min={0} max={100} value={humidity} onChange={e => setHumidity(Number(e.target.value))} style={{ flex: 1 }} />
            <span style={{ minWidth: 32, textAlign: "right", color: "#2e7d32", fontWeight: 600 }}>{humidity}%</span>
          </div>
        </div>
        <div style={{ marginBottom: 32 }}>
          <label style={{ fontWeight: 600, color: "#333", marginBottom: 8, display: "block" }}>☀️ Sunlight (%)</label>
          <div style={{ display: "flex", alignItems: "center", gap: 16 }}>
            <input type="number" min={0} max={100} value={sunlight} onChange={e => setSunlight(Number(e.target.value))} style={{ border: "1px solid #e0e0e0", borderRadius: 6, padding: "6px 12px", width: 80, fontSize: 16 }} />
            <input type="range" min={0} max={100} value={sunlight} onChange={e => setSunlight(Number(e.target.value))} style={{ flex: 1 }} />
            <span style={{ minWidth: 32, textAlign: "right", color: "#2e7d32", fontWeight: 600 }}>{sunlight}%</span>
          </div>
        </div>
        <button style={{
          width: "100%",
          background: "#2e7d32",
          color: "#fff",
          border: "none",
          padding: "12px 0",
          borderRadius: 8,
          fontWeight: 600,
          fontSize: 18,
          boxShadow: "0 2px 8px rgba(46,125,50,0.08)",
          cursor: "pointer",
          transition: "background 0.2s"
        }}
        onMouseOver={e => e.currentTarget.style.background = "#1b5e20"}
        onMouseOut={e => e.currentTarget.style.background = "#2e7d32"}
        >Save</button>
      </div>
    </div>
  );
} 