let websock;

// events name
const eventNames = {
    connectedUsersEventName: "user",

    timeEventName: "time",
    gateEventName: "gate",

    lightStatusEventName: "list",
    lightWorkingHoursFrom: "liwf",
    lightWorkingHoursTo: "liwt",

    shutterEventName: "shut",

    intervalsEventName: "inte",
    workingTimeEventName: "woti",
    idleTimeEventName: "idti",
    lastIntervalTimeEventName: "lati",
    estimatedFinishTimeEventName: "esti",
    workStatusEventName: "wost",

    isOpenGateAtWorkEventName:"isga",
    isLightOnAtWorkEventName: "isli",
    isPressButtonAtWorkEventName: "isbu",
};

// elements
const statusElement = document.getElementById('status');
const usersContainerElement = document.getElementById('users');

const timeElement = document.getElementById('time');

const gateElement = document.getElementById('gateStatus');

const lightElement = document.getElementById('lightStatus');
const lightValueElement = document.getElementById('lightValue');

const lightOnAtElement = document.getElementById('lightOnAt');
const lightOffAtElement = document.getElementById('lightOffAt');

const intervalsInputElement = document.getElementById('intervals');
const intervalIdleInputElement = document.getElementById('idleTime');
const intervalWorkInputElement = document.getElementById('workTime');
const lastIntervalTimeElement = document.getElementById('lastIntervalTime');
const processStatusElement = document.getElementById('processStatus');
const estimatedIntervalTimeElement = document.getElementById('estIntervalTime');

const isOpneBoxAtWorkElement = document.getElementById('isOpneBoxAtWork');
const isLigtOnAtWorkElement = document.getElementById('isLigtOnAtWork');
const isMakePhotoAtWorkElement = document.getElementById('isMakePhotoAtWork');

function start() {

    websock = webSocetConnect()

    websock.onopen = function(evt) {
        setStatus('online');
    };
    websock.onclose = function(evt) {
        setStatus('offline');
    };

    websock.onerror = function(evt) { 
        console.error(evt);
        setStatus('offline');
        setTimeout(() => {
            console.log('recconect...');
            websock = webSocetConnect();
        }, 5000);
    };

    websock.onmessage = function(evt) {
        console.log(evt);
        const data = JSON.parse(evt.data);

        switch (data.name) {
            case eventNames.connectedUsersEventName:
                usersContainerElement.innerText = data.value;
                break;
            case eventNames.timeEventName:
                setTime(data.value);
                break;
            case eventNames.gateEventName:
                setGateStatus(data.value);
                break;
            case eventNames.lightStatusEventName:
                setLightStatus(data.value);
                break;
            case eventNames.lightWorkingHoursFrom:
                setLightWorkingHoursFrom(data.value);
                break;
            case eventNames.lightWorkingHoursTo:
                setLightWorkingHoursTo(data.value);
                break;
            case eventNames.intervalsEventName:
                setIntarvals(data.value);
                break;
            case eventNames.idleTimeEventName:
                setIntervalIdleTime(data.value);
                break;
            case eventNames.workingTimeEventName:
                setIntarvalWorkTime(data.value);
                break;
            case eventNames.lastIntervalTimeEventName:
                setLastIntervalTime(data.value);
                break;
            case eventNames.estimatedFinishTimeEventName:
                setEstimatedIntervalTime(data.value);
                break;
            case eventNames.workStatusEventName:
                setIntervalStatus(data.value);
                break;
            case eventNames.isOpenGateAtWorkEventName:
                setIsOpneBoxAtWork(data.value);
                break;
            case eventNames.isLightOnAtWorkEventName:
                setIsLigtOnAtWork(data.value);
                break;
            case eventNames.isPressButtonAtWorkEventName:
                setIsMakePhotoAtWork(data.value);
                break;
            default:
                console.error(`unnkwon event name: ${JSON.stringify(data, null, 3)}`);
        }

    };
}

function webSocetConnect() {
    return new WebSocket('ws://' + window.location.hostname + ':81/');
}

function setStatus(status) {
    if (status === 'online') {
        statusElement.classList.add('online');
    } else if (status === 'offline') {
        statusElement.classList.remove('online');
    }
}


function setGateStatus(status) {
    if (status == 1) {
        gateElement.innerText = 'gate is opened';
    } else if (status == 0) {
        gateElement.innerText = 'gate is closed';
    }
}

function setLightStatus(status) {
    lightElement.value = status;
    lightValueElement.innerText = status;
}

function setLightWorkingHoursFrom(from) {
    lightOnAtElement.value = convertSecondsToStringHorst(from);
}

