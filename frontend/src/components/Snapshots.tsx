import { Icon } from "@iconify/react/dist/iconify.js";
import { useEffect } from "react";

export default function Snapshots({
	snapshots,
	fetchSnapshots
}: {
	snapshots: string[];
	fetchSnapshots: () => void;
}) {

	const clearAllSnapshots = () => {
		const openDB = indexedDB.open("snapshotsDB", 1);

		openDB.onsuccess = () => {
			const db = openDB.result;
			const tx = db.transaction("snapshots", "readwrite");
			const store = tx.objectStore("snapshots");
			const request = store.clear();
			
			request.onsuccess = () => {
			  fetchSnapshots();
			  console.log("Cleared all snapshots");
			};
			
			request.onerror = (e) => console.error("Error clearing snapshots:", e);
			
			tx.oncomplete = () => db.close();
		};

		openDB.onerror = (e) => console.error("Error opening DB:", e);
	};

	useEffect(() => {
    fetchSnapshots();
  }, [fetchSnapshots]);

  return (
    <div className='flex flex-col flex-grow min-h-0 p-4'>
			<div className="flex flex-row justify-between mb-2 text-xl">
				<h2 className='font-semibold'>Snapshots</h2>
				<button className='text-red-600 btn' onClick={clearAllSnapshots}>
					<Icon icon="material-symbols:delete-forever-outline" width="28" height="28" />
				</button>
			</div>
			<div className='h-[580px] space-y-2 overflow-y-auto overflow-x-hidden pr-4 w-[320px]'>
				{snapshots.slice().reverse().map((snapshot, index) => (
					<div key={index} className='relative border-2 border-black w-[320px] h-[180px]'>
						<img className='object-cover w-full h-full' src={snapshot} alt={`Snapshot ${index + 1}`} />
					</div>
				))}
			</div>
		</div>
  )
}
