<template>
  <div class="data-collection-view">
    <div class="settings">
      <div class="settings__item">
        <span>{{numSample}} sample per capture</span>
        <input type="range" :min="SAMPLE_RAMGE[0]" :max="SAMPLE_RAMGE[1]" v-model="numSample" />
      </div>
      <div class="settings__item">
        <span>threshold: {{threshold}}</span>
        <input type="range" :min="THRESHOLD_RANGE[0]" :max="THRESHOLD_RANGE[1]" step="0.01" v-model="threshold" />
      </div>
      <div class="strength">
          <span>Current strength: {{ strength }}</span>
          <div class="bar">
            <span :style="{width: `calc(${strength} / 0.8 * 100%)`}"></span>
          </div>
        </div>
    </div>
    <div class="captures">
      <div class="captures__header" :class="{disabled: isCaptureDisabled()}">
        <h4>Captured: {{ capturedCount }}</h4>
        <GenericButton @click="startCapture">{{captureStarted ? 'Pause' : 'Start'}}</GenericButton>
        <GenericButton @click="resetCapture">Clear</GenericButton>
        <a class="downloadCSV" download="capture.csv" ref="download" :class="{disabled: !capturedBuffer[0] || !capturedBuffer[0].length}" @click="saveCapture">Save as .csv</a>
      </div>
      <div class="captures__records">
        <div class="captures__records__wrapper">
          <div :class="captureItemClass(i)" v-for="(capArray, i) in capturedBuffer" :key="i" @click="() => selectedCapIndex = i">
            Capture {{ i+1 }}
            <span class="delete" @click="() => removeCapturedItem(i)">&#10005;</span>
          </div>
          <div class="new-capture-indicator" :style="getIndicatorStyle()"></div>
        </div>
        <DataVisual :sensorData="capturedBuffer[selectedCapIndex] || []"></DataVisual>
      </div>
      
    </div>
  </div>
</template>

<script>
import { ref, computed, onBeforeUnmount } from 'vue'
import DataVisual from './DataVisual.vue'
import GenericButton from './GenericButton.vue'
import { getConnectedDevices } from '@/js/device'
import {
  SAMPLE_RAMGE, THRESHOLD_RANGE, threshold, capturedCount, captureStarted, numSample, buffer, capturedBuffer,
  selectedCapIndex, startCapture, pauseCapture, resetCapture, removeCapturedItem, isCaptureDisabled
} from '@/js/capture'

export default {
  components: { GenericButton, DataVisual },
  setup() {
    const strength = computed(() => {
      return ((Math.abs(buffer.aX) + Math.abs(buffer.aY) + Math.abs(buffer.aZ) +
            Math.abs(buffer.gX) + Math.abs(buffer.gY) + Math.abs(buffer.gZ)) / 6).toFixed(2)
    })
    
    const captureItemClass = capturedIndex => {
      return {
        'captures__records__item': true,
        'selected': capturedIndex === selectedCapIndex.value
      }
    }

    const getIndicatorStyle = () => {
      const ratio = captureStarted.value ? Math.min(1, strength.value / threshold.value) : 0
      return { width: `calc(${ratio} * 100%)` }
    }

    const download = ref(null)
    const saveCapture = () => {
      pauseCapture()
      const csvContent = 'aX,aY,aZ,gX,gY,gZ\r\n' + capturedBuffer.value.map(bf => bf.join("\r\n")).join("\r\n\n")

      const blob = new Blob([csvContent], {type: 'text/csv;charset=utf-8;'})
      
      download.value.href = URL.createObjectURL(blob)    
      // console.log(csvContent)
      // const encodeUri = encodeURI(csvContent)
      // window.open(encodeUri)
    }

    onBeforeUnmount(() => {
      captureStarted.value = false
    })

    return {
      numSample, threshold, SAMPLE_RAMGE, THRESHOLD_RANGE,
      capturedCount, capturedBuffer, startCapture, resetCapture, captureStarted,
      selectedCapIndex, removeCapturedItem, getConnectedDevices, isCaptureDisabled,
      strength, download, captureItemClass, getIndicatorStyle, saveCapture
    }
  }
  
}
</script>

<style lang="scss">
.data-collection-view {
  color: #FFFFFF;
  font-size: 18px;
  display: flex;
  flex-direction: column;

  .settings {
    padding: 10px;
    box-sizing: border-box;
    width: 100%;
    max-width: 500px;
    margin: 0 auto;

    .settings__item {
      display: flex;
      justify-content: space-between;
      margin-bottom: 20px;

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
      }
    }
  } // settings

  .captures {
    height: 100%;
    overflow-y: hidden;
    display: flex;
    flex-direction: column;

    .captures__header {
      padding: 10px;
      box-sizing: border-box;

      &.disabled,
      .disabled {
        opacity: 0.4;
        cursor: default;
        pointer-events: none;
      }

      & > * {
        display: inline-block;
        margin: 0;
        margin-right: 10px;
      }

      a.downloadCSV {
        color: #d1e1ff;
        text-decoration: none;
        border-bottom: 2px dotted #d1e1ff;
        cursor: pointer;
        opacity: 0.8;

        &:hover {
          opacity: 1;
          border-bottom: 2px solid #d1e1ff;
        }
      }
    }
    .captures__records {
      display: grid;
      grid-template-columns: 1fr 1fr;
      height: 100%;
      overflow-y: hidden;

      .captures__records__wrapper {
          height: 100%;
          overflow-y: hidden;
          background-color: #ffffff;
          overflow-y: scroll;

          & > * {
            background-color: #e8e8e8;

            &:nth-child(2n) {
              background-color: #d5d5d5;
            }
          }

        .captures__records__item {
            position: relative;
            color: #333333;
            user-select: none;
            padding: 10px;
            cursor: pointer;
            box-sizing: border-box;
          
            &.selected {
              background-color: #b5b5b5;
              cursor: default;
            }

            .delete {
              display: none;
              position: absolute;
              right: 10px;
              top: 50%;
              transform: translateY(-50%);
              transition: transform 0.2s;
              cursor: pointer;
            }

            &.selected > .delete,
            &:hover > .delete {
              display: block;

              &:hover {
                transform: translateY(-50%) scale(1.2);
              }
            }

            &:hover {
              background-color: #cbcbcb;
            }
          }

          .new-capture-indicator {
            color: #333333;
            box-sizing: border-box;
            pointer-events: none;
            overflow-x: hidden;
            height: 40px;
          }
      }

      .data-visual {
        width: 100%;
        height: 100%;
        background-color: yellow;
      }
    }
  }
}
</style>
