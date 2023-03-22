<template>
  <div class="data-collection-view">
    <div class="captures">
      <div class="captures__header">
        <h4>Captured: {{ capturedBuffer.length }}</h4>
        <GenericButton :class="{disabled: !hasAvailableDevices()}" @click="startCapture">{{captureStarted ? 'Pause' : 'Start'}}</GenericButton>
        <GenericButton :class="{disabled: !capturedBuffer.length}" @click="resetCapture">Clear</GenericButton>
        <a class="downloadCSV" :class="{disabled: !capturedBuffer.length}" download="capture.csv" ref="download" @click="saveCapture">Save as .csv</a>
      </div>
      <div class="captures__records">
        <div class="captures__records__wrapper">
          <div :class="captureItemClass(i)" v-for="({t}, i) in capturedBuffer" :key="i" @click="() => selectCapturedBuffer(i)">
            captured at: {{ t }}
            <span class="delete" @click="e => removeCapturedItem(i, e)">&#10005;</span>
          </div>
          <div class="new-capture-indicator" :style="getIndicatorStyle()"></div>
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
import { ref, onBeforeUnmount } from 'vue'
import DataVisual from './DataVisual.vue'
import GenericButton from './GenericButton.vue'
import {
  threshold, captureStarted, capturedBuffer, captureSnaphot,
  selectedCapIndex, startCapture, pauseCapture, resetCapture, removeCapturedItem, hasAvailableDevices,
  strength
} from '@/js/capture'
import { updateDeviceParam} from '@/js/device'
export default {
  components: { GenericButton, DataVisual },
  setup() {
    const captureItemClass = capturedIndex => {
      return {
        'captures__records__item': true,
        'selected': capturedIndex === selectedCapIndex.value,
        'disabled': captureStarted.value
      }
    }

    const getIndicatorStyle = () => {
      const ratio = captureStarted.value ? Math.min(1, strength.value / threshold.value) : 0
      return { width: `calc(${ratio} * 100%)` }
    }

    const selectCapturedBuffer = i => {
      selectedCapIndex.value = selectedCapIndex.value === i ? -1 : i
    }

    const getSensorData = () => {
      if (selectedCapIndex.value >= 0) return capturedBuffer.value[selectedCapIndex.value]
      else if (hasAvailableDevices()) return captureSnaphot

      return null
    }

    const getCurrentView = () => {
      if (selectedCapIndex.value >= 0) return 'captured at ' + capturedBuffer.value[selectedCapIndex.value].t
      else if (hasAvailableDevices()) return 'live'

      return ''
    }



    const download = ref(null)

    const saveCapture = () => {
      pauseCapture()
      const csvContent = 'aX,aY,aZ,gX,gY,gZ\r\n' + capturedBuffer.value.map(bf => {
        // return bf.val.map(bfStr => normaliseData(bfStr)).join('\r\n')
        return bf.val.join("\r\n")
      }).join("\r\n\n")

      const blob = new Blob([csvContent], {type: 'text/csv;charset=utf-8;'})
      
      download.value.href = URL.createObjectURL(blob)
    }

    onBeforeUnmount(() => {
      captureStarted.value = false
    })

    return {
      capturedBuffer, startCapture, resetCapture, captureStarted,
      selectedCapIndex, removeCapturedItem, hasAvailableDevices,
      download, captureItemClass, getIndicatorStyle, selectCapturedBuffer,
      saveCapture, getSensorData, getCurrentView, updateDeviceParam
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

  // .settings {
  //   padding: 10px;
  //   box-sizing: border-box;
  //   width: 100%;
  //   max-width: 500px;
  //   margin: 0 auto;
  //   font-size: 17px;

  //   &.disabled {
  //     pointer-events: none;
  //     opacity: 0.4;
  //   }

  //   .settings__item {
  //     display: flex;
  //     align-items: center;
  //     justify-content: space-between;
  //     margin-bottom: 10px;

  //     input {
  //       width: 50%;
  //       height: 5px;
  //       margin: 0;
  //     }
  //   }
  //   .strength {
  //     display: flex;
  //     align-items: baseline;
  //     justify-content: space-between;

  //     .bar {
  //       position: relative;
  //       display: block;
  //       height: 4px;
  //       background-color: #FFFFFF;
  //       width: 50%;
  //       overflow: hidden;

  //       span {
  //         position: absolute;
  //         top: 0;
  //         left: 0;
  //         height: 100%;
  //         background-color: red;
  //       }

  //       .mark {
  //         width: 2px;
  //         background-color: #333333;
  //       }
  //     }
  //   }
  // } // settings

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