function setLightWorkingHoursTo(to) {
    lightOffAtElement.value = convertSecondsToStringHorst(to);
}

function setIntarvals(intervals) {
    intervalsInputElement.value = intervals;
}

function setIntervalIdleTime(intervals) {
    intervalIdleInputElement.value = intervals;
}

function setIntarvalWorkTime(intervals) {
    intervalWorkInputElement.value = intervals;
}

function setTime(time) {
    timeElement.innerText = time;
}

function setLastIntervalTime(time) {
    lastIntervalTimeElement.innerText = formatDate(new Date(time * 1000));
}

function setEstimatedIntervalTime(time) {
    estimatedIntervalTimeElement.innerText = formatDate(new Date(time * 1000));
}

function setIntervalStatus(status) {
    if (status == 1) {
        processStatusElement.innerText = 'working';
    } else if (status == 0) {
        processStatusElement.innerText = 'idle';
    }
}

function setIsOpneBoxAtWork(value) {
    isOpneBoxAtWorkElement.checked = value;
}

function setIsLigtOnAtWork(value) {
    isLigtOnAtWorkElement.checked = value;
}

function setIsMakePhotoAtWork(value) {
    isMakePhotoAtWorkElement.checked = value;
}

function sendGateEvent() {
    websock.send(getMessageToSend(eventNames.gateEventName, 0));
}

function sendIsOpenBoxAtWrok() {
    websock.send(getMessageToSend(eventNames.isOpenGateAtWorkEventName, Number(isOpneBoxAtWorkElement.checked)));
}

function sendIsLighOnAtWrok() {
    websock.send(getMessageToSend(eventNames.isLightOnAtWorkEventName, Number(isLigtOnAtWorkElement.checked)));
}

function sendIsMakePhotoAtWrok() {
    websock.send(getMessageToSend(eventNames.isPressButtonAtWorkEventName, Number(isMakePhotoAtWorkElement.checked)));
}

function sendLightEvent() {
    lightValueElement.innerText = lightElement.value;
    websock.send(getMessageToSend(eventNames.lightStatusEventName, lightElement.value));
}

function sendEventWorkingHours() {
    const timeOnToSec = convertStringHorstToSeconds(lightOnAtElement.value);
    const timeOffToSec = convertStringHorstToSeconds(lightOffAtElement.value);
    setLightWorkingHoursFrom(timeOnToSec);
    setLightWorkingHoursTo(timeOffToSec);
    websock.send(getMessageToSend(eventNames.lightWorkingHoursFrom, timeOnToSec));
    websock.send(getMessageToSend(eventNames.lightWorkingHoursTo, timeOffToSec));
}

function resetWorkingHors() {
    lightOnAtElement.value = '00:00';
    lightOffAtElement.value = '00:00';
    sendEventWorkingHours();
}

function sendShutterEvent() {
    websock.send(getMessageToSend(eventNames.shutterEventName, 1));
}

function sendEventIntervals() {
    websock.send(getMessageToSend(eventNames.intervalsEventName, intervalsInputElement.value));
    websock.send(getMessageToSend(eventNames.workingTimeEventName, intervalWorkInputElement.value));
    websock.send(getMessageToSend(eventNames.idleTimeEventName, intervalIdleInputElement.value));
}

function getMessageToSend(name, value) {
    return JSON.stringify({name, value});
}

function formatDate(d) {
    try {
        const offset = d.getTimezoneOffset() * 60000;
        const date = new Date(d.getTime() + offset);
        return date.toLocaleString();
        // return `${formatNumber(d.getDate())}.${formatNumber(d.getMonth() + 1)}.${d.getFullYear()} ${formatNumber(d.getHours())}:${formatNumber(d.getMinutes())}`;
    } catch(e) {
        console.log(e);
        return 'error format';
    }
}

function convertStringHorstToSeconds(str) {
    const strArr = str.split(':');
    if (strArr[0] && strArr[1]) {
        return Number(strArr[0]) * 60 * 60 + Number(strArr[1] * 60);
    }
}

function convertSecondsToStringHorst(rawSec) {
    const hours = Math.floor(rawSec / 60 / 60);
    const hoursToSec = hours * 60 * 60;
    const min = Math.floor((rawSec - hoursToSec)/60);
    const minToSec = min * 60;
    const sec = rawSec - hoursToSec - minToSec;
    const stringHours = `${formatNumber(hours)}:${formatNumber(min)}:${formatNumber(sec)}`;
    return stringHours;
}

function formatNumber(n) {
    if (n < 9) {
        return '0' + n;
    } else {
        return n;
    }
}

start();
