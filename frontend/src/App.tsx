import { useRef, useEffect, useState } from 'react'
import './App.css'
import ControlBar from './components/ControlBar';
import Navbar from './components/Navbar';
import Snapshots from './components/Snapshots';

const SERVER = "ws://192.168.7.2:9002";

function App() {
  const videoRef = useRef<HTMLImageElement>(null);
  const [hasVideo, setHasVideo] = useState(false);
  const [numPeople, setNumPeople] = useState(0);

  useEffect(() => {
    const websocket = new WebSocket(SERVER);
    websocket.binaryType = "arraybuffer";

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

    return () => {
      websocket.close();
    };
  }, []);

  return (
    <div className='flex flex-col h-screen'>
      {/* Navbar */}
      <Navbar />

      {/* Content */}
      <div className='flex flex-row m-auto hover-shadow'>
        {/* Video Player */}
        <div className='flex flex-col'>
          {hasVideo ? (
            <img className='border-4 border-black w-[1280px] h-[720px] object-cover' ref={videoRef} alt="Video Feed" />
          ) : (
            <div className='border-4 bg-white border-black w-[1280px] h-[720px] flex items-center justify-center'>
              <p className='text-2xl text-center text-black'>Loading...</p>
            </div>
          )}
          <ControlBar />
        </div>

        {/* Sidebar */}
        <div className='flex flex-col text-black bg-white border-4 border-l-0 border-black text-start'>
          {/* Stats */}
          <div className='p-4 border-b-4 border-black'>
            <h2 className='mb-2 text-xl font-semibold'>Stats</h2>
            <p>People: {numPeople}</p>
            <p>Runtime: 00:21:29</p>
          </div>

          {/* Snapshots */}
          <Snapshots />
        </div>
      </div>
    </div>
  )
}

export default App
