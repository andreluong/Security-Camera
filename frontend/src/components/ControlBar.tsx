import { Icon } from '@iconify/react/dist/iconify.js'
import { CommandType } from '../hooks/useCommandSocket';

export default function ControlBar({
	sendCommand
}: {
	sendCommand: (command: CommandType) => void
}) {
  return (
    <div className='flex flex-row justify-center gap-4 p-4 text-black bg-white border-4 border-t-0 border-black'>
			<button className='btn'>
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
