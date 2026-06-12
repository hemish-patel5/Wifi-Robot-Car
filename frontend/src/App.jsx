import './index.css'
import { startCommand } from './Controls.js'
import { stopCommand } from './Controls.js'


// Shared Tailwind classes for the raised cream panels used throughout the UI.
const panelChrome =
  'border-2 border-[rgba(145,141,132,0.58)] bg-[linear-gradient(145deg,rgba(255,255,255,0.82),rgba(232,228,218,0.78))]'

// Shared pixel text shadow to match the chunky game-controller look.
const pixelText =
  'font-black tracking-normal text-[#333] [text-shadow:1px_0_0_#333,0_2px_0_rgba(255,255,255,0.95),2px_3px_0_rgba(0,0,0,0.16)]'

// These are the four static control buttons. They are intentionally display-only for now.
const directions = [
  { label: 'Forward', direction: 'f' },
  { label: 'Left', direction: 'l' },
  { label: 'Right', direction: 'r' },
  { label: 'Backward', direction: 'b' },
]

// Tailwind positioning for each control button inside the direction pad.
const directionPositions = {
  f: 'top-8 left-1/2 -translate-x-1/2',
  l: 'top-1/2 left-5 -translate-y-1/2',
  r: 'top-1/2 right-5 -translate-y-1/2',
  b: 'bottom-8 left-1/2 -translate-x-1/2',
}

// Rotation classes let one arrow drawing point in all four directions.
const arrowRotations = {
  f: 'rotate-0',
  r: 'rotate-90',
  b: 'rotate-180',
  l: '-rotate-90',
}

function ArrowIcon({ direction }) {
  return (
    // The arrow is made from two simple shapes: a stem and a clipped arrow head.
    <span
      className={`relative block size-[44%] ${arrowRotations[direction]}`}
      aria-hidden="true"
    >
      <span className="absolute bottom-[5%] left-1/2 h-[58%] w-[34%] -translate-x-1/2 rounded bg-[#f9f9f9]" />
      <span className="absolute left-1/2 top-0 h-[76%] w-[76%] -translate-x-1/2 bg-[#f9f9f9] [clip-path:polygon(50%_0,100%_58%,70%_58%,70%_100%,30%_100%,30%_58%,0_58%)]" />
    </span>
  )
}

function GearIcon() {
  return (
    // A decorative settings icon built with Tailwind borders so no icon package is needed.
    <span
      className="relative size-7 rounded-full border-[7px] border-dashed border-current"
      aria-hidden="true"
    >
      <span className="absolute inset-1 rounded-full bg-[#f2eee6]" />
      <span className="absolute inset-[9px] rounded-full bg-current" />
    </span>
  )
}

function DirectionButton({ label, direction }) {
  function startMoving(event) {
    event.preventDefault()
    startCommand(direction)
  }

  function stopMoving(event) {
    event.preventDefault()
    stopCommand()
  }

  function handlePointerDown(event) {
    event.preventDefault()

    if (event.currentTarget.setPointerCapture) {
      event.currentTarget.setPointerCapture(event.pointerId)
    }

    startCommand(direction)
  }

  function handlePointerRelease(event) {
    event.preventDefault()

    if (
      event.currentTarget.hasPointerCapture &&
      event.currentTarget.hasPointerCapture(event.pointerId)
    ) {
      event.currentTarget.releasePointerCapture(event.pointerId)
    }

    stopCommand()
  }

  return (
    // One direction control: an inactive button plus its label.
    <div
      className={`absolute grid w-[clamp(94px,28vw,122px)] justify-items-center gap-2 ${directionPositions[direction]}`}
    >
      <button
        className="grid aspect-square w-[clamp(76px,22vw,98px)] touch-none select-none place-items-center rounded-2xl border-[3px] border-[#070707] bg-[radial-gradient(circle_at_28%_18%,rgba(255,255,255,0.18),transparent_36%),linear-gradient(145deg,#555,#1b1b1b)]"
        type="button"
        aria-label={label}
        onContextMenu={(event) => event.preventDefault()}
        onPointerDown={handlePointerDown}
        onPointerUp={handlePointerRelease}
        onPointerCancel={handlePointerRelease}
        onTouchStart={startMoving}
        onTouchEnd={stopMoving}
        onTouchCancel={stopMoving}
        onMouseDown={startMoving}
        onMouseUp={stopMoving}
        onMouseLeave={stopMoving}
      >
        <ArrowIcon direction={direction} />
      </button>
      <span className={`${pixelText} text-center text-[clamp(0.72rem,2.8vw,0.95rem)] leading-none`}>
        {label}
      </span>
    </div>
  )
}

