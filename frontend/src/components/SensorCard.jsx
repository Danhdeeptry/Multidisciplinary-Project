import React from "react";

const SensorCard = ({ label, value, unit }) => {
  return (
    <div className="bg-white p-4 rounded-lg shadow-md text-center w-60">
      <h3 className="text-lg font-semibold text-gray-700">{label}</h3>
      <p className="text-2xl font-bold text-green-700">
        {value} {unit}
      </p>
    </div>
  );
};

export default SensorCard;