import React, { useState, useEffect } from "react";
import axios from "axios";
import Clock from '../components/Clock';
import useNavigation from "../components/Navigate";
import farm1img from "../images/farm1.jpg";
import farm2img from "../images/farm2.jpg";
import farm3img from "../images/farm3.jpg";
import { useNavigate } from "react-router-dom";
import AlarmWidget from '../components/widgets/AlarmWidget';

const farms = [
  {
    label: "FARM1",
    image: farm1img,
  },
  {
    label: "FARM2",
    image: farm2img,
  },
  {
    label: "FARM3",
    image: farm3img,
  },
];

const FarmCard = ({ label, image }) => {
  const [farmData, setFarmData] = useState({ temperature: "N/A", humidity: "N/A", sunlight: "N/A" });
  const navigate = useNavigate();

  // Hàm lấy dữ liệu từ localStorage (nếu có)
  const getFarmData = () => {
    const local = localStorage.getItem(`farmStatus_${label}`);
    if (local) {
      const { temperature, humidity, sunlight } = JSON.parse(local);
      return {
        temperature: `${temperature}°C`,
        humidity: `${humidity}%`,
        sunlight: (typeof sunlight === 'number' || /^\d+$/.test(sunlight)) ? `${sunlight} lux` : (sunlight === undefined ? "N/A" : sunlight)
      };
    }
    // Nếu không có localStorage, lấy mockData dạng số lux
    const mockData = {
      FARM1: {
        temperature: "27°C",
        humidity: "65%",
        sunlight: "90 lux",
      },
      FARM2: {
        temperature: "24°C",
        humidity: "72%",
        sunlight: "60 lux",
      },
      FARM3: {
        temperature: "29°C",
        humidity: "58%",
        sunlight: "95 lux",
      }
    };
    return mockData[label] || { temperature: "N/A", humidity: "N/A", sunlight: "N/A" };
  };

  useEffect(() => {
  let intervalId;

  const fetchData = async () => {
    if (label === "FARM1") {
      try {
        const response = await axios.get("http://127.0.0.1:8000/YoloFarms");
        const { temperature, humidity, sunlight } = response.data;
        const newData = {
          temperature: `${temperature}°C`,
          humidity: `${humidity}%`,
          sunlight: (typeof sunlight === 'number' || /^\d+$/.test(sunlight)) ? `${sunlight} lux` : (sunlight === undefined ? "N/A" : sunlight)
        };
        setFarmData(newData);
        localStorage.setItem("farmStatus_FARM1", JSON.stringify(newData));
      } catch (error) {
        console.error("Error fetching Farm 1 data:", error);
      }
    } else {
      const mockData = {
        FARM2: {
          temperature: "24°C",
          humidity: "72%",
          sunlight: "60 lux",
        },
        FARM3: {
          temperature: "29°C",
          humidity: "58%",
          sunlight: "95 lux",
        }
      };
      setFarmData(mockData[label] || { temperature: "N/A", humidity: "N/A", sunlight: "N/A" });
    }
  };

  // Initial fetch
  fetchData();

  // Set interval only for FARM1
  if (label === "FARM1") {
    intervalId = setInterval(fetchData, 100); // Every 5 seconds
  }

  // Cleanup on unmount
  return () => {
    if (intervalId) clearInterval(intervalId);
  };
}, [label]);
  
  return (
    <div 
      style={{
        backgroundColor: "white",
        borderRadius: "8px",
        overflow: "hidden",
        boxShadow: "0 2px 8px rgba(0,0,0,0.1)",
        transition: "transform 0.3s, box-shadow 0.3s",
        cursor: "pointer",
        width: "100%",
        height: "100%"
      }}
    >
      <div style={{ position: "relative" }}>
        <img 
          src={image} 
          alt={label} 
          style={{ 
            width: "100%", 
            height: "180px", 
            objectFit: "cover" 
          }}
        />
        <div style={{ 
          position: "absolute", 
          top: "10px", 
          left: "10px", 
          backgroundColor: "#2e7d32", 
          color: "white", 
          padding: "4px 10px", 
          borderRadius: "4px",
          fontWeight: "500"
        }}>
          {label}
        </div>
      </div>
      
      <div style={{ padding: "16px" }}>
        <h3 style={{ 
          fontSize: "18px", 
          marginBottom: "12px", 
          color: "#333",
          fontWeight: "600" 
        }}>
          Farm Status
        </h3>
        
        <div style={{ marginBottom: "16px" }}>
          <div style={{ 
            display: "flex", 
            alignItems: "center", 
            marginBottom: "8px" 
          }}>
            <span style={{ marginRight: "8px" }}>🌡️</span>
            <span>Temperature: {farmData.temperature}</span>
          </div>
          <div style={{ 
            display: "flex", 
            alignItems: "center", 
            marginBottom: "8px" 
          }}>
            <span style={{ marginRight: "8px" }}>💧</span>
            <span>Humidity: {farmData.humidity}</span>
          </div>
          <div style={{ 
            display: "flex", 
            alignItems: "center", 
            marginBottom: "8px" 
          }}>
            <span style={{ marginRight: "8px" }}>☀️</span>
            <span>Sunlight: {farmData.sunlight}</span>
          </div>
        </div>
        
        <button style={{ 
          width: "100%", 
          backgroundColor: "#2e7d32", 
          color: "white", 
          border: "none", 
          padding: "8px 16px", 
          borderRadius: "4px", 
          cursor: "pointer",
          display: "flex",
          justifyContent: "space-between",
          alignItems: "center",
          fontSize: "14px",
          fontWeight: "500",
          transition: "background-color 0.3s"
        }}
        onClick={() => navigate(`/farm/${label}`)}
        onMouseOver={(e) => {
          e.currentTarget.style.backgroundColor = "#1b5e20";
        }}
        onMouseOut={(e) => {
          e.currentTarget.style.backgroundColor = "#2e7d32";
        }}
        >
          <span>View Details</span>
          <span>→</span>
        </button>
      </div>
    </div>
  );
};


