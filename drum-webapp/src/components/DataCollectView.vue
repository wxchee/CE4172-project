<template>
  <div class="data-collection-view">
    data collect view

    <GenericButton @click="startReadingIMU">Live IMU</GenericButton>
  </div>
</template>

<script>
import GenericButton from './GenericButton.vue'


export default {
  components: { GenericButton },
  setup() {
    const startReadingIMU = () => {
      // updateServerMode("1")
    }

    return {startReadingIMU}
  }
  
}
</script>

<style lang="scss">
.data-collection-view {

}
</style>
