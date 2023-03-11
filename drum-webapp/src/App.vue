<template>
  <div class="drum-web-app">
    <transition>
      <div v-if="!audioReady" class="virtual-drum-entry">
        <div v-if="!isLoading" class="load-button" @click="onLoad">Load Virtual Drum</div>
        <div v-if="isLoading" class="load-visual">Loading...</div>
      </div>
    </transition>
    <DrumVisual :class="curType" :highlights="highlights"></DrumVisual>
    <div class="drum-type">
      <RadioButton v-model="curType" :options="DRUM_TYPES"></RadioButton>
    </div>
    
    <div class="option">
      <div class="option-content">
        <div class="connect-button" @click="onConnect">Connect BT devices</div>
      </div>
    </div>
  </div>
</template>

<script>
import {ref, reactive} from 'vue'
import { connectBTDevice, setupKeyboardInput } from './js/utils'
import {audioReady, isLoading, loadAudioDatas, play} from './js/audio'
import RadioButton from '@/components/RadioButton.vue'
import DrumVisual from '@/components/DrumVisual.vue'
// import DrumSelect from '@/components/DrumSelect.vue'


const DRUM_TYPES = ['General', 'Analog', 'Latin']

export default {
  name: 'drum-web-app',
  components: {DrumVisual, RadioButton},
  setup () {
    console.log(navigator.bluetooth)
    const curType = ref(DRUM_TYPES[0])

    const onLoad = () => loadAudioDatas(() => setupKeyboardInput(onNewCode))

    const onConnect = () => {
      connectBTDevice(newVal => {
        console.log('val: ', newVal)
      })
    }

    const onNewCode = drumCode => {
      play(curType.value, drumCode)
      setHightlight(drumCode)
    }

    const highlights = reactive({0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0, 6: 0, 7: 0})
    const hCountDown = []
    const setHightlight = i => {
      highlights[i] = 1

      if (hCountDown[i]) {
        cancelAnimationFrame(hCountDown[i])
        hCountDown[i] = null
      }

      hCountDown[i] = () => {
        highlights[i] -= 0.1

        if (highlights[i] <= 0) {
          cancelAnimationFrame(hCountDown[i])
          hCountDown[i] = null
          highlights[i] = 0
        } else requestAnimationFrame(hCountDown[i])
      }

      requestAnimationFrame(hCountDown[i])
    }

    return {curType, highlights, onLoad, onConnect, audioReady, isLoading, DRUM_TYPES}
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

  .virtual-drum-entry {
    position: fixed;
    z-index: 1;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(black, 0.95);

    .load-button {
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
      transition: transform $tt, background-color $tt, color $tt, padding $tt;

      &:hover {
        transform: translate(-50%, -50%) scale(1.05);
        background-color: #555555;
        color: #FFFFFF;
        padding: 15px 25px;
      }
    }
    
    .load-visual {
      position: absolute;
      color: white;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
    }
  }

  .drum-visual {
    height: calc(100% - 150px);
    overflow: hidden;
  }
  
  .radio-button {
    position: fixed;
    z-index: 0;
    right: 20px;
    top: 20px;
  }

  .option {
    // background-color: grey;
    height: 150px;

    .option-content {
      margin: 0 auto;
      max-width: 600px;
      padding: 20px;
      box-sizing: border-box;
      background-color: #666666;

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
}
</style>
