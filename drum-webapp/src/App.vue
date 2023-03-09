<template>
  <div class="drum-web-app">
    <div class="drum-view">
      drum view
      <div class="log_left">{{ logLeftString }}</div>
      <div class="log_right"></div>
    </div>
    <div class="options">
      <div class="connect-button" @click="onConnect">Connect</div>
    </div>
  </div>
</template>

<script>
import {ref} from 'vue'

const SERVICE_UUID = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5"
const characteristic_UUID = "2f925c9d-0a5b-4217-8e63-2d527c9211c1"
export default {
  name: 'drum-web-app',
  setup () {
    const logLeftString = ref("")
    const decoder = new TextDecoder('utf-8')
    const onCharChange = e => {
      const value = e.target.value
      console.log('val: ', decoder.decode(value))
    }

    const onConnect = () => {
      navigator.bluetooth.requestDevice({ filters: [{ services: [SERVICE_UUID] }]})
      .then(device => {
        console.log(device.name)
        return device.gatt.connect()
      })
      .then(server => server.getPrimaryService(SERVICE_UUID))
      .then(service => service.getCharacteristic(characteristic_UUID))
      .then(characteristic => characteristic.startNotifications())
      .then(characteristic => {
        characteristic.addEventListener('characteristicvaluechanged', onCharChange)
      })
      .catch(err => console.error(err))
    }


    return {onConnect, logLeftString}
  }
}
</script>

<style lang="scss">
body {
  font-family: sans-serif;
  font-size: 20px;
  padding: 0;
  margin: 0;
}

.drum-web-app {
  max-width: 1200px;
  height: 100vh;
  margin: 0 auto;
  display: flex;
  flex-direction: column;

  .drum-view {
    height: calc(100% - 150px);
    background-color: green;
  }

  .options {
    background-color: grey;
    height: 150px;
    .connect-button {
      display: inline-block;
      padding: 6px 20px;
      border-radius: 20px;
      color: #FFFFFF;
      background-color: rgba(#9e9e9e, 0.75);
      cursor: pointer;

      &:hover {
        background-color: #9e9e9e;
      }
    }
  }
}
</style>
