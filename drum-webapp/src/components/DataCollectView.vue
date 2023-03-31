<template>
  <div class="data-collection-view">
    <DrumSettings :showCooldown="false" :showTestResponseTime="false"></DrumSettings>
    <div class="captures">
      <div class="captures__header">
        <h4>Captured: {{ captureListView === LIST_VIEWS[0] ? mainCapBF.length : filteredCapBF.length }}</h4>
        <GenericButton :class="{disabled: !hasAvailableDevices()}" @click="startCapture">{{captureStarted ? 'Pause' : 'Start'}}</GenericButton>
        <GenericButton :class="{disabled: !capturedBuffer.length}" @click="resetCapture">Clear</GenericButton>
        <a v-if="captureListView === LIST_VIEWS[0]" class="downloadCSV" :class="{disabled: !mainCapBF.length}" download="capture.csv" ref="download" @click="saveCapture">Save gesture as .csv</a>
        <a v-else class="updateUnknownCSV" :class="{disabled: !filteredCapBF.length}" download="unknown.csv" ref="downloadFilterCapture" @click="saveFilteredCapture">Save extracted gesture as .csv</a>
      </div>
      <div class="captures__records">
        <div class="captures__records__wrapper">
          <RadioButton class="capture__records__view-toggle" v-model="captureListView" :options="LIST_VIEWS"></RadioButton>
          <div class="capture__records__list" v-if="captureListView === LIST_VIEWS[0]">
            <div :class="captureItemClass(t)" v-for="({t}, i) in mainCapBF" :key="i" @click="() => selectCapturedBuffer(t)">
              captured at: <span class="time">{{ t }}</span>
              <span class="delete" @click="e => removeCapturedItem(t, e)">&#10005;</span>
              <span class="filter-capture" @click="e => setUnknownCapture(t, e)">&#8594;</span>
            </div>
            <div class="new-capture-indicator" :style="getIndicatorStyle()"></div>
          </div>
          <div v-else class="capture__records__list">
            <div :class="captureItemClass(t)" v-for="({t}, i) in filteredCapBF" :key="i" @click="() => selectCapturedBuffer(t)">
              captured at: <span class="time">{{ t }}</span>
              <span class="delete" @click="e => removeCapturedItem(t, e)">&#10005;</span>
              <span class="filter-capture" @click="e => setUnknownCapture(t, e)">&#8592;</span>
            </div>
            <div class="new-capture-indicator" :style="getIndicatorStyle()"></div>
          </div>
          
        </div>
        <div class="capture-visual">
          <h4>View: <span>{{getCurrentView()}}</span></h4>
          <DataVisual :sensorData="getSensorData()"></DataVisual>
        </div>
      </div>
      
    </div>
  </div>
</template>

<script>
import { ref, onBeforeUnmount, computed } from 'vue'
import DataVisual from './DataVisual.vue'
import GenericButton from './GenericButton.vue'
import RadioButton from './RadioButton.vue'
import DrumSettings from './DrumSettings.vue'

import {
  captureStarted, capturedBuffer, captureSnaphot,
  selectedCapTime, startCapture, pauseCapture, resetCapture, removeCapturedItem, hasAvailableDevices,
  filterCaptureTRef, LIST_VIEWS, captureListView,
  threshold, magnitude,
} from '@/js/capture'


