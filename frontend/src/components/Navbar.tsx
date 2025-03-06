import { Icon } from '@iconify/react/dist/iconify.js'

export default function Navbar() {
  return (
    <header className="flex flex-row justify-between p-4 bg-white shadow-md">
			<h1 className="flex flex-row text-2xl font-bold tracking-widest gradient-text">
			<Icon icon="ph:security-camera" width="32" height="32" color='purple' />&nbsp;CCTV
			</h1>
			<p className='text-lg font-semibold gradient-text'>About</p>
    </header>
  )
}
