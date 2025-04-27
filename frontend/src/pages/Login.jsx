import React from "react";
import "./Login.css";
import bgImage from "../assets/bg.jpg";
import useNavigation from "../components/Navigate";

const Login = () => {
  const { goToDashboard } = useNavigation();

  const handleLogin = () => {
    goToDashboard(); // chuyển hướng vào Dashboard
  };

  return (
    <div
      className="login-container"
      style={{ backgroundImage: `url(${bgImage})` }}
    >
      <div className="login-box">
        <h2>Welcome Back</h2>
        <form>
          <label>Username</label>
          <input type="text" placeholder="Enter your username" />

          <label>Password</label>
          <input type="password" placeholder="Enter your password" />

          <div className="options">
            <label>
              <input type="checkbox" /> Remember me
            </label>
            <a href="#">Forgot Password?</a>
          </div>

          <button type="submit" onClick={handleLogin}>Login</button>

          <div className="divider">or</div>

          <div className="social-login">
            <button className="facebook">Facebook</button>
            <button className="twitter">Twitter</button>
          </div>
        </form>
        <p className="footer">© 2025 Smart Farm. All rights reserved.</p>
      </div>
    </div>
  );
};

export default Login;
