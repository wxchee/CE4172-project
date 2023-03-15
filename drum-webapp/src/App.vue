<template>
  <div class="drum-web-app">
    <transition>
      <div v-if="!audioReady" class="loader">
        <div v-if="!isAudioLoading" class="loader__button" @click="loadAudioDatas">Load Virtual Drum</div>
        <div v-if="isAudioLoading" class="loader__visual">Loading...</div>
      </div>
    </transition>
    <DrumVisual></DrumVisual>
    <div class="drum-type">
      <RadioButton v-model="drumType" :options="DRUM_TYPES"></RadioButton>
    </div>
    
    <div class="option">
      <div class="option-content">
        <DevicePanel></DevicePanel>
        <DataPanel></DataPanel>
      </div>
    </div>
  </div>
</template>

<script>
import {drumType, DRUM_TYPES} from '@/js/drum'
import {audioReady, isAudioLoading, loadAudioDatas} from '@/js/audio'
import RadioButton from '@/components/RadioButton.vue'
import DrumVisual from '@/components/DrumVisual.vue'
import DevicePanel from '@/components/DevicePanel.vue'
import DataPanel from '@/components/DataPanel.vue'

export default {
  name: 'drum-web-app',
  components: {DrumVisual, RadioButton, DevicePanel, DataPanel},
  setup () {
    console.log(navigator.bluetooth)

    return {drumType, DRUM_TYPES, loadAudioDatas, audioReady, isAudioLoading}
  }
}
</script>

<style lang="scss">
$tt: 0.2s;
body {
  font-family: sans-serif;
  font-size: 20px;
  padding: 0;
  margin: 0;
  background-color: #111111;
  height: 100vh;
  overflow: hidden;
}

.v-enter-active,
.v-leave-active {
  transition: opacity 0.5s ease;
}

.v-enter-from,
.v-leave-to {
  opacity: 0;
}

.drum-web-app {
  max-width: 1200px;
  height: 100vh;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  user-select: none;

  .loader {
    position: fixed;
    z-index: 1;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(black, 0.95);

    .loader__button {
      position: absolute;
      color: #333333;
      font-size: 25px;
      font-weight: 700;
      line-height: 1;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      padding: 10px 20px;
      border-radius: 30px;
      background-color: #FFFFFF;
      cursor: pointer;
      white-space: nowrap;
      transition: transform $tt, background-color $tt, color $tt, padding $tt;

      &:hover {
        transform: translate(-50%, -50%) scale(1.05);
        background-color: #555555;
        color: #FFFFFF;
        padding: 15px 25px;
      }
    }
    
    .loader__visual {
      position: absolute;
      color: white;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
    }
  }

  .drum-visual {
    height: calc(100% - 200px);
    overflow: hidden;
  }
  
  .radio-button {
    position: fixed;
    z-index: 0;
    right: 20px;
    top: 20px;
    text-transform: capitalize;
  }

  .option {
    height: 200px;

    .option-content {
      margin: 0 auto;
      max-width: 600px;
      background-color: #666666;
    }
  }
}
</style>
