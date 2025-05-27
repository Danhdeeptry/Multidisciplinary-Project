import React from "react";
import { Routes, Route } from "react-router-dom";
import Login from "./pages/Login";
import Dashboard from "./pages/Dashboard";
import RoomPage from "./pages/RoomPage";
import FarmDetails from "./pages/FarmDetails";

const App = () => {
  return (
    <Routes>
      <Route path="/" element={<Login />} />  
      <Route path="/login" element={<Login />} />
      <Route path="/dashboard" element={<Dashboard />} />
      <Route path="/rooms/:roomId" element={<RoomPage />} />
      <Route path="/farm/:farmId" element={<FarmDetails />} />
    </Routes>
  );
};

export default App;