import React from 'react';

const FarmCard = ({ label, image, onCardClick }) => {
  // Sample mock data - in a real app, you would fetch this from your API
  const mockData = {
    FARM1: {
      temperature: "27°C",
      humidity: "65%",
      sunlight: "High"
    },
    FARM2: {
      temperature: "24°C",
      humidity: "72%",
      sunlight: "Medium"
    },
    FARM3: {
      temperature: "29°C",
      humidity: "58%",
      sunlight: "High"
    }
  };

  const farmData = mockData[label] || { temperature: "N/A", humidity: "N/A", sunlight: "N/A" };
  
  return (
    <div 
      className="bg-white rounded-lg overflow-hidden shadow-md hover:shadow-lg transition-shadow duration-300 cursor-pointer"
      onClick={onCardClick}
    >
      <div className="relative">
        <img 
          src={image} 
          alt={label} 
          className="w-full h-48 object-cover"
        />
        <div className="absolute top-0 left-0 bg-green-600 text-white px-3 py-1 m-2 rounded-md font-medium">
          {label}
        </div>
      </div>
      
      <div className="p-4">
        <h3 className="text-lg font-semibold text-gray-800 mb-3">Farm Status</h3>
        
        <div className="space-y-2 mb-4">
          <div className="flex items-center">
            <span className="text-red-500 mr-2">🌡️</span>
            <span className="text-gray-700">Temperature: {farmData.temperature}</span>
          </div>
          
          <div className="flex items-center">
            <span className="text-blue-500 mr-2">💧</span>
            <span className="text-gray-700">Humidity: {farmData.humidity}</span>
          </div>
          
          <div className="flex items-center">
            <span className="text-yellow-500 mr-2">☀️</span>
            <span className="text-gray-700">Sunlight: {farmData.sunlight}</span>
          </div>
        </div>
        
        <button 
          className="w-full bg-green-600 text-white py-2 px-4 rounded-md hover:bg-green-700 transition-colors duration-300 flex justify-between items-center"
        >
          <span>View Details</span>
          <span>→</span>
        </button>
      </div>
    </div>
  );
};

export default FarmCard;