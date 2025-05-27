import { useParams } from "react-router-dom";

export default function RoomPage() {
  const { roomId } = useParams();

  return (
    <div className="p-4">
      <h1 className="text-2xl font-bold mb-4">Room: {roomId.toUpperCase()}</h1>
      {/* Hiển thị sensor, thông số của phòng này */}
    </div>
  );
}