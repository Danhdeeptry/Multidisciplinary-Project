import { useParams } from "react-router-dom";
import { useState } from "react";
import farm1img from "../images/farm1.jpg";
import farm2img from "../images/farm2.jpg";
import farm3img from "../images/farm3.jpg";

const farmImages = {
  FARM1: farm1img,
  FARM2: farm2img,
  FARM3: farm3img,
};

const mockStatus = {
  FARM1: { temperature: 27, humidity: 65, sunlight: 90 },
  FARM2: { temperature: 24, humidity: 72, sunlight: 60 },
  FARM3: { temperature: 29, humidity: 58, sunlight: 95 },
};

export default function FarmDetailPage() {
  const { farmId } = useParams();
  const image = farmImages[farmId?.toUpperCase()] || farm1img;
  const initialStatus = mockStatus[farmId?.toUpperCase()] || { temperature: 0, humidity: 0, sunlight: 0 };

  // Trạng thái hiện tại (hiển thị phía trên)
  const [currentStatus, setCurrentStatus] = useState(initialStatus);
  // Giá trị điều chỉnh (có thể khác trạng thái hiện tại)
  const [temperature, setTemperature] = useState(initialStatus.temperature);
  const [humidity, setHumidity] = useState(initialStatus.humidity);
  const [sunlight, setSunlight] = useState(initialStatus.sunlight);

  const handleSave = () => {
    setCurrentStatus({ temperature, humidity, sunlight });
    // Lưu vào localStorage
    localStorage.setItem(
      `farmStatus_${farmId?.toUpperCase()}`,
      JSON.stringify({ temperature, humidity, sunlight })
    );
  };

  return (
    <div style={{
      minHeight: "100vh",
      background: "#f5f8f5",
      display: "flex",
      justifyContent: "center",
      alignItems: "flex-start",
      paddingTop: 40
    }}>
      <div style={{
        background: "#fff",
        borderRadius: 16,
        boxShadow: "0 4px 16px rgba(0,0,0,0.10)",
        padding: 0,
        minWidth: 400,
        maxWidth: 520,
        width: "100%"
      }}>
        {/* Ảnh bìa */}
        <div style={{ width: "100%", height: 220, overflow: "hidden", borderTopLeftRadius: 16, borderTopRightRadius: 16 }}>
          <img src={image} alt={farmId} style={{ width: "100%", height: "100%", objectFit: "cover" }} />
        </div>
        {/* Tiêu đề */}
        <h1 style={{
          fontSize: 28,
          fontWeight: 700,
          color: "#2e7d32",
          margin: "32px 0 16px 0",
          textAlign: "center"
        }}>Farm: {farmId?.toUpperCase()}</h1>
        {/* Trạng thái hiện tại */}
        <div style={{ display: "flex", justifyContent: "space-around", marginBottom: 32 }}>
          <div style={{ textAlign: "center" }}>
            <div style={{ fontSize: 28 }}>🌡️</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Temperature</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.temperature}°C</div>
          </div>
          <div style={{ textAlign: "center" }}>
            <div style={{ fontSize: 28 }}>💧</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Humidity</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.humidity}%</div>
          </div>
          <div style={{ textAlign: "center" }}>
            <div style={{ fontSize: 28 }}>☀️</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Sunlight</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.sunlight}%</div>
          </div>
        </div>
        {/* Điều chỉnh */}
        <div style={{ padding: 32 }}>
          <h2 style={{ fontSize: 20, fontWeight: 700, color: "#2e7d32", marginBottom: 24, textAlign: "center" }}>Adjust Parameters</h2>
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
          onClick={handleSave}
          >Save</button>
        </div>
      </div>
    </div>
  );
} 