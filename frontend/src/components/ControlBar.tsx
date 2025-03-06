import { Icon } from '@iconify/react/dist/iconify.js'

export default function ControlBar() {
  return (
    <div className='flex flex-row justify-center gap-4 p-4 text-black bg-white border-4 border-t-0 border-black'>
			<button className='btn'>
				Snapshot
			</button>
			<button className='btn'>
				Alarm
			</button>
			<button className='btn'>
				<Icon icon="material-symbols:arrow-upward-alt" width="28" height="28" />
			</button>
			<button className='btn'>
				<Icon icon="material-symbols:arrow-downward-alt" width="28" height="28" />
			</button>
			<button className='btn'>
				<Icon icon="material-symbols:arrow-left-alt" width="28" height="28" />
			</button>
			<button className='btn'>
				<Icon icon="material-symbols:arrow-right-alt" width="28" height="28" />
			</button>
    </div>
  )
}
