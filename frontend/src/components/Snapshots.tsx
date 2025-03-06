export default function Snapshots() {
	
	// TODO: Should add and fetch pictures from local storage / indexedDB

  return (
    <div className='flex flex-col flex-grow min-h-0 p-4'>
			<h2 className='mb-2 text-xl font-semibold'>Snapshots</h2>

			{/* List */}
			<div className='h-[580px] space-y-2 overflow-y-auto overflow-x-hidden pr-4'>
				<div className='border-2 border-black w-[320px] h-[180px]'>
					Picture 1
				</div>
				<div className='border-2 border-black w-[320px] h-[180px]'>
					Picture 2
				</div>
				<div className='border-2 border-black w-[320px] h-[180px]'>
					Picture 3
				</div>
				<div className='border-2 border-black w-[320px] h-[180px]'>
					Picture 4
				</div>
				<div className='border-2 border-black w-[320px] h-[180px]'>
					Picture 5
				</div>
			</div>
		</div>
  )
}
