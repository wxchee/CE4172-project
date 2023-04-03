<template>
  <div class="drum-web-app">
    <transition>
      <div v-if="!audioReady" class="loader">
        <div v-if="!isAudioLoading" class="loader__button" @click="loadAudioDatas">Load Virtual Drum</div>
        <div v-if="isAudioLoading" class="loader__visual">Loading...</div>
      </div>
    </transition>
    <div class="navbar">
      <div v-for="(v, i) in VIEWS" :key="i"
        :class="navbarOptionClass(v)" 
        @click="() => setView(v)">{{ v.label }}</div>
    </div>
    <div class="view-container">
      <DemoView v-if="view.index === VIEWS[0].index"></DemoView>
      <DataCollectView v-else></DataCollectView>
    </div>
    <DevicePanel></DevicePanel>
  </div>
</template>

<script>
import {audioReady, isAudioLoading, loadAudioDatas} from '@/js/audio'
import DemoView from './components/DemoView.vue'
import DataCollectView from './components/DataCollectView.vue'
import DevicePanel from './components/DevicePanel.vue'
import {VIEWS, view, setView} from '@/js/view'
export default {
  name: 'drum-web-app',
  components: { DemoView, DataCollectView, DevicePanel },
  setup () {
    if (!navigator.bluetooth)
      console.warn('Please use a browser that supports web bluetooth api')
    

    const navbarOptionClass = newView => {
      return {
        'navbar__option': true,
        'current': newView.index === view.index
      }
    }

    return {
      view, VIEWS, setView, navbarOptionClass, loadAudioDatas, audioReady, isAudioLoading,
    }
  }
}
</script>

<style lang="scss">
$tt: 0.2s;
body {
  font-family: sans-serif;
  font-size: 18px;
  padding: 0;
  margin: 0;
  background-color: #000000;
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
    gap: 30px;
    padding: 10px;
    box-sizing: border-box;
    font-size: 21px;


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
  } // navbar

  .view-container {
    height: 100%;
    overflow-y: hidden;

    & > * {
      width: 100%;
      height: 100%;
      overflow-y: hidden;
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
