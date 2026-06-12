const DEFAULT_ROBOT_HOST = 'robotcar.local'
const SETUP_AP_HOST = '192.168.4.1'
const STORAGE_KEY = 'robotCarHost'
const COMMAND_REPEAT_MS = 120

let activeCommand = null
let activeRequest = null
let repeatTimer = null

function normalizeHost(host) {
    return host.replace(/^https?:\/\//, '').replace(/\/.*$/, '').trim()
}

function getRobotHost() {
    const params = new URLSearchParams(window.location.search)
    const queryHost = params.get('robot') || params.get('host')
    const configuredHost = queryHost || import.meta.env.VITE_ROBOT_HOST || localStorage.getItem(STORAGE_KEY)

    if (configuredHost) {
        const host = normalizeHost(configuredHost)

        if (queryHost) {
            localStorage.setItem(STORAGE_KEY, host)
        }

        return host
    }

    if (window.location.hostname === SETUP_AP_HOST) {
        return SETUP_AP_HOST
    }

    return DEFAULT_ROBOT_HOST
}

const send = async (command, options = {}) => {
    const robotHost = getRobotHost()

    try {
        await fetch(`http://${robotHost}/${command}`, {
            cache: 'no-store',
            signal: options.signal,
        });
    } catch (error) {
        if (error.name === 'AbortError') {
            return
        }

        console.error(`Could not send "${command}" to ${robotHost}`, error)
    }
};

function clearRepeat() {
    if (repeatTimer !== null) {
        clearTimeout(repeatTimer)
        repeatTimer = null
    }

    if (activeRequest !== null) {
        activeRequest.abort()
        activeRequest = null
    }
}

async function repeatCommand(command) {
    if (activeCommand !== command) {
        return
    }

    const controller = new AbortController()
    activeRequest = controller

    await send(command, { signal: controller.signal })

    if (activeRequest === controller) {
        activeRequest = null
    }

    if (activeCommand === command) {
        repeatTimer = setTimeout(() => repeatCommand(command), COMMAND_REPEAT_MS)
    }
}

export function startCommand(command) {
    if (activeCommand === command) {
        return
    }

    clearRepeat()
    activeCommand = command
    console.log(`Start command: ${command}`)
    repeatCommand(command)
}

export function stopCommand(command = 's') {
    if (activeCommand === null && repeatTimer === null) {
        return
    }

    console.log(`Stop command: ${command}`)
    activeCommand = null
    clearRepeat()
    send(command)
}

window.addEventListener('blur', () => stopCommand())
window.addEventListener('pagehide', () => stopCommand())
window.addEventListener('visibilitychange', () => {
    if (document.visibilityState === 'hidden') {
        stopCommand()
    }
})
