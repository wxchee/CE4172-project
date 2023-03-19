import { getConnectedDevices } from "./device"
import {ref, reactive} from 'vue'
const SAMPLE_RAMGE = [10, 50]
const THRESHOLD_RANGE = [0, 0.6]
let curSample = 0
let canCapture = false

const threshold = ref((THRESHOLD_RANGE[0] + (THRESHOLD_RANGE[1] - THRESHOLD_RANGE[0]) * 0.3).toFixed(3))
let captureStarted = ref(false)
const numSample = ref(20)
const buffer = reactive({ aX: 0, aY: 0, aZ: 0, gX: 0, gY: 0, gZ: 0 })
const capturedBuffer = ref([])
const selectedCapIndex = ref(-1)
const captureSnaphot = reactive({t: 0, val: []})

let temp = []
const onReceiveNewDataForDataCollect = async newVal => {
  const [aX, aY, aZ, gX, gY, gZ] = newVal.split(",").map(val => parseFloat(val))
    buffer.aX = aX
    buffer.aY = aY
    buffer.aZ = aZ
    buffer.gX = gX
    buffer.gY = gY
    buffer.gZ = gZ
  
    if (captureStarted.value) {
      if (!canCapture && ((Math.abs(aX) + Math.abs(aY) + Math.abs(aZ) +
          Math.abs(gX) + Math.abs(gY) + Math.abs(gZ)) / 6).toFixed(3) >= threshold.value) {
            canCapture = true
            curSample = 0
            temp = []
        }

        if (canCapture) {
          temp.push(newVal)
          curSample++

          if (curSample === parseInt(numSample.value)) {
            canCapture = false
            curSample = 0
            capturedBuffer.value.push({t: Date.now(), val: temp})
          }
        }
    }

    if (captureSnaphot.val.length >= numSample.value) captureSnaphot.val.splice(0, captureSnaphot.val.length - numSample.value + 1)
    captureSnaphot.t = Date.now()
    captureSnaphot.val.push(newVal)
    
}

const startCapture = () => {
  if (!getConnectedDevices().length) return
  captureStarted.value = !captureStarted.value
  if(captureStarted.value) {
    selectedCapIndex.value = -1
  }
}

const pauseCapture = () => {
  captureStarted.value = false
  canCapture = false
  curSample = 0
}

const resetCapture = () => {
  capturedBuffer.value = []
  selectedCapIndex.value = -1

  canCapture = false
  curSample = 0
}

const removeCapturedItem = (capturedIndex, e) => {
  capturedBuffer.value.splice(capturedIndex, 1)
  if (selectedCapIndex.value == capturedIndex && selectedCapIndex.value === capturedBuffer.value.length) {
    console.log('match', selectedCapIndex.value, capturedIndex)
    selectedCapIndex.value =  capturedIndex - 1
  }

  if (e) e.stopPropagation()
}

const hasAvailableDevices = () => {
  return getConnectedDevices().length
}

export {
  SAMPLE_RAMGE, THRESHOLD_RANGE, threshold, captureStarted, numSample, buffer, capturedBuffer, selectedCapIndex, captureSnaphot,
  onReceiveNewDataForDataCollect, startCapture, pauseCapture, resetCapture, removeCapturedItem, hasAvailableDevices
}
