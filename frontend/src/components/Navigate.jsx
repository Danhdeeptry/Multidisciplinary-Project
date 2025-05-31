import { useNavigate } from "react-router-dom";

const useNavigation = () => {
  const navigate = useNavigate();

  const goToDashboard = () => navigate("/dashboard");
  const goToLogin = () => navigate("/login");
  // Có thể thêm nhiều hàm chuyển hướng khác ở đây

  return {
    goToDashboard,
    goToLogin,
  };
};

export default useNavigation;
