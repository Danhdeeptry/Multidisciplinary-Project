import { useParams, useNavigate } from "react-router-dom";
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

// Mock leaf status data - in real app, this would come from backend
const mockLeafStatus = {
  FARM1: { health: 95, color: "#4CAF50", status: "Excellent" },
  FARM2: { health: 75, color: "#FFC107", status: "Good" },
  FARM3: { health: 45, color: "#FF5722", status: "Needs Attention" },
};

export default function FarmDetailPage() {
  const { farmId } = useParams();
  const navigate = useNavigate();
  const image = farmImages[farmId?.toUpperCase()] || farm1img;
  const initialStatus = mockStatus[farmId?.toUpperCase()] || { temperature: 0, humidity: 0, sunlight: 0 };
  const leafStatus = mockLeafStatus[farmId?.toUpperCase()] || { health: 0, color: "#757575", status: "Unknown" };

  // Trạng thái hiện tại (hiển thị phía trên)
  const [currentStatus, setCurrentStatus] = useState(initialStatus);
  // Chuyển sunlight thành trạng thái (None, Very Low, Low, Medium, High, Very High)
  const sunlightOptions = ["None", "Very Low", "Low", "Medium", "High", "Very High"];
  // Giá trị điều chỉnh (có thể khác trạng thái hiện tại)
  const [temperature, setTemperature] = useState(initialStatus.temperature);
  const [humidity, setHumidity] = useState(initialStatus.humidity);
  const [sunlightIdx, setSunlightIdx] = useState(
    initialStatus.sunlight >= 95 ? 5 :
    initialStatus.sunlight >= 80 ? 4 :
    initialStatus.sunlight >= 60 ? 3 :
    initialStatus.sunlight >= 40 ? 2 :
    initialStatus.sunlight >= 20 ? 1 :
    0
  );
  const [sunlight, setSunlight] = useState(sunlightOptions[sunlightIdx]);

  const handleSunlightChange = (idx) => {
    setSunlightIdx(idx);
    setSunlight(sunlightOptions[idx]);
  };

  const handleSave = () => {
    setCurrentStatus({ temperature, humidity, sunlight });
    // Lưu vào localStorage để Dashboard có thể đọc được
    // Sau này, thay đoạn này bằng gọi API cập nhật dữ liệu lên server
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
        width: "100%",
        position: "relative"
      }}>
        {/* Nút trở về Dashboard */}
        <button
          onClick={() => navigate("/dashboard")}
          style={{
            position: "absolute",
            top: 20,
            left: 20,
            background: "#2e7d32",
            color: "#fff",
            border: "none",
            borderRadius: 24,
            padding: "8px 18px 8px 12px",
            fontWeight: 600,
            fontSize: 16,
            display: "flex",
            alignItems: "center",
            boxShadow: "0 2px 8px rgba(46,125,50,0.08)",
            cursor: "pointer",
            transition: "background 0.2s, box-shadow 0.2s"
          }}
          onMouseOver={e => e.currentTarget.style.background = "#1b5e20"}
          onMouseOut={e => e.currentTarget.style.background = "#2e7d32"}
        >
          <span style={{ fontSize: 20, marginRight: 8 }}>←</span> Home
        </button>
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

        {/* Leaf Status Monitoring Box */}
        <div style={{ 
          margin: "0 32px 32px 32px", 
          padding: "24px", 
          background: "#f8f9fa", 
          borderRadius: "16px",
          boxShadow: "0 4px 12px rgba(0,0,0,0.1)",
          border: "2px solid #e0e0e0",
          position: "relative",
          zIndex: 1
        }}>
          <h2 style={{ 
            fontSize: 24, 
            fontWeight: 700, 
            color: "#2e7d32", 
            marginBottom: 24, 
            textAlign: "center",
            borderBottom: "2px solid #e0e0e0",
            paddingBottom: "12px"
          }}>Leaf Health Status</h2>
          
          <div style={{ 
            display: "flex", 
            alignItems: "center", 
            justifyContent: "space-between",
            marginBottom: 20,
            padding: "0 12px"
          }}>
            <div style={{ display: "flex", alignItems: "center", gap: 16 }}>
              <div style={{ 
                width: 48, 
                height: 48, 
                borderRadius: "50%", 
                background: leafStatus.color,
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                color: "#fff",
                fontSize: 24,
                boxShadow: "0 2px 8px rgba(0,0,0,0.1)"
              }}>
                🌿
              </div>
              <div>
                <div style={{ fontWeight: 600, color: "#333", fontSize: 16 }}>Health Status</div>
                <div style={{ color: leafStatus.color, fontWeight: 700, fontSize: 18 }}>{leafStatus.status}</div>
              </div>
            </div>
            <div style={{ 
              background: "#fff", 
              padding: "10px 20px", 
              borderRadius: "24px",
              fontWeight: 600,
              color: leafStatus.color,
              fontSize: 18,
              boxShadow: "0 2px 8px rgba(0,0,0,0.05)"
            }}>
              {leafStatus.health}%
            </div>
          </div>

          <div style={{ 
            height: 12, 
            background: "#e0e0e0", 
            borderRadius: 6,
            overflow: "hidden",
            margin: "0 12px"
          }}>
            <div style={{ 
              width: `${leafStatus.health}%`, 
              height: "100%", 
              background: leafStatus.color,
              transition: "width 0.3s ease",
              borderRadius: 6
            }} />
          </div>
        </div>

        {/* Trạng thái hiện tại */}
        <div style={{ 
          display: "flex", 
          justifyContent: "space-around", 
          marginBottom: 32,
          position: "relative",
          zIndex: 2,
          marginTop: "-100px",
          padding: "0 32px"
        }}>
          <div style={{ 
            textAlign: "center",
            background: "#fff",
            padding: "16px",
            borderRadius: "12px",
            boxShadow: "0 4px 12px rgba(0,0,0,0.1)",
            width: "30%"
          }}>
            <div style={{ fontSize: 28 }}>🌡️</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Temperature</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.temperature}°C</div>
          </div>
          <div style={{ 
            textAlign: "center",
            background: "#fff",
            padding: "16px",
            borderRadius: "12px",
            boxShadow: "0 4px 12px rgba(0,0,0,0.1)",
            width: "30%"
          }}>
            <div style={{ fontSize: 28 }}>💧</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Humidity</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.humidity}%</div>
          </div>
          <div style={{ 
            textAlign: "center",
            background: "#fff",
            padding: "16px",
            borderRadius: "12px",
            boxShadow: "0 4px 12px rgba(0,0,0,0.1)",
            width: "30%"
          }}>
            <div style={{ fontSize: 28 }}>☀️</div>
            <div style={{ color: "#333", fontWeight: 600 }}>Sunlight</div>
            <div style={{ color: "#2e7d32", fontWeight: 700, fontSize: 20 }}>{currentStatus.sunlight}</div>
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
            <label style={{ fontWeight: 600, color: "#333", marginBottom: 8, display: "block" }}>☀️ Sunlight</label>
            <div style={{ display: "flex", justifyContent: "center", alignItems: "center", gap: 0 }}>
              <input
                type="range"
                min={0}
                max={5}
                step={1}
                value={sunlightIdx}
                onChange={e => handleSunlightChange(Number(e.target.value))}
                style={{ width: "90%" }}
              />
            </div>
            <div style={{ display: "flex", justifyContent: "space-between", marginTop: 8, width: "90%", marginLeft: "5%" }}>
              {sunlightOptions.map((opt, idx) => (
                <span key={opt} style={{ fontSize: 12, color: idx === sunlightIdx ? "#2e7d32" : "#888", fontWeight: idx === sunlightIdx ? 700 : 400 }}>{opt}</span>
              ))}
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