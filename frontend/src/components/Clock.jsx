import React, { useEffect, useState } from 'react';
import './Clock.css';
const Clock = () => {
  const [currentTime, setCurrentTime] = useState(new Date());

  useEffect(() => {
    const interval = setInterval(() => {
      setCurrentTime(new Date());
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  const formatTime = (date) => {
    const year = date.getFullYear();
    const month = (`0${date.getMonth() + 1}`).slice(-2);
    const day = (`0${date.getDate()}`).slice(-2);
    const hour = (`0${date.getHours()}`).slice(-2);
    const minute = (`0${date.getMinutes()}`).slice(-2);
    const second = (`0${date.getSeconds()}`).slice(-2);
    return `${year}/${month}/${day} ${hour}:${minute}:${second}`;
  };

  return (
    <div className="clock-wrapper">
      <div className="datetime-box">
        {formatTime(currentTime)}
      </div>
    </div>
  );
};

export default Clock;