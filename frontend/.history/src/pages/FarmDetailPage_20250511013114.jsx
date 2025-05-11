import { useParams } from "react-router-dom";
import { useState } from "react";

export default function FarmDetailPage() {
  const { farmId } = useParams();
  // Mock data, sau này sẽ lấy từ API
  const [temperature, setTemperature] = useState(25);
  const [humidity, setHumidity] = useState(60);
  const [sunlight, setSunlight] = useState(50);

  return (
    <div className="p-8 max-w-xl mx-auto">
      <h1 className="text-2xl font-bold mb-6 text-green-700">Farm: {farmId?.toUpperCase()}</h1>
      <div className="mb-8">
        <label className="block font-semibold mb-2">Temperature (°C)</label>
        <input type="number" min={0} max={50} value={temperature} onChange={e => setTemperature(Number(e.target.value))} className="border rounded px-2 py-1 mr-4 w-24" />
        <input type="range" min={0} max={50} value={temperature} onChange={e => setTemperature(Number(e.target.value))} className="w-64 align-middle" />
      </div>
      <div className="mb-8">
        <label className="block font-semibold mb-2">Humidity (%)</label>
        <input type="number" min={0} max={100} value={humidity} onChange={e => setHumidity(Number(e.target.value))} className="border rounded px-2 py-1 mr-4 w-24" />
        <input type="range" min={0} max={100} value={humidity} onChange={e => setHumidity(Number(e.target.value))} className="w-64 align-middle" />
      </div>
      <div className="mb-8">
        <label className="block font-semibold mb-2">Sunlight (%)</label>
        <input type="number" min={0} max={100} value={sunlight} onChange={e => setSunlight(Number(e.target.value))} className="border rounded px-2 py-1 mr-4 w-24" />
        <input type="range" min={0} max={100} value={sunlight} onChange={e => setSunlight(Number(e.target.value))} className="w-64 align-middle" />
      </div>
      <div className="mt-8">
        <button className="bg-green-600 text-white px-6 py-2 rounded hover:bg-green-700 transition">Save</button>
      </div>
    </div>
  );
} 