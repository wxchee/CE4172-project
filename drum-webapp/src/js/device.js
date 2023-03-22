import {reactive} from 'vue'
import {mode} from '@/js/mode'
import {captureStarted, numSample, onReceiveNewDataForDataCollect, threshold} from '@/js/capture'
import { onDrumHit } from './drum'

const SERVICE_UUID = 'f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5'
const gesture_characteristic_UUID = '2f925c9d-0a5b-4217-8e63-2d527c9211c1'
// const imu_characteristic_UUID = 'f8edf338-6bbd-4c3b-bf16-d8d2b6cdaa6e'
const devices = {
  left: reactive({ name: '', gatt: null, gesChar: null, imuChar: null }),
  right: reactive({ name: '', gatt: null, gesChar: null, imuChar: null })
}

const encoder = new TextEncoder('utf-8')
const decoder = new TextDecoder('utf-8')

const connectBTDevice = async cb => {
  let gestureChar = null

  try {
    let device = await navigator.bluetooth.requestDevice({ filters: [{ services: [SERVICE_UUID] }]})
    const deviceType = device.name == 'DRUM_L' ? 'left' : 'right'
    
    device.addEventListener('gattserverdisconnected', () => {
      console.log(device.name, ' disconnect')
      devices[deviceType].gatt = null
      devices[deviceType].gesChar = null
    })

    let gatt = await device.gatt.connect()
    let service = await gatt.getPrimaryService(SERVICE_UUID)
    gestureChar = await service.getCharacteristic(gesture_characteristic_UUID)
    if (gestureChar.properties.notify) {
      gestureChar.addEventListener('characteristicvaluechanged', onReceiveIncomingData)
      await gestureChar.startNotifications()

      devices[deviceType].name = device.name
      devices[deviceType].gatt = device.gatt
      devices[deviceType].gesChar = gestureChar

      if (cb) cb(true)
      console.log(device.name, 'connected.')

      // updateDeviceMode()
      updateDeviceParam()
    }
  } catch (err) {
    if (cb) cb(false)
    console.warn('Error occurs during BT device connection.', err)
  }
}

const disconnectBTDevice = async d => {
  if (d.gatt.connected) {
    await d.gatt.disconnect()
  } else {
    console.log('device is already disconnected')
  }
}

const onReceiveIncomingData = e => {
  const newVal = decoder.decode(e.target.value)
  if (newVal.slice(0,2) === 'm0') {  // drum inference mode
    // console.log(newVal[2])
    onDrumHit(parseInt(newVal[2]))
  } else {  // data collection mode
    onReceiveNewDataForDataCollect(newVal)
  }
}

const getConnectedDevices = () => Object.keys(devices)
  .filter(dk => devices[dk].gatt && devices[dk].gatt.connected)
  .map(dk => devices[dk])


const updateDeviceParam = (m=mode.index, canCapture=captureStarted.value,sampleCount=numSample.value,th=threshold.value) => {
  let sentCount = 0
  const formattedMsg = `${m}${canCapture ? 1 : 0}${sampleCount}${parseFloat(th).toFixed(3)}`
  console.log(formattedMsg)
  const connectedDevices = getConnectedDevices()

  connectedDevices.forEach(async d => {
    await d.gesChar.writeValueWithResponse(encoder.encode(formattedMsg))

    console.log(`[-> ${d.name}]: mode(${m}) canCapture(${canCapture}) sampleCount(${sampleCount}) threshold(${th})`)

    sentCount++
  })

  return sentCount == connectedDevices.length
}

export {devices, connectBTDevice, disconnectBTDevice, getConnectedDevices, updateDeviceParam}
