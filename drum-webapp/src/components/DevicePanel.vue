<template>
  <div class="device-panel">
    <div class="device-panel-container" :class="{'inView': inView}" @blur="onFocusOut">
      <div class="device-panel__content">
        <GenericButton class="device-panel__button connected" v-for="(d, i) in connectedDevices" :key="i"
          @click="() => onDisconnect(d)">{{d.name}}</GenericButton>
        <GenericButton class="device-panel__button " v-if="connectedDevices.length < 2" @click="onConnect">&#43; Add device</GenericButton>
      </div>
      <div class="device-panel__entry" @click="onToggleView">
        <svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
          viewBox="0 0 20 20" enable-background="new 0 0 20 20" xml:space="preserve">
        <polyline fill="none" stroke="#D1D3D4" stroke-width="1.5" stroke-miterlimit="10" points="6.35,6.04 13.65,13.34 10.14,16.85 
          10.14,3.15 13.63,6.65 6.67,13.62 "/></svg>
          <span>{{ connectedDevices.length }}</span>
        </div>
    </div>
    
  </div>
</template>

<script>
import GenericButton from '@/components/GenericButton.vue'
import {connectBTDevice, disconnectBTDevice, getConnectedDevices} from '@/js/device'
import { computed, ref } from 'vue'



export default {
  components: {GenericButton},
  setup(props) {
    const inView = ref(false)
    const connectedDevices = computed(() => getConnectedDevices())
    const onConnect = () => {
      connectBTDevice(newVal => {
        console.log(newVal)
      })
    }

    const onDisconnect = d => disconnectBTDevice(d)

    const checkIfClickWithinView = e => {
      if (e.target.classList.value.indexOf('device-panel') === -1) {
        inView.value = e.target.classList.value.indexOf('device-panel') !== -1
      }
    }
    
    const onToggleView = () => {
      inView.value = !inView.value

      if (inView.value) window.addEventListener('click', checkIfClickWithinView)
      else window.removeEventListener('click', checkIfClickWithinView)
    }
    const onFocusOut = () => {
      console.log('oout!')
    }
    return {onConnect, onDisconnect, onFocusOut, connectedDevices, inView, onToggleView}
  }
}
</script>

<style lang="scss">
.device-panel {
  position: fixed;
  top: 0;
  left: 0;

  .device-panel-container {
    position: absolute;
    z-index: 1;
    top: 0;
    left: 0;
    transform: translateX(calc(-100% + 40px));
    display: flex;
    align-items: start;
    transition: transform 0.2s;

    &.inView {
      transform: translateX(0);
    }

    .device-panel__entry {
      transform: translateY(10px);
      color: #FFFFFF;
      line-height: 1;
      width: 40px;
      height: 35px;
      background-color:#0082FC;
      display: flex;
      align-items: center;
      justify-content: center;
      box-sizing: border-box;
      cursor: pointer;
      transition: filter 0.2s;
      &:hover {
        filter:brightness(0.8);
      }

      & > * {
        user-select: none;
        pointer-events: none;
      }
      
      svg {
        width: 22px;
      }

      span {
        font-size: 15px;
      }
    }

    .device-panel__content {
      padding: 20px 15px;
      background-color:#002b53;
      border-right: 3px solid #0082FC;
      & > * {
        position: relative;
        text-align: center;
        vertical-align: middle;
        line-height: 1;
        white-space: nowrap;
        transition: background-color 0.15s, border 0.15s;
        &:not(:last-child) {
          margin-bottom: 10px;
        }

        &.connected {
          position: relative;
          border: 3px solid rgb(32, 126, 71);
          background-color: rgb(70, 199, 124);
          box-sizing: border-box;

          &:hover {
            border: 3px solid rgb(62, 62, 62);
            background-color: rgb(97, 97, 97);
            // opacity: 0.8;
            color: transparent;

            &::after {
              display: block;
              content: 'Disconnect';
              position: absolute;
              color: #FFFFFF;
              font-size: 20px;
              font-family: sans-serif;
              top: 50%;
              left: 50%;
              transform: translate(-50%, -50%);
              width: 100%;
              height: 100%;
              line-height: 32px;
            }
          }
        }
      }
    }
  }

  
  
}
</style>