<template>
  <svg ref="root" class="data-visual" width="100%" height="500">
    {{ sensorData }}
    <!-- <polyline stroke="#FF0000" fill="none" :points="points[0]" />
    <polyline stroke="#FF5500" fill="none" :points="points[1]" />
    <polyline stroke="#FFC100" fill="none" :points="points[2]" />
    <polyline stroke="#0042FF" fill="none" :points="points[3]" />
    <polyline stroke="#0097FF" fill="none" :points="points[4]" />
    <polyline stroke="#00D8FF" fill="none" :points="points[5]" /> -->
    <!-- <text>{{ sensorData }}</text> -->
  </svg>
</template>

<script>
import {computed, onMounted, ref, reactive} from 'vue'
import { mode } from '@/js/mode'

export default {
  props: {
    sensorData: Array,
    default: []
  },
  setup (props) {
    const root = ref(null)
    const box = reactive({width: 0, height: 0})
    const points = computed(() => {
      // const dummy = [
      //   '0.18,-0.14,0.14,-0.02,0.00,0.03', '0.18,-0.13,0.14,-0.02,0.00,0.03', '0.18,0.04,0.09,-0.03,0.01,0.05', '0.19,0.17,0.14,-0.02,0.00,0.03',
      //   '0.18,-0.14,0.14,-0.02,0.00,0.03', '0.18,-0.13,0.14,-0.02,0.00,0.03', '0.18,0.04,0.09,-0.03,0.01,0.05', '0.19,0.17,0.14,-0.02,0.00,0.03',
      //   '0.18,-0.14,0.14,-0.02,0.00,0.03', '0.18,-0.13,0.14,-0.02,0.00,0.03', '0.18,0.04,0.09,-0.03,0.01,0.05', '0.19,0.17,0.14,-0.02,0.00,0.03',
      //   '0.18,-0.14,0.14,-0.02,0.00,0.03', '0.18,-0.13,0.14,-0.02,0.00,0.03', '0.18,0.04,0.09,-0.03,0.01,0.05', '0.19,0.17,0.14,-0.02,0.00,0.03',
      //   '0.18,-0.14,0.14,-0.02,0.00,0.03', '0.18,-0.13,0.14,-0.02,0.00,0.03', '0.18,0.04,0.09,-0.03,0.01,0.05', '0.19,0.17,0.14,-0.02,0.00,0.03'
      // ]
      const pts = ['', '', '', '', '', '']
      props.sensorData.forEach((d, i) => {
        d.split(',').forEach((x, j) => {
          pts[j] += `${i/20 * box.width},${(x + 1)/2 * box.height} `
        })
      })
      return pts
    })

    const onResize = () => {
      if (!root.value) return
      box.width = parseInt(root.value.getBoundingClientRect().width)
      box.height = parseInt(root.value.getBoundingClientRect().height)
      // console.log(width.value)
    }

    const resizeObserver = new ResizeObserver(onResize)
    

    onMounted(() => {
      resizeObserver.observe(root.value)
    })
    

    return {root, points}
  }
}
</script>
<style lang="scss">
.data-visual {

}
</style>