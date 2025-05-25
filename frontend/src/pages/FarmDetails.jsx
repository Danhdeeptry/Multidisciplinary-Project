import React, { useState, useEffect, useRef } from 'react';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  TimeScale,
  Filler
} from 'chart.js';
import { Line } from 'react-chartjs-2';
import 'chartjs-adapter-date-fns';
import { useNavigate, useParams } from 'react-router-dom';
import { FaLeaf } from 'react-icons/fa';

// Register ChartJS components
ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  TimeScale,
  Filler
);

const mockStatus = {
  FARM1: { temperature: 27, humidity: 65, sunlight: 90 },
  FARM2: { temperature: 24, humidity: 72, sunlight: 60 },
  FARM3: { temperature: 29, humidity: 58, sunlight: 95 },
};

const getGradient = (ctx, colorTop, colorBottom) => {
  const gradient = ctx.createLinearGradient(0, 0, 0, 300);
  gradient.addColorStop(0, colorTop);
  gradient.addColorStop(1, colorBottom);
  return gradient;
};

const FarmDetails = () => {
  const { farmId } = useParams();
  const navigate = useNavigate();
  const storageKey = `farmStatus_${farmId?.toUpperCase()}`;
  // Lấy trạng thái từ localStorage nếu có, nếu không thì lấy mockStatus
  const getInitialStatus = () => {
    const local = localStorage.getItem(storageKey);
    if (local) return JSON.parse(local);
    return mockStatus[farmId?.toUpperCase()] || { temperature: 0, humidity: 0, sunlight: 0 };
  };
  const [appliedStatus, setAppliedStatus] = useState(getInitialStatus());
  const [currentStatus, setCurrentStatus] = useState(getInitialStatus());
  const [temperature, setTemperature] = useState(currentStatus.temperature);
  const [humidity, setHumidity] = useState(currentStatus.humidity);
  // sunlight luôn là số lux
  const [sunlight, setSunlight] = useState(currentStatus.sunlight);

  // Khi farmId thay đổi (chuyển farm), cập nhật lại state từ localStorage
  useEffect(() => {
    const status = getInitialStatus();
    setAppliedStatus(status);
    setCurrentStatus(status);
    setTemperature(status.temperature);
    setHumidity(status.humidity);
    setSunlight(status.sunlight);
  }, [farmId]);

  // Chart data state
  const [chartData, setChartData] = useState({
    temperature: {
      labels: [],
      datasets: [{
        label: 'Temperature (°C)',
        data: [],
        borderColor: 'rgb(255, 99, 132)',
        backgroundColor: 'rgba(255, 99, 132, 0.2)',
        pointBackgroundColor: 'rgb(255, 99, 132)',
        pointRadius: 5,
        pointHoverRadius: 8,
        fill: true,
        tension: 0.4,
        borderWidth: 3,
        shadowOffsetX: 0,
        shadowOffsetY: 4,
        shadowBlur: 10,
        shadowColor: 'rgba(255,99,132,0.3)'
      }]
    },
    humidity: {
      labels: [],
      datasets: [{
        label: 'Humidity (%)',
        data: [],
        borderColor: 'rgb(53, 162, 235)',
        backgroundColor: 'rgba(53, 162, 235, 0.2)',
        pointBackgroundColor: 'rgb(53, 162, 235)',
        pointRadius: 5,
        pointHoverRadius: 8,
        fill: true,
        tension: 0.4,
        borderWidth: 3,
        shadowOffsetX: 0,
        shadowOffsetY: 4,
        shadowBlur: 10,
        shadowColor: 'rgba(53,162,235,0.3)'
      }]
    },
    sunlight: {
      labels: [],
      datasets: [{
        label: 'Sunlight (lux)',
        data: [],
        borderColor: 'rgb(255, 206, 86)',
        backgroundColor: 'rgba(255, 206, 86, 0.2)',
        pointBackgroundColor: 'rgb(255, 206, 86)',
        pointRadius: 5,
        pointHoverRadius: 8,
        fill: true,
        tension: 0.4,
        borderWidth: 3,
        shadowOffsetX: 0,
        shadowOffsetY: 4,
        shadowBlur: 10,
        shadowColor: 'rgba(255,206,86,0.3)'
      }]
    }
  });

  // Refs for chart instances
  const tempRef = useRef();
  const humRef = useRef();
  const sunRef = useRef();

  // Chart options
  const baseOptions = {
    responsive: true,
    maintainAspectRatio: false,
    animation: {
      duration: 1200,
      easing: 'easeInOutQuart',
    },
    scales: {
      x: {
        type: 'time',
        time: {
          unit: 'minute',
          displayFormats: {
            minute: 'HH:mm'
          }
        },
        title: {
          display: true,
          text: 'Time',
          color: '#888',
          font: { weight: 'bold' }
        },
        grid: { color: '#eee' }
      },
      y: {
        beginAtZero: true,
        grid: { color: '#eee' },
        ticks: { color: '#888' }
      }
    },
    plugins: {
      legend: {
        position: 'top',
        labels: { font: { weight: 'bold' }, color: '#444' }
      },
      title: {
        display: true,
        text: 'Real-time Farm Data',
        color: '#2e7d32',
        font: { size: 18, weight: 'bold' }
      },
      tooltip: {
        enabled: true,
        backgroundColor: '#fff',
        titleColor: '#2e7d32',
        bodyColor: '#333',
        borderColor: '#2e7d32',
        borderWidth: 1,
        padding: 12,
        cornerRadius: 8,
        caretSize: 8,
        displayColors: false
      }
    },
    hover: {
      mode: 'nearest',
      intersect: true
    }
  };

  const handleSave = () => {
    setCurrentStatus({ temperature, humidity, sunlight });
    setAppliedStatus(getInitialStatus());
    localStorage.setItem(storageKey, JSON.stringify({ temperature, humidity, sunlight }));
    window.dispatchEvent(new Event('farmStatusChanged'));
  };

  // Cập nhật biểu đồ mỗi 3s: nếu đã nhấn Save thì dùng giá trị appliedStatus, chưa thì random
  useEffect(() => {
    const interval = setInterval(() => {
      const now = new Date();
      setChartData(prevData => ({
        temperature: {
          ...prevData.temperature,
          labels: [...prevData.temperature.labels, now].slice(-20),
          datasets: [{
            ...prevData.temperature.datasets[0],
            data: [...prevData.temperature.datasets[0].data, appliedStatus.temperature ?? (Math.random() * 30 + 10)].slice(-20)
          }]
        },
        humidity: {
          ...prevData.humidity,
          labels: [...prevData.humidity.labels, now].slice(-20),
          datasets: [{
            ...prevData.humidity.datasets[0],
            data: [...prevData.humidity.datasets[0].data, appliedStatus.humidity ?? (Math.random() * 50 + 30)].slice(-20)
          }]
        },
        sunlight: {
          ...prevData.sunlight,
          labels: [...prevData.sunlight.labels, now].slice(-20),
          datasets: [{
            ...prevData.sunlight.datasets[0],
            data: [...prevData.sunlight.datasets[0].data, appliedStatus.sunlight ?? (Math.random() * 1000 + 500)].slice(-20)
          }]
        }
      }));
    }, 5000);
    return () => clearInterval(interval);
  }, [appliedStatus]);

  // Add gradients to chart lines
  const getChartDataWithGradient = (data, ref, colorTop, colorBottom) => {
    if (ref.current && ref.current.ctx) {
      const ctx = ref.current.ctx;
      const gradient = getGradient(ctx, colorTop, colorBottom);
      return {
        ...data,
        datasets: data.datasets.map(ds => ({
          ...ds,
          backgroundColor: gradient
        }))
      };
    }
    return data;
  };

  return (
    <div style={{ minHeight: '100vh', background: 'linear-gradient(120deg, #e0f7fa 0%, #f5f8f5 100%)', padding: 0 }}>
      {/* Header with return button */}
      <div style={{
        display: 'flex',
        alignItems: 'center',
        padding: '32px 0 16px 0',
        maxWidth: 1200,
        margin: '0 auto',
        gap: 16
      }}>
        <button
          onClick={() => navigate('/dashboard')}
          style={{
            background: '#2e7d32',
            color: 'white',
            border: 'none',
            borderRadius: 8,
            padding: '10px 22px',
            fontWeight: 600,
            fontSize: 16,
            boxShadow: '0 2px 8px rgba(46,125,50,0.15)',
            cursor: 'pointer',
            marginRight: 24,
            transition: 'background 0.2s'
          }}
          onMouseOver={e => e.currentTarget.style.background = '#256025'}
          onMouseOut={e => e.currentTarget.style.background = '#2e7d32'}
        >
          ← Dashboard
        </button>
        <FaLeaf size={32} color="#2e7d32" style={{ marginRight: 12 }} />
        <div>
          <h1 style={{ fontSize: 32, fontWeight: 800, color: '#2e7d32', margin: 0 }}>Farm Monitoring Dashboard</h1>
          <div style={{ color: '#666', fontSize: 16, marginTop: 2 }}>Real-time environmental data</div>
        </div>
      </div>

      {/* Charts */}
      <div style={{
        display: 'grid',
        gridTemplateColumns: '1fr',
        gap: 32,
        maxWidth: 1200,
        margin: '0 auto',
        padding: '24px 0 48px 0'
      }}>
        {/* Temperature Chart */}
        <div style={{ background: 'white', borderRadius: 18, boxShadow: '0 4px 24px rgba(255,99,132,0.08)', padding: 32 }}>
          <h2 style={{ color: '#ff6384', fontWeight: 700, fontSize: 22, marginBottom: 12 }}>🌡️ Temperature</h2>
          <div style={{ height: 300 }}>
            <Line
              ref={tempRef}
              options={baseOptions}
              data={getChartDataWithGradient(chartData.temperature, tempRef, 'rgba(255,99,132,0.7)', 'rgba(255,99,132,0.05)')}
            />
          </div>
        </div>
        {/* Humidity Chart */}
        <div style={{ background: 'white', borderRadius: 18, boxShadow: '0 4px 24px rgba(53,162,235,0.08)', padding: 32 }}>
          <h2 style={{ color: '#3592eb', fontWeight: 700, fontSize: 22, marginBottom: 12 }}>💧 Humidity</h2>
          <div style={{ height: 300 }}>
            <Line
              ref={humRef}
              options={baseOptions}
              data={getChartDataWithGradient(chartData.humidity, humRef, 'rgba(53,162,235,0.7)', 'rgba(53,162,235,0.05)')}
            />
          </div>
        </div>
        {/* Sunlight Chart */}
        <div style={{ background: 'white', borderRadius: 18, boxShadow: '0 4px 24px rgba(255,206,86,0.08)', padding: 32 }}>
          <h2 style={{ color: '#ffce56', fontWeight: 700, fontSize: 22, marginBottom: 12 }}>☀️ Sunlight</h2>
          <div style={{ height: 300 }}>
            <Line
              ref={sunRef}
              options={baseOptions}
              data={getChartDataWithGradient(chartData.sunlight, sunRef, 'rgba(255,206,86,0.7)', 'rgba(255,206,86,0.05)')}
            />
          </div>
        </div>

        {/* Control Box */}
        <div style={{ background: 'white', borderRadius: 18, boxShadow: '0 4px 24px rgba(46,125,50,0.08)', padding: 32 }}>
          <h2 style={{ color: '#2e7d32', fontWeight: 700, fontSize: 22, marginBottom: 24, textAlign: 'center' }}>Adjust Parameters</h2>
          
          {/* Current Status */}
          <div style={{ display: 'flex', justifyContent: 'space-around', marginBottom: 32 }}>
            <div style={{ textAlign: 'center' }}>
              <div style={{ fontSize: 28 }}>🌡️</div>
              <div style={{ color: '#333', fontWeight: 600 }}>Temperature</div>
              <div style={{ color: '#2e7d32', fontWeight: 700, fontSize: 20 }}>{currentStatus.temperature}°C</div>
            </div>
            <div style={{ textAlign: 'center' }}>
              <div style={{ fontSize: 28 }}>💧</div>
              <div style={{ color: '#333', fontWeight: 600 }}>Humidity</div>
              <div style={{ color: '#2e7d32', fontWeight: 700, fontSize: 20 }}>{currentStatus.humidity}%</div>
            </div>
            <div style={{ textAlign: 'center' }}>
              <div style={{ fontSize: 28 }}>☀️</div>
              <div style={{ color: '#333', fontWeight: 600 }}>Sunlight</div>
              <div style={{ color: '#2e7d32', fontWeight: 700, fontSize: 20 }}>{currentStatus.sunlight} lux</div>
            </div>
          </div>

          {/* Controls */}
          <div style={{ marginBottom: 32 }}>
            <label style={{ fontWeight: 600, color: '#333', marginBottom: 8, display: 'block' }}>🌡️ Temperature (°C)</label>
            <div style={{ display: 'flex', alignItems: 'center', gap: 16 }}>
              <input 
                type="number" 
                min={0} 
                max={50} 
                value={temperature} 
                onChange={e => setTemperature(Number(e.target.value))} 
                style={{ border: '1px solid #e0e0e0', borderRadius: 6, padding: '6px 12px', width: 80, fontSize: 16 }} 
              />
              <input 
                type="range" 
                min={0} 
                max={50} 
                value={temperature} 
                onChange={e => setTemperature(Number(e.target.value))} 
                style={{ flex: 1 }} 
              />
              <span style={{ minWidth: 32, textAlign: 'right', color: '#2e7d32', fontWeight: 600 }}>{temperature}°C</span>
            </div>
          </div>

          <div style={{ marginBottom: 32 }}>
            <label style={{ fontWeight: 600, color: '#333', marginBottom: 8, display: 'block' }}>💧 Humidity (%)</label>
            <div style={{ display: 'flex', alignItems: 'center', gap: 16 }}>
              <input 
                type="number" 
                min={0} 
                max={100} 
                value={humidity} 
                onChange={e => setHumidity(Number(e.target.value))} 
                style={{ border: '1px solid #e0e0e0', borderRadius: 6, padding: '6px 12px', width: 80, fontSize: 16 }} 
              />
              <input 
                type="range" 
                min={0} 
                max={100} 
                value={humidity} 
                onChange={e => setHumidity(Number(e.target.value))} 
                style={{ flex: 1 }} 
              />
              <span style={{ minWidth: 32, textAlign: 'right', color: '#2e7d32', fontWeight: 600 }}>{humidity}%</span>
            </div>
          </div>

          <div style={{ marginBottom: 32 }}>
            <label style={{ fontWeight: 600, color: '#333', marginBottom: 8, display: 'block' }}>☀️ Sunlight (lux)</label>
            <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', gap: 0 }}>
              <input
                type="range"
                min={0}
                max={1600}
                step={1}
                value={sunlight}
                onChange={e => setSunlight(Number(e.target.value))}
                style={{ width: '90%' }}
              />
            </div>
            <div style={{ textAlign: 'center', marginTop: 8, color: '#2e7d32', fontWeight: 700, fontSize: 16 }}>{sunlight} lux</div>
          </div>

          <button 
            style={{
              width: '100%',
              background: '#2e7d32',
              color: '#fff',
              border: 'none',
              padding: '12px 0',
              borderRadius: 8,
              fontWeight: 600,
              fontSize: 18,
              boxShadow: '0 2px 8px rgba(46,125,50,0.08)',
              cursor: 'pointer',
              transition: 'background 0.2s'
            }}
            onMouseOver={e => e.currentTarget.style.background = '#1b5e20'}
            onMouseOut={e => e.currentTarget.style.background = '#2e7d32'}
            onClick={handleSave}
          >
            Save Changes
          </button>
        </div>
      </div>
    </div>
  );
};

export default FarmDetails; 