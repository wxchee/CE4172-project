<template>
  <div class="data-panel">
    <GenericButton @click="onStartCollectData">Start data collection</GenericButton>
  </div>
</template>

<script>
import { broadcastTrigger, getConnectedDevices } from '@/js/device'
import GenericButton from '@/components/GenericButton.vue'
import { reactive } from 'vue'

const trigger = reactive({
  ready: false,
  lastSentMS: 0
})

export {trigger}

export default {
  components: {GenericButton},
  setup () {
    const onStartCollectData = () => {

      if (!getConnectedDevices().length) {
        console.log('no connected device!')
      } else {
        let prevMS = Date.now()
        setInterval(() => {
          let curMS = Date.now()
          // console.log(curMS, prevMS)
          if (curMS - prevMS >= 500) {
            if (trigger.ready) {
              trigger.ready = !broadcastTrigger('t')
            } else {
              console.log('nano late to reply, wait for next interval')
            }
            prevMS = curMS
          }
          
        }, 10)
      }
      
      
    }

    return {onStartCollectData}
  }
}
</script>