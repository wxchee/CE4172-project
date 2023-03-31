<template>
  <div class="demo-view" >
    <DrumSettings :showMagnitude="false" :showNumSample="false"></DrumSettings>
    <ToggleButton :labels="DEMO_VIEWS" v-model="demoMode"></ToggleButton>
    <DrumVisual></DrumVisual>
    <div class="option">
      <div class="option-content">
        <div class="drum-type">
          <RadioButton v-model="drumType" :options="DRUM_TYPES"></RadioButton>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { ref } from 'vue'
import RadioButton from '@/components/RadioButton.vue'
import DrumVisual from '@/components/DrumVisual.vue'
import DrumSettings from './DrumSettings.vue'
import ToggleButton from './ToggleButton.vue'
import {drumType, DRUM_TYPES} from '@/js/drum'

const DEMO_VIEWS = ['Test response', 'Inference mode']
const demoMode = ref(DEMO_VIEWS[0])

export {demoMode, DEMO_VIEWS}

export default {
  components: { DrumSettings, DrumVisual, RadioButton, ToggleButton },
  setup () {
    
    return {drumType, DRUM_TYPES, demoMode, DEMO_VIEWS}
  }
}
</script>

<style lang="scss">
.demo-view {
  display: flex;
  flex-direction: column;

  .drum-visual {
    height: calc(100% - 200px);
    overflow: hidden;
  }
  
  

  .option {
    height: 200px;
    width: 100%;

    .option-content {
      margin: 0 auto;
      max-width: 600px;

      .radio-button {
        display: grid;
        grid-template-columns: 1fr 1fr 1fr;
        text-transform: capitalize;
        background-color: unset;
        width: 100%;
        gap: 10px;

        .radio-button__option {
          text-align: center;
          padding: 15px 15px;
          font-size: 25px;
          line-height: 1;
          background-color: rgba(#FFFFFF, 0.1);
          transition: background-color 0.2s;

          &.checked,
          &.checked:hover {
            background-color: rgba(#FFFFFF, 0.6);
            color: #333333;
          }

          &:hover {
            background-color: rgba(#FFFFFF, 0.3);
          }
        }
      }
    }
  }
}
</style>
