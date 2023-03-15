<template>
  <div class="device-panel">
    <GenericButton class="connected" v-for="(d, i) in getConnectedDevices()" :key="i"
      @click="() => onDisconnect(d)">{{d.name}}</GenericButton>
    <GenericButton v-if="getConnectedDevices().length < 2" @click="onConnect">Add device</GenericButton>
  </div>
</template>

<script>
import GenericButton from '@/components/GenericButton.vue'
import {connectBTDevice, disconnectBTDevice, getConnectedDevices} from '@/js/device'
import { trigger } from './DataPanel.vue'


export default {
  components: {GenericButton},
  setup(props) {
    const onConnect = () => {
      connectBTDevice(newVal => {
        console.log(newVal)
      })
    }

    const onDisconnect = d => disconnectBTDevice(d)

    return {onConnect, onDisconnect, getConnectedDevices}
  }
}
</script>

<style lang="scss">
.device-panel {
  display: flex;
  align-items: stretch;
  align-content: stretch;
  padding: 20px;

  & > * {
    position: relative;
    width: 50%;
    height: 80px;

    text-align: center;
    vertical-align: middle;
    line-height: 80px;
    white-space: nowrap;
    transition: background-color 0.15s, border 0.15s;

    &.connected {
      border: 5px solid rgb(32, 126, 71);
      background-color: rgb(70, 199, 124);

      &:hover {
        border: 5px solid rgb(211, 35, 0);
        background-color: rgb(255, 157, 121);
        opacity: 0.8;
        color: transparent;

        &::after {
          display: block;
          content: 'Disconnect';
          position: absolute;
          color: #FFFFFF;
          font-size: 22px;
          font-family: sans-serif;
          top: 50%;
          left: 50%;
          transform: translate(-50%, -50%);
          width: 100%;
          height: 100%;
          line-height: 88px;
        }
      }
    }
  }
}
</style>