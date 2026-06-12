const IP = '192.168.4.1'

let interval = null;

const send = async (command) => {
    await fetch(`http://${IP}/${command}`);
};

export function startCommand(command) {
    if (interval !== null){
        clearInterval(interval);
    }
    console.log(`Start command: ${command}`)
    send(command)
    interval = setInterval(() => {
        send(command)
    }, 100);
}

export function stopCommand(command) {
    console.log(`Stop command: ${command}`)
    clearInterval(interval);
    interval = null;
    send(command)
}



