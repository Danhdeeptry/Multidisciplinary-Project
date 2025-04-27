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
import { useNavigate } from 'react-router-dom';
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

const getGradient = (ctx, colorTop, colorBottom) => {
  const gradient = ctx.createLinearGradient(0, 0, 0, 300);
  gradient.addColorStop(0, colorTop);
  gradient.addColorStop(1, colorBottom);
  return gradient;
};

const FarmDetails = () => {
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

  // Simulated data update function (replace with actual API call later)
  const updateChartData = () => {
    const now = new Date();
    const newData = {
      temperature: Math.random() * 30 + 10, // 10-40°C
      humidity: Math.random() * 50 + 30,    // 30-80%
      sunlight: Math.random() * 1000 + 500  // 500-1500 lux
    };
    setChartData(prevData => ({
      temperature: {
        ...prevData.temperature,
        labels: [...prevData.temperature.labels, now].slice(-20),
        datasets: [{
          ...prevData.temperature.datasets[0],
          data: [...prevData.temperature.datasets[0].data, newData.temperature].slice(-20)
        }]
      },
      humidity: {
        ...prevData.humidity,
        labels: [...prevData.humidity.labels, now].slice(-20),
        datasets: [{
          ...prevData.humidity.datasets[0],
          data: [...prevData.humidity.datasets[0].data, newData.humidity].slice(-20)
        }]
      },
      sunlight: {
        ...prevData.sunlight,
        labels: [...prevData.sunlight.labels, now].slice(-20),
        datasets: [{
          ...prevData.sunlight.datasets[0],
          data: [...prevData.sunlight.datasets[0].data, newData.sunlight].slice(-20)
        }]
      }
    }));
  };

  useEffect(() => {
    const interval = setInterval(updateChartData, 3000);
    return () => clearInterval(interval);
  }, []);

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

  const navigate = useNavigate();

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
      </div>
    </div>
  );
};

export default FarmDetails; 