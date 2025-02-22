import { useRef, useEffect } from 'react'
import './App.css'

const SERVER = "ws://192.168.7.2:9002";

function App() {
  const videoRef = useRef<HTMLImageElement>(null);

  useEffect(() => {
    const websocket = new WebSocket(SERVER);
    websocket.binaryType = "arraybuffer";

    websocket.onmessage = (event) => {
      const blob = new Blob([event.data], { type: "image/jpeg" });
      const url = URL.createObjectURL(blob);
      
      if (videoRef.current) {
        videoRef.current.src = url;
      }
    };

    return () => {
      websocket.close();
    };
  }, []);

  return (
    <>
      <div className='flex h-screen'>
        <div className='m-auto'>
          <p className='text-4xl mb-4'>Video Feed</p>
          <img className='border-4 border-black' ref={videoRef} alt="Video Feed" />
        </div>
      </div>
    </>
  )
}

export default App
