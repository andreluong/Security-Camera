import { Icon } from '@iconify/react/dist/iconify.js'
import { CommandType } from '../hooks/useCommandSocket';

export default function ControlBar({
	sendCommand,
	currentImage,
  onSnapshotAdded
}: {
	sendCommand: (command: CommandType) => void;
	currentImage: HTMLImageElement | null;
  onSnapshotAdded: () => void;
}) {

	const storeCurrentImage = async () => {
		if (!currentImage || !currentImage.src) return;

    const address = await fetch(currentImage.src);
    const blob = await address.blob();

    const openDB = indexedDB.open("snapshotsDB", 1);

    openDB.onupgradeneeded = () => {
      const db = openDB.result;
      if (!db.objectStoreNames.contains("snapshots")) {
        db.createObjectStore("snapshots", { autoIncrement: true });
      }
    };

    openDB.onsuccess = () => {
      const db = openDB.result;
      const tx = db.transaction("snapshots", "readwrite");
      const store = tx.objectStore("snapshots");
      const request = store.add(blob);

      request.onsuccess = () => {
        onSnapshotAdded();
			  console.log("Added snapshot to DB");
			};
			
			request.onerror = (e) => console.error("Error adding snapshot:", e);
			
			tx.oncomplete = () => db.close();
    };

    openDB.onerror = (e) => console.error("Error opening DB:", e);
  };

  return (
    <div className='flex flex-row justify-center gap-4 p-4 text-black bg-white border-4 border-t-0 border-black'>
			<button className='btn' onClick={storeCurrentImage}>
				Snapshot
			</button>
			<button className='btn' onClick={() => sendCommand("alarm")}>
				Alarm
			</button>
			<button className='btn' onClick={() => sendCommand("up")}>
				<Icon icon="material-symbols:arrow-upward-alt" width="28" height="28" />
			</button>
			<button className='btn' onClick={() => sendCommand("down")}>
				<Icon icon="material-symbols:arrow-downward-alt" width="28" height="28" />
			</button>
			<button className='btn' onClick={() => sendCommand("left")}>
				<Icon icon="material-symbols:arrow-left-alt" width="28" height="28" />
			</button>
			<button className='btn' onClick={() => sendCommand("right")}>
				<Icon icon="material-symbols:arrow-right-alt" width="28" height="28" />
			</button>
    </div>
  )
}
