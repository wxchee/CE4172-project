<template>
  <div class="settings">
      <div v-if="showNumSample" class="settings__item">
        <span>{{numSample}} sample per capture</span>
        <input type="range" :min="SAMPLE_RAMGE[0]" :max="SAMPLE_RAMGE[1]" v-model="numSample" @mouseup="() => updateDeviceParam()" />
      </div>
      <div v-if="showTestResponseTime" class="settings__item">
        <span>Test response time: {{testResponseTime}}</span>
        <input type="range" :min="TEST_RESPONSE_TIME_RANGE[0]" :max="TEST_RESPONSE_TIME_RANGE[1]" step="1.0" v-model="testResponseTime" @mouseup="() => updateDeviceParam()" />
      </div>
      <div v-if="showCooldown" class="settings__item">
        <span>Cooldown: {{cooldown}}</span>
        <input type="range" :min="COOLDOWN_RANGE[0]" :max="COOLDOWN_RANGE[1]" step="1.0" v-model="cooldown" @mouseup="() => updateDeviceParam()" />
      </div>
      <div v-if="showThreshold" class="settings__item">
        <span>Threshold: {{threshold}}</span>
        <input type="range" :min="THRESHOLD_RANGE[0]" :max="THRESHOLD_RANGE[1]" step="0.01" v-model="threshold" @mouseup="() => updateDeviceParam()" />
      </div>
      
      <div v-if="showMagnitude" class="magnitude">
          <span>Magnitude: {{ magnitude }}</span>
          <div class="bar">
            <span :style="getMagnitudeBarStyle()"></span>
            <span class="mark" :style="{left: `calc(${threshold / THRESHOLD_RANGE[1] } * 100%)`}"></span>
          </div>
        </div>
    </div>
</template>

<script>
import {
  threshold, THRESHOLD_RANGE, magnitude,
  numSample, SAMPLE_RAMGE
} from '@/js/capture'
import { cooldown, COOLDOWN_RANGE, TEST_RESPONSE_TIME_RANGE, testResponseTime, updateDeviceParam } from '@/js/device'
export default {
  props: {
    showThreshold: {
      type: Boolean,
      default: true
    },
    showMagnitude: {
      type: Boolean,
      default: true
    },
    showTestResponseTime: {
      type: Boolean,
      default: true
    },
    showCooldown: {
      type: Boolean,
      default: true
    },
    showNumSample: {
      type: Boolean,
      default: true
    }
  },
  setup () {
    const getMagnitudeBarStyle = () => {
      return {
        width: `calc(${ Math.min(1, magnitude.value / THRESHOLD_RANGE[1])} * 100%)`,
        backgroundColor: magnitude.value > threshold.value ? '#42ff75' : 'red'
      }
    }

    return {
      getMagnitudeBarStyle,
      threshold, THRESHOLD_RANGE, cooldown, COOLDOWN_RANGE,
      numSample, SAMPLE_RAMGE, magnitude,
      testResponseTime, TEST_RESPONSE_TIME_RANGE,
      updateDeviceParam
    }
  }
}
</script>

<style lang="scss">
.settings {
    padding: 10px;
    box-sizing: border-box;
    width: 100%;
    max-width: 600px;
    margin: 0 auto;
    // font-size: 17px;
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
        width: 60%;
        // height: 5px;
        margin: 0;
        cursor: pointer;
        opacity: 0.7;

        &:hover {
          opacity: 1;
        }
      }
    }
    .magnitude {
      display: flex;
      align-items: baseline;
      justify-content: space-between;
      color: #FFFFFF;
      & > span {
        opacity: 0.5;
      }

      .bar {
        position: relative;
        display: block;
        height: 4px;
        background-color: rgba(#FFFFFF, 0.5);
        width: 60%;
        overflow: hidden;

        span {
          position: absolute;
          top: 0;
          left: 0;
          height: 100%;
          background-color: red;
          opacity: 1;
        }

        .mark {
          width: 2px;
          background-color: #333333;
        }
      }
    }
  }
</style>