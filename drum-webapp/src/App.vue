<template>
  <div class="drum-web-app">
    <transition>
      <LoadingVisual v-if="!audioReady"></LoadingVisual>
    </transition>
    <DrumSet :highlights="highlights"></DrumSet>
    <div class="options">
      <div class="connect-button" @click="onConnect">Connect</div>
    </div>
  </div>
</template>

<script>
import {reactive, watch} from 'vue'
import { connectBTDevice, setupKeyboardInput } from './js/utils'
import {audioReady, play} from './js/audio'
import DrumSet from '@/components/DrumSet.vue'
import LoadingVisual from '@/components/LoadingVisual.vue'


export default {
  name: 'drum-web-app',
  components: {DrumSet, LoadingVisual},
  setup () {
    console.log(navigator.bluetooth)
    const highlights = reactive({0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0, 6: 0, 7: 0})
    const hCountDown = []
    const onConnect = () => {
      connectBTDevice(newVal => {
        console.log('val: ', newVal)
      })
    }

    const onNewCode = drumCode => {
      play(drumCode)
      setHightlight(drumCode)
    }

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

    watch(
      () => audioReady.value,
      newVal => {
        if (newVal) setupKeyboardInput(onNewCode)
      })
    

    return {highlights, onConnect, audioReady}
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

  .drum-set {
    height: calc(100% - 150px);
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