function MiniCarIcon() {
  return (
    // Small speed icon: a tiny car with motion marks, made from layered spans.
    <span
      className="relative inline-block h-[33px] w-16 border-b-4 border-[#343434]"
      aria-hidden="true"
    >
      <span className="absolute bottom-[13px] left-4 h-3.5 w-[27px] rounded-t-[13px] border-[3px] border-b-0 border-[#343434] bg-[#f7f4ed]" />
      <span className="absolute bottom-1.5 right-0 h-4 w-[50px] rounded-[4px_12px_4px_4px] border-[3px] border-[#343434] bg-[#ece8df]" />
      <span className="absolute bottom-0 right-[34px] z-10 size-[11px] rounded-full border-[3px] border-[#343434] bg-[#f8f6ef]" />
      <span className="absolute bottom-0 right-[7px] z-10 size-[11px] rounded-full border-[3px] border-[#343434] bg-[#f8f6ef]" />
      <span className="absolute bottom-3.5 left-0 h-3 w-[13px] border-y-[3px] border-dashed border-y-[#6c6a64]" />
    </span>
  )
}

function PixelCar() {
  return (
    // Decorative status car at the bottom of the controller.
    <span className="relative h-10 w-16 flex-none" aria-hidden="true">
      <span className="absolute left-8 top-1 h-5 w-1 bg-[#1d1d1d]" />
      <span className="absolute left-7 top-0 h-2 w-3 bg-[#d82f2a]" />
      <span className="absolute left-[22px] top-[15px] h-3.5 w-8 rounded-t-xl border-[3px] border-b-0 border-[#1e1e1e] bg-[linear-gradient(90deg,#1d1f21_0_48%,#f0f0ed_48%_58%,#454749_58%)]" />
      <span className="absolute left-[9px] top-6 h-4 w-[50px] rounded-[5px_12px_5px_5px] border-[3px] border-[#1e1e1e] bg-[linear-gradient(90deg,#d9362e_0_13%,transparent_13%_82%,#ec4b35_82%_100%),linear-gradient(180deg,#4b4d4f,#252627)]" />
      <span className="absolute left-3.5 top-[30px] size-[17px] rounded-full border-4 border-[#202020] bg-[radial-gradient(circle,#cf2d27_0_32%,#444_34%_48%,#181818_50%)]" />
      <span className="absolute right-2 top-[30px] size-[17px] rounded-full border-4 border-[#202020] bg-[radial-gradient(circle,#cf2d27_0_32%,#444_34%_48%,#181818_50%)]" />
    </span>
  )
}

