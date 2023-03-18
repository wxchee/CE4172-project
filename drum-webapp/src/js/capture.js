import { getConnectedDevices } from "./device"
import {ref, reactive, nextTick} from 'vue'
const SAMPLE_RAMGE = [10, 100]
const THRESHOLD_RANGE = [0, 0.8]
let curSample = 0
let canCapture = false

const threshold = ref((THRESHOLD_RANGE[0] + (THRESHOLD_RANGE[1] - THRESHOLD_RANGE[0]) * 0.2).toFixed(2))
let captureStarted = ref(false)
const numSample = ref(SAMPLE_RAMGE[0])
const buffer = reactive({ aX: 0, aY: 0, aZ: 0, gX: 0, gY: 0, gZ: 0 })
const capturedBuffer = ref([])
const selectedCapIndex = ref(-1)

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
          Math.abs(gX) + Math.abs(gY) + Math.abs(gZ)) / 6).toFixed(2) >= threshold.value) {
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
}

const startCapture = () => {
  if (!getConnectedDevices().length) return
  captureStarted.value = !captureStarted.value
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

const removeCapturedItem = capturedIndex => {
  capturedBuffer.value.splice(capturedIndex, 1)
}

const isCaptureDisabled = () => {
  return !getConnectedDevices().length
}

export {
  SAMPLE_RAMGE, THRESHOLD_RANGE, threshold, captureStarted, numSample, buffer, capturedBuffer, selectedCapIndex,
  onReceiveNewDataForDataCollect, startCapture, pauseCapture, resetCapture, removeCapturedItem, isCaptureDisabled
}
