import { useEffect, useState } from 'react';
import './App.css'
import ControlBar from './components/ControlBar';
import Navbar from './components/Navbar';
import Snapshots from './components/Snapshots';
import { useCameraSocket } from './hooks/useCameraSocket';
import { useCommandSocket } from './hooks/useCommandSocket';

function fetchSnapshots(setSnapshots: React.Dispatch<React.SetStateAction<string[]>>) {
  const openDB = indexedDB.open("snapshotsDB", 1);

  openDB.onupgradeneeded = () => {
    const db = openDB.result;
    if (!db.objectStoreNames.contains("snapshots")) {
      db.createObjectStore("snapshots", { autoIncrement: true });
    }
  };

  openDB.onsuccess = () => {
    const db = openDB.result;
    const tx = db.transaction("snapshots", "readonly");
    const store = tx.objectStore("snapshots");
    const request = store.getAll();

    request.onsuccess = () => {
      setSnapshots((prev) => {
        prev.forEach(URL.revokeObjectURL);
        return request.result.map((blob: Blob) => URL.createObjectURL(blob));
      });
    };

    request.onerror = (e) => console.error("Error fetching snapshots:", e);

    tx.oncomplete = () => db.close();
  };
  
  openDB.onerror = (e) => console.error("Error fetching all snapshots:", e);
}

function App() {
  const { videoRef, hasVideo } = useCameraSocket();
  const { peopleCount, sendCommand } = useCommandSocket();
	const [snapshots, setSnapshots] = useState<string[]>([]);
  const [runtime, setRuntime] = useState<number>(0);

  // Runtime
  useEffect(() => {
    let intervalId: number| null = null;

    if (hasVideo) {
      intervalId = setInterval(() => {
        setRuntime(prev => prev + 1);
      }, 1000);
    } else {
      setRuntime(0);
    }

    return () => {
      if (intervalId) clearInterval(intervalId);
    };
  }, [hasVideo]);

  // Format runtime to HH:MM:S
  const formatTime = (seconds: number) => {
    const hours = Math.floor(seconds / 3600);
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return [
      hours.toString().padStart(2, '0'),
      mins.toString().padStart(2, '0'),
      secs.toString().padStart(2, '0')
    ].join(':');
  };

  return (
    <div className='flex flex-col h-screen'>
      {/* Navbar */}
      <Navbar />

      {/* Content */}
      <div className='flex flex-row m-auto hover-shadow'>
        {/* Video Player */}
        <div className='flex flex-col'>
          {/* {hasVideo ? (
            <img className='border-4 border-black w-[1280px] h-[720px] object-cover' ref={videoRef} alt="Video Feed" />
          ) : (
            <div className='border-4 bg-white border-black w-[1280px] h-[720px] flex items-center justify-center'>
              <p className='text-2xl text-center text-black'>Loading...</p>
            </div>
          )} */}
          <img className='border-4 border-black w-[1280px] h-[720px] object-cover' ref={videoRef} alt="Video Feed" />
          <ControlBar sendCommand={sendCommand} currentImage={videoRef.current} onSnapshotAdded={() => fetchSnapshots(setSnapshots)} />
        </div>

        {/* Sidebar */}
        <div className='flex flex-col text-black bg-white border-4 border-l-0 border-black text-start'>
          {/* Stats */}
          <div className='p-4 border-b-4 border-black'>
            <h2 className='mb-2 text-xl font-semibold'>Stats</h2>
            <p>People: {peopleCount}</p>
            <p>Runtime: {formatTime(runtime)}</p>
          </div>

          {/* Snapshots */}
          <Snapshots snapshots={snapshots} fetchSnapshots={() => fetchSnapshots(setSnapshots)} />
        </div>
      </div>
    </div>
  )
}

export default App