function App() {
  return (
    // The app now fills the mobile viewport directly, with no fake phone frame.
    <main
      className="mx-auto flex min-h-dvh w-full min-w-80 max-w-[430px] flex-col overflow-hidden bg-[linear-gradient(90deg,rgba(255,255,255,0.62)_0_1px,transparent_1px_100%),linear-gradient(0deg,rgba(255,255,255,0.48)_0_1px,transparent_1px_100%),radial-gradient(circle_at_22%_8%,rgba(255,255,255,0.84),transparent_35%),linear-gradient(145deg,#f9f8f4_0%,#e7e2d9_100%)] bg-[length:auto,18px_18px,auto,auto] px-4 pb-[max(16px,env(safe-area-inset-bottom))] pt-[max(16px,env(safe-area-inset-top))] text-[#272727] sm:min-h-[844px] sm:px-5"
      aria-label="WiFi robot car controller"
    >
      {/* Connection and settings row. Buttons are visual only for now. */}
      <div className="flex items-center justify-between gap-3.5">
        <div className={`${panelChrome} inline-flex min-h-[34px] items-center gap-2.5 rounded-lg px-3.5 py-1.5 text-[clamp(0.7rem,2.75vw,0.9rem)] font-black text-[#2d7c17] [text-shadow:1px_1px_0_rgba(255,255,255,0.85)] max-[365px]:px-2.5`}>
          <span className="size-2.5 flex-none rounded-full bg-[#2d8f1b]" />
          <span>Connected</span>
        </div>

        <button
          className={`${panelChrome} grid size-12 cursor-default place-items-center rounded-[10px] text-[#3e3e3e]`}
          type="button"
          aria-label="Settings"
        >
          <GearIcon />
        </button>
      </div>

      {/* App title and decorative separator dots. */}
      <div className="mt-4 text-center">
        <h1 className={`${pixelText} text-[clamp(1.35rem,6.8vw,2.25rem)] leading-[1.08]`}>
          WiFi Robot Car
        </h1>
        <div className="mx-2 mb-4 mt-5 grid grid-cols-[1fr_auto_1fr] items-center gap-4" aria-hidden="true">
          <span className="h-1 rounded-full bg-[#bcb8ae]" />
          <div className="flex items-center gap-2.5">
            <span className="size-[11px] rounded-[3px] bg-[#d13d34]" />
            <span className="size-[11px] rounded-[3px] bg-[#d13d34]" />
            <span className="size-[11px] rounded-[3px] bg-[#d13d34]" />
          </div>
          <span className="h-1 rounded-full bg-[#bcb8ae]" />
        </div>
      </div>

      {/* Direction pad. The pale cross shape is decorative; each button remains static. */}
      <section className={`${panelChrome} relative h-[min(45dvh,390px)] min-h-[352px] overflow-hidden rounded-[30px]`} aria-label="Movement controls">
        <div className="absolute inset-9 rounded-[22px] bg-[linear-gradient(#ebe7df,#ebe7df),linear-gradient(#ebe7df,#ebe7df)] bg-[length:28%_100%,100%_28%] bg-center bg-no-repeat" aria-hidden="true" />
        {directions.map((item) => (
          <DirectionButton
            key={item.direction}
            label={item.label}
            direction={item.direction}
          />
        ))}
      </section>

      {/* Speed panel. The slider is just a static visual placeholder for now. */}
      <section className={`${panelChrome} mt-4 rounded-[26px] px-5 py-5 pb-[22px] sm:px-[26px]`} aria-label="Speed control">
        <div className="flex items-center justify-between gap-[18px]">
          <h2 className={`${pixelText} text-[clamp(1.1rem,4.3vw,1.55rem)] leading-none`}>
            Speed
          </h2>
          <MiniCarIcon />
        </div>

        <div className="relative mt-[26px] h-[18px] rounded-full border-[3px] border-[#333] bg-[linear-gradient(180deg,#171717,#30302f)]" aria-hidden="true">
          <span className="absolute inset-y-0 left-0 w-1/2 rounded-full bg-[linear-gradient(180deg,#3465bd,#1d448d)]" />
          <span className="absolute left-1/2 top-1/2 flex size-[52px] -translate-x-1/2 -translate-y-1/2 items-center justify-center gap-1.5 rounded-full border-[3px] border-[rgba(190,185,174,0.9)] bg-[linear-gradient(145deg,#fbfaf6,#dcd7ce)]">
            <span className="h-6 w-1 rounded-full bg-[#c8c4bb]" />
            <span className="h-6 w-1 rounded-full bg-[#c8c4bb]" />
            <span className="h-6 w-1 rounded-full bg-[#c8c4bb]" />
          </span>
        </div>

        <div className="mt-[22px] flex justify-between text-[clamp(0.9rem,3.5vw,1.15rem)] font-black leading-none [text-shadow:1px_2px_0_rgba(255,255,255,0.92)]">
          <span className="text-[#1d4a95]">Low</span>
          <span className="text-[#bf3029]">High</span>
        </div>
      </section>

      {/* Bottom readiness strip with a decorative pixel car. */}
      <footer className={`${panelChrome} mt-4 flex min-h-[76px] items-center justify-center gap-3 overflow-hidden rounded-3xl px-4 py-3 sm:gap-[18px] sm:px-[18px]`}>
        <span className="text-[clamp(1.2rem,5vw,1.7rem)] font-black leading-none text-[#8e8a81] max-[430px]:hidden" aria-hidden="true">
          ...
        </span>
        <PixelCar />
        <span className="text-[clamp(1rem,4vw,1.5rem)] font-black leading-none text-[#8e8a81] max-[430px]:hidden" aria-hidden="true">
          ...
        </span>
        <span className={`${pixelText} min-w-0 whitespace-nowrap text-[clamp(0.64rem,2.55vw,0.86rem)] leading-none`}>
          Ready to drive!
        </span>
        <span className="text-[clamp(1rem,4vw,1.5rem)] font-black leading-none text-[#8e8a81] max-[430px]:hidden" aria-hidden="true">
          ...
        </span>
      </footer>
    </main>
  )
}

export default App
