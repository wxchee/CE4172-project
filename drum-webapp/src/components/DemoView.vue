<template>
  <div class="demo-view" >
    <DrumVisual></DrumVisual>
    <div class="drum-type">
      <RadioButton v-model="drumType" :options="DRUM_TYPES"></RadioButton>
    </div>
    <div class="option">
      <div class="option-content">
      </div>
    </div>
  </div>
</template>

<script>
import RadioButton from '@/components/RadioButton.vue'
import DrumVisual from '@/components/DrumVisual.vue'

import {drumType, DRUM_TYPES} from '@/js/drum'

export default {
  components: {DrumVisual, RadioButton},
  setup () {
    
    return {drumType, DRUM_TYPES}
  }
}
</script>

<style lang="scss">
.demo-view {
  display: flex;
  flex-direction: column;

  .radio-button {
    position: fixed;
    z-index: 0;
    right: 20px;
    top: 20px;
    text-transform: capitalize;
  }

  .drum-visual {
    height: calc(100% - 200px);
    overflow: hidden;
  }
  
  

  .option {
    height: 200px;

    .option-content {
      margin: 0 auto;
      max-width: 600px;
      background-color: #666666;
    }
  }
}
</style>