const Dashboard = () => {
  const { goToLogin } = useNavigation();
  const navigate = useNavigate();
  const [alarms, setAlarms] = useState([]);
  
  // Function to navigate to the farm page
  const goToFarm = () => {
    window.location.href = "/farm";
  };
  
  useEffect(() => {
    // Giả lập fetch từ localStorage hoặc API
    const data = [
      {
        alert_message: "Light intensity returned to normal",
        alert_timestamp: 1748669271582,
        alert_type: "NORMAL",
        alert_value: 165.5
      },
      {
        alert_message: "Light intensity too low",
        alert_timestamp: 1748668000000,
        alert_type: "LOW",
        alert_value: 20.1
      }
    ];
    setAlarms(data);
  }, []);

  return (
    <div style={{ 
      display: "flex", 
      height: "100vh", 
      fontFamily: "sans-serif" 
    }}>
      {/* Sidebar */}
      <aside style={{
        width: "250px",
        backgroundColor: "#2e7d32",
        color: "white",
        padding: "20px 0",
        display: "flex",
        flexDirection: "column"
      }}>
        <h2 style={{ 
          padding: "0 20px", 
          marginBottom: "30px", 
          fontSize: "24px", 
          fontWeight: "bold", 
          textAlign: "center" 
        }}>
          Smart Farm
        </h2>
        
        <nav>
          <a href="#" style={{ 
            display: "block",
            padding: "12px 20px",
            color: "white", 
            textDecoration: "none",
            backgroundColor: "rgba(255, 255, 255, 0.1)",
            marginBottom: "5px",
            fontWeight: "500"
          }}>
            Dashboard
          </a>
          <a href="#" style={{ 
            display: "block",
            padding: "12px 20px",
            color: "white", 
            textDecoration: "none",
            transition: "background-color 0.2s",
            marginBottom: "5px"
          }}
          onMouseOver={(e) => {
            e.currentTarget.style.backgroundColor = "rgba(255, 255, 255, 0.1)";
          }}
          onMouseOut={(e) => {
            e.currentTarget.style.backgroundColor = "transparent";
          }}
          >
            Sensors
          </a>
          <a href="#" style={{ 
            display: "block",
            padding: "12px 20px",
            color: "white", 
            textDecoration: "none",
            transition: "background-color 0.2s",
            marginBottom: "5px"
          }}
          onMouseOver={(e) => {
            e.currentTarget.style.backgroundColor = "rgba(255, 255, 255, 0.1)";
          }}
          onMouseOut={(e) => {
            e.currentTarget.style.backgroundColor = "transparent";
          }}
          >
            Statistics
          </a>
          <a href="#" style={{ 
            display: "block",
            padding: "12px 20px",
            color: "white", 
            textDecoration: "none",
            transition: "background-color 0.2s",
            marginBottom: "5px"
          }}
          onMouseOver={(e) => {
            e.currentTarget.style.backgroundColor = "rgba(255, 255, 255, 0.1)";
          }}
          onMouseOut={(e) => {
            e.currentTarget.style.backgroundColor = "transparent";
          }}
          >
            Settings
          </a>
          <a href="#" style={{ 
            display: "block",
            padding: "12px 20px",
            color: "white", 
            textDecoration: "none",
            transition: "background-color 0.2s",
            marginTop: "auto"
          }}
          onClick={goToLogin}
          onMouseOver={(e) => {
            e.currentTarget.style.backgroundColor = "rgba(255, 255, 255, 0.1)";
          }}
          onMouseOut={(e) => {
            e.currentTarget.style.backgroundColor = "transparent";
          }}
          >
            Logout
          </a>
        </nav>
      </aside>

      {/* Main Content */}
      <main style={{ 
        flex: 1, 
        backgroundColor: "#f5f8f5", 
        padding: "40px", 
        overflowY: "auto" 
      }}>
        <div style={{ 
          display: "flex", 
          justifyContent: "space-between", 
          alignItems: "center", 
          marginBottom: "30px" 
        }}>
          <h1 style={{ 
            fontSize: "28px", 
            color: "#2e7d32",
            fontWeight: "bold" 
          }}>
            Welcome Farmer
          </h1>
          <div style={{
            backgroundColor: "white",
            padding: "10px 16px",
            borderRadius: "8px",
            boxShadow: "0 2px 6px rgba(0,0,0,0.1)"
          }}>
            <Clock />
          </div>
        </div>
        
        <h2 style={{ 
          fontSize: "22px", 
          marginBottom: "20px", 
          color: "#2e7d32",
          fontWeight: 800,
          letterSpacing: 1
        }}>
          Your Farms
        </h2>
        
        {/* Farm cards in a row with equal sizing */}
        <div style={{ 
          display: "grid", 
          gridTemplateColumns: "repeat(3, 1fr)", 
          gap: "20px",
          marginBottom: "40px"
        }}>
          {farms.map((farm) => (
            <FarmCard 
              key={farm.label} 
              label={farm.label} 
              image={farm.image} 
            />
          ))}
        </div>
        
        {/* Recent Activity & Alarms side by side */}
        <div style={{
          display: "flex",
          gap: "24px",
          marginTop: "40px",
          alignItems: "flex-start"
        }}>
          {/* Recent Activity */}
          <div style={{
            flex: 1,
            backgroundColor: "white",
            borderRadius: "8px",
            padding: "20px",
            boxShadow: "0 2px 6px rgba(0,0,0,0.1)"
          }}>
            <h2 style={{
              fontSize: "20px",
              marginBottom: "20px",
              color: "#2e7d32",
              fontWeight: 800,
              letterSpacing: 1
            }}>
              Recent Activity
            </h2>
            <div style={{
              borderBottom: "1px solid #e0e0e0",
              paddingBottom: "12px",
              marginBottom: "12px"
            }}>
              <div style={{ display: "flex", justifyContent: "space-between" }}>
                <div>
                  <p style={{ fontWeight: "500", color: "#333" }}>Irrigation system activated</p>
                  <p style={{ fontSize: "14px", color: "#666", marginTop: "4px" }}>FARM1</p>
                </div>
                <span style={{ fontSize: "14px", color: "#666" }}>10 minutes ago</span>
              </div>
            </div>
            <div style={{
              borderBottom: "1px solid #e0e0e0",
              paddingBottom: "12px",
              marginBottom: "12px"
            }}>
              <div style={{ display: "flex", justifyContent: "space-between" }}>
                <div>
                  <p style={{ fontWeight: "500", color: "#333" }}>Temperature alert</p>
                  <p style={{ fontSize: "14px", color: "#666", marginTop: "4px" }}>FARM2 - High temperature detected</p>
                </div>
                <span style={{ fontSize: "14px", color: "#666" }}>1 hour ago</span>
              </div>
            </div>
            <div>
              <div style={{ display: "flex", justifyContent: "space-between" }}>
                <div>
                  <p style={{ fontWeight: "500", color: "#333" }}>Soil sensor maintenance</p>
                  <p style={{ fontSize: "14px", color: "#666", marginTop: "4px" }}>FARM3 - Completed</p>
                </div>
                <span style={{ fontSize: "14px", color: "#666" }}>Yesterday</span>
              </div>
            </div>
          </div>
          {/* Alarms */}
          <div style={{ flex: 1, minWidth: 400, maxWidth: 600 }}>
            <AlarmWidget alarms={alarms} />
          </div>
        </div>
      </main>
    </div>
  );
};

export default Dashboard;