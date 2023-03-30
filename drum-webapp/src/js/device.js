import {reactive, ref} from 'vue'
import {mode} from '@/js/mode'
import {captureStarted, numSample, onReceiveNewDataForDataCollect, threshold} from '@/js/capture'
import { onDrumHit } from './drum'

const SERVICE_UUID = 'f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5'
const gesture_characteristic_UUID = '2f925c9d-0a5b-4217-8e63-2d527c9211c1'
// const imu_characteristic_UUID = 'f8edf338-6bbd-4c3b-bf16-d8d2b6cdaa6e'
const devices = {
  left: reactive({ name: '', gatt: null, gesChar: null, readyToWrite: true }),
  right: reactive({ name: '', gatt: null, gesChar: null, readyToWrite: true })
}

const cooldown = ref(40)
const COOLDOWN_RANGE = [20, 99]

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
      devices[deviceType].readyToWrite = true

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
  if (newVal.slice(1,3) === 'm0') {  // drum inference mode
    // console.log(newVal[2])
    onDrumHit(parseInt(newVal[3]))
  } else {  // data collection mode
    if (getConnectedDevices().length < 2 || newVal[0] === '0') {
      onReceiveNewDataForDataCollect(newVal.slice(1))
    }
  }
}

const getConnectedDevices = () => Object.keys(devices)
  .filter(dk => devices[dk].gatt && devices[dk].gatt.connected)
  .map(dk => devices[dk])


const updateDeviceParam = (m=mode.index, canCapture=captureStarted.value,sampleCount=numSample.value,th=threshold.value, coold=cooldown.value) => {
  let sentCount = 0
  // const formattedMsg = `${m}${canCapture ? 1 : 0}${sampleCount}${coold}${parseFloat(th).toFixed(3)}`
  const formattedMsg = `${m}${canCapture ? 1 : 0}${sampleCount}${coold}${parseFloat(th).toFixed(3)}`.substring(0, 20)
  const connectedDevices = getConnectedDevices()

  connectedDevices.forEach(async d => {
    try {
      // console.log(d.getChar)
      if (!d.readyToWrite) {
        console.warn('previous write operation still in progress, please update device params again later.')
        return 
      }
      d.readyToWrite = false
      await d.gesChar.writeValueWithResponse(encoder.encode(formattedMsg))
      d.readyToWrite = true
      console.log(`[-> ${d.name}]: mode(${m}) capture(${canCapture}) data(${sampleCount}) cooldown(${coold}) threshold(${th})`)
  
      sentCount++
    } catch (err) {
      console.warn('Error on updating device param. ', err)
    }
  })
    
  return sentCount == connectedDevices.length
}

export {devices, connectBTDevice, disconnectBTDevice, getConnectedDevices, updateDeviceParam, cooldown, COOLDOWN_RANGE}
