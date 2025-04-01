import { useEffect, useRef, useState } from "react";

const CAMERA_SERVER = "ws://192.168.7.2:9002";

export function useCameraSocket() {
  const videoRef = useRef<HTMLImageElement>(null);
  const [hasVideo, setHasVideo] = useState(false);

  useEffect(() => {
    const websocket = new WebSocket(CAMERA_SERVER);
    websocket.binaryType = "arraybuffer";

    websocket.onopen = () => console.log("Camera WebSocket connected");

    websocket.onclose = () => {
      console.warn("Camera WebSocket closed");
      setHasVideo(false);
    }
    
    websocket.onerror = (err) => console.error("Camera WebSocket error:", err);

    websocket.onmessage = (event) => {
      const blob = new Blob([event.data], { type: "image/jpeg" });
      const url = URL.createObjectURL(blob);

      if (videoRef.current) {
        videoRef.current.src = url;
        setHasVideo(true);
      } else {
        setHasVideo(false);
      }
    };

    return () => websocket.close();
  }, []);

  return { videoRef, hasVideo };
}
