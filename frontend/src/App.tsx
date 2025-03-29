import './App.css'
import ControlBar from './components/ControlBar';
import Navbar from './components/Navbar';
import Snapshots from './components/Snapshots';
import { useCameraSocket } from './hooks/useCameraSocket';
import { useCommandSocket } from './hooks/useCommandSocket';

function App() {
  const { videoRef, hasVideo } = useCameraSocket();
  const { peopleCount, sendCommand } = useCommandSocket();

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
          <ControlBar sendCommand={sendCommand} />
        </div>

        {/* Sidebar */}
        <div className='flex flex-col text-black bg-white border-4 border-l-0 border-black text-start'>
          {/* Stats */}
          <div className='p-4 border-b-4 border-black'>
            <h2 className='mb-2 text-xl font-semibold'>Stats</h2>
            <p>People: {peopleCount}</p>
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
