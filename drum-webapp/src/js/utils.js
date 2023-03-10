const SERVICE_UUID = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5"
const characteristic_UUID = "2f925c9d-0a5b-4217-8e63-2d527c9211c1"

const DRUM_CODES = {
  'q': 0,
  'w': 1,
  'a': 2,
  's': 3,
  'e': 4,
  'r': 5,
  'd': 6,
  'f': 7
}

const decoder = new TextDecoder('utf-8')

const connectBTDevice = cb => {
  navigator.bluetooth.requestDevice({ filters: [{ services: [SERVICE_UUID] }]})
  .then(device => {
    console.log(device.name)
    return device.gatt.connect()
  })
  .then(server => server.getPrimaryService(SERVICE_UUID))
  .then(service => service.getCharacteristic(characteristic_UUID))
  .then(characteristic => characteristic.startNotifications())
  .then(characteristic => characteristic.addEventListener('characteristicvaluechanged', e => decoder.decode(e.target.value)))
  .catch(err => console.warn(err))
}

const setupKeyboardInput = cb => {
  window.addEventListener('keypress', e => {
    if (DRUM_CODES[e.key] || DRUM_CODES[e.key] === 0) cb(DRUM_CODES[e.key])
  })
}

export {connectBTDevice, setupKeyboardInput}