export default {
  components: { DrumSettings, GenericButton, RadioButton, DataVisual },
  setup() {
    const mainCapBF = computed(() => capturedBuffer.value.filter(bf => !filterCaptureTRef.value[bf.t]))
    const filteredCapBF = computed(() => capturedBuffer.value.filter(bf => filterCaptureTRef.value[bf.t]))
    
    const captureItemClass = captureTime => {
      return {
        'captures__records__item': true,
        'selected': captureTime === selectedCapTime.value,
        'disabled': captureStarted.value,
        'isUnknown': filterCaptureTRef.value[captureTime]
      }
    }

    const setUnknownCapture = (i, e) => {
      filterCaptureTRef.value[i] = !filterCaptureTRef.value[i]
      e.stopPropagation()
    }

    const getIndicatorStyle = () => {
      const ratio = captureStarted.value ? Math.min(1, magnitude.value / threshold.value) : 0
      return { width: `calc(${ratio} * 100%)` }
    }

    const selectCapturedBuffer = t => {
      selectedCapTime.value = selectedCapTime.value === t ? -1 : t
    }

    const getSensorData = () => {
      if (selectedCapTime.value >= 0) return capturedBuffer.value.filter(bf => bf.t === selectedCapTime.value)[0]
      else if (hasAvailableDevices()) return captureSnaphot

      return null
    }

    const getCurrentView = () => {
      if (selectedCapTime.value >= 0) return 'captured at ' + selectedCapTime.value
      else if (hasAvailableDevices()) return 'live'

      return ''
    }

    const download = ref(null)

    const saveCapture = () => {
      pauseCapture()
      const csvContent = 'aX,aY,aZ,gX,gY,gZ\r\n' + mainCapBF.value.map(bf => {
      // const csvContent = 'aX,aY,aZ,gX,gY,gZ,mX,mY,mZ\r\n' + capturedBuffer.value.map(bf => {
        // return bf.val.map(bfStr => normaliseData(bfStr)).join('\r\n')
        return bf.val.join("\r\n")
      }).join("\r\n\n")

      const blob = new Blob([csvContent], {type: 'text/csv;charset=utf-8;'})
      
      download.value.href = URL.createObjectURL(blob)


    }

    const downloadFilterCapture = ref(null)
    
    const saveFilteredCapture = () => {
      const csvContent = 'aX,aY,aZ,gX,gY,gZ\r\n' + filteredCapBF.value.map(bf => {
        return bf.val.join("\r\n")
      }).join("\r\n\n")

      const blob = new Blob([csvContent], {type: 'text/csv;charset=utf-8;'})
      
      downloadFilterCapture.value.href = URL.createObjectURL(blob)
    }

    onBeforeUnmount(() => {
      captureStarted.value = false
    })

    return {
      mainCapBF, filteredCapBF, capturedBuffer, startCapture, resetCapture, captureStarted,
      selectedCapTime, removeCapturedItem, hasAvailableDevices,
      download, downloadFilterCapture, captureItemClass, setUnknownCapture, getIndicatorStyle, selectCapturedBuffer,
      saveCapture, saveFilteredCapture, getSensorData, getCurrentView,
      threshold, magnitude,
      captureListView, LIST_VIEWS, filterCaptureTRef
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

      a {
        color: #d1e1ff;
        text-decoration: none;
        border-bottom: 2px dotted #d1e1ff;
        cursor: pointer;
        opacity: 0.9;

        &.disabled {
          opacity: 0.4;
        }

        &:hover {
          opacity: 1;
          border-bottom: 2px solid #d1e1ff;
        }
      }
    }
    .captures__records {
      display: flex;
      height: 100%;
      overflow-y: hidden;

      .captures__records__wrapper {
          height: 100%;
          min-width: 300px;
          flex: 1;
          overflow-y: hidden;
          background-color: #ffffff;
          overflow-y: auto;

          .capture__records__view-toggle {
            position: relative;
            // display: grid;
            // grid-template-columns: 1fr 1fr;
            // width: 200px;
            .radio-button__option {
              background-color: transparent;
              border: none;
              color: #333333;
              opacity: 0.5;
              border-radius: 0;
              padding: 5px 10px 3px;

              &.checked {
                opacity: 1;
                border-bottom: 2px solid #333333;
                
              }

              &:hover {
                opacity: 0.8;
              }
            }
          }

          .capture__records__list {
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
              &.selected,
              &.selected:hover {
                background-color: #111111;
                color: #FFFFFF;

              }

              &.disabled {
                // cursor: not-allowed;
                pointer-events: none;
                opacity: 0.5;
              }

              .time {
                font-size: 13px;
              }

              .delete,
              .filter-capture {
                position: absolute;
                right: 10px;
                top: 50%;
                transform: translateY(-50%);
                transition: transform 0.2s;
                cursor: pointer;

                &:hover {
                  transform: translateY(-50%) scale(1.2);
                }
              }

              .filter-capture:active {
                color: red;
              }

              &:hover {
                background-color: #cbcbcb;
                
              }

              &.isUnknown .filter-capture {
                color: red;

                &:hover {
                  color: grey;
                }
              }

              .filter-capture {
                right: 40px;
                line-height: 1;
                color: grey;
              }
            }

            .new-capture-indicator {
              color: #333333;
              box-sizing: border-box;
              pointer-events: none;
              overflow-x: hidden;
              height: 40px;
            }
          } // capture__records__list

          
      } // captures__records__wrapper

      .capture-visual {
        position: relative;
        flex: 2;
        height: 100%;
        background-color: #111111;

        h4 {
          position: absolute;
          top: 0;
          left: 0;
          margin: 0;
          padding: 10px 20px 0;
          font-size: 22px;
          
          span {
            font-size: 17px;
            font-weight: normal;
          }
        }

        .data-visual {
          position: absolute;
          width: 100%;
          height: 100%;
          top: 0;
          left: 0;
        }
        
      }
    }
  }
}
</style>
