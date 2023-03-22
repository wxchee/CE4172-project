<template>
  <div class="drum-web-app">
    <transition>
      <div v-if="!audioReady" class="loader">
        <div v-if="!isAudioLoading" class="loader__button" @click="loadAudioDatas">Load Virtual Drum</div>
        <div v-if="isAudioLoading" class="loader__visual">Loading...</div>
      </div>
    </transition>
    <div class="navbar">
      <div v-for="(mode, i) in MODES" :key="i"
        :class="navbarOptionClass(mode)" 
        @click="() => setMode(mode)">{{ mode.label }}</div>
    </div>
    <div class="settings" :class="{disabled: captureStarted}">
      <div class="settings__item">
        <span>{{numSample}} sample per capture</span>
        <input type="range" :min="SAMPLE_RAMGE[0]" :max="SAMPLE_RAMGE[1]" v-model="numSample" @mouseup="() => updateDeviceParam()" />
      </div>
      <div class="settings__item">
        <span>Cooldown: {{cooldown}}</span>
        <input type="range" :min="COOLDOWN_RANGE[0]" :max="COOLDOWN_RANGE[1]" step="1" v-model="cooldown" @mouseup="() => updateDeviceParam()" />
      </div>
      <div class="settings__item">
        <span>Threshold: {{threshold}}</span>
        <input type="range" :min="THRESHOLD_RANGE[0]" :max="THRESHOLD_RANGE[1]" step="0.01" v-model="threshold" @mouseup="() => updateDeviceParam()" />
      </div>
      
      <div class="strength">
          <span>Current strength: {{ strength }}</span>
          <div class="bar">
            <span :style="getStrengthBarStyle()"></span>
            <span class="mark" :style="{left: `calc(${threshold / THRESHOLD_RANGE[1] } * 100%)`}"></span>
          </div>
        </div>
    </div>
    <div class="view-container">
      <DemoView v-if="mode.index === MODES[0].index"></DemoView>
      <DataCollectView v-else></DataCollectView>
    </div>
    <DevicePanel></DevicePanel>
  </div>
</template>

<script>
import {audioReady, isAudioLoading, loadAudioDatas} from '@/js/audio'
import {updateDeviceParam, cooldown, COOLDOWN_RANGE} from '@/js/device'
import DemoView from './components/DemoView.vue'
import DataCollectView from './components/DataCollectView.vue'
import DevicePanel from './components/DevicePanel.vue'
import {MODES, mode, setMode} from '@/js/mode'
import { strength, captureStarted, numSample, SAMPLE_RAMGE, threshold, THRESHOLD_RANGE } from './js/capture'

export default {
  name: 'drum-web-app',
  components: { DemoView, DataCollectView, DevicePanel },
  setup () {
    console.log(navigator.bluetooth)
    const navbarOptionClass = nMode => {
      return {
        'navbar__option': true,
        'current': nMode.index === mode.index
      }
    }

    const getStrengthBarStyle = () => {
      return {
        width: `calc(${ Math.min(1, strength.value / THRESHOLD_RANGE[1])} * 100%)`,
        backgroundColor: strength.value > threshold.value ? '#42ff75' : 'red'
      }
    }

    return {
      mode, MODES, setMode, navbarOptionClass, loadAudioDatas, audioReady, isAudioLoading,
      captureStarted, numSample, SAMPLE_RAMGE, threshold, THRESHOLD_RANGE, updateDeviceParam,
      strength, getStrengthBarStyle, cooldown, COOLDOWN_RANGE
    }
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

  .settings {
    padding: 10px;
    box-sizing: border-box;
    width: 100%;
    max-width: 500px;
    margin: 0 auto;
    font-size: 17px;
    color: #FFFFFF;

    &.disabled {
      pointer-events: none;
      opacity: 0.4;
    }

    .settings__item {
      display: flex;
      align-items: center;
      justify-content: space-between;
      margin-bottom: 10px;

      input {
        width: 50%;
        height: 5px;
        margin: 0;
      }
    }
    .strength {
      display: flex;
      align-items: baseline;
      justify-content: space-between;

      .bar {
        position: relative;
        display: block;
        height: 4px;
        background-color: #FFFFFF;
        width: 50%;
        overflow: hidden;

        span {
          position: absolute;
          top: 0;
          left: 0;
          height: 100%;
          background-color: red;
        }

        .mark {
          width: 2px;
          background-color: #333333;
        }
      }
    }
  } // settings

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
