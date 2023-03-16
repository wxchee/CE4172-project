<template>
  <div class="drum-web-app">
    <transition>
      <div v-if="!audioReady" class="loader">
        <div v-if="!isAudioLoading" class="loader__button" @click="loadAudioDatas">Load Virtual Drum</div>
        <div v-if="isAudioLoading" class="loader__visual">Loading...</div>
      </div>
    </transition>
    <div class="navbar">
      <div v-for="(opt, i) in VIEWS" :key="i"
        :class="navbarOptionClass(opt)" 
        @click="() => onNavClick(opt, i)">{{ opt }}</div>
    </div>
    <div class="view-container">
      <DemoView v-if="curView == VIEWS[0]"></DemoView>
      <DataCollectView v-else></DataCollectView>
    </div>
    <DevicePanel></DevicePanel>
  </div>
</template>

<script>
import { ref } from 'vue'
import {audioReady, isAudioLoading, loadAudioDatas} from '@/js/audio'
import { updateServerMode } from '@/js/device'
import DemoView from './components/DemoView.vue'
import DataCollectView from './components/DataCollectView.vue'
import DevicePanel from './components/DevicePanel.vue'


const VIEWS = ['Demo', 'Data Collection']

export default {
  name: 'drum-web-app',
  components: { DemoView, DataCollectView, DevicePanel },
  setup () {
    console.log(navigator.bluetooth)
    const curView = ref(VIEWS[0])

    const navbarOptionClass = (opt) => {
      return {
        'navbar__option': true,
        'current': opt === curView.value
      }
    }

    const onNavClick = (newOpt, modeIndex) => {
      curView.value = newOpt
      updateServerMode(modeIndex.toString())
    }

    return {curView, VIEWS, navbarOptionClass, onNavClick, loadAudioDatas, audioReady, isAudioLoading}
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
  position: relative;
  max-width: 1200px;
  height: 100vh;
  margin: 0 auto;
  user-select: none;

  display: flex;
  flex-direction: column;

  .navbar {
    display: flex;
    width: 100%;
    color: #FFFFFF;
    justify-content: center;
    gap: 20px;
    padding: 10px;
    box-sizing: border-box;


    & > * {
      opacity: 0.3;
      cursor: pointer;
      transition: opacity 0.2s;

      &.current {
        opacity: 1;
        cursor: none;
        pointer-events: none;
      }

      &:hover {
        opacity: 0.9;
      }
    }
  }

  .view-container {
    height: 100%;

    & > * {
      width: 100%;
      height: 100%;
    }
  }

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

  
}
</style>
