<template>
  <svg ref="root" class="data-visual" width="100%" height="100%">
    <path v-for="({color, d}, i) in paths" :key="i" :d="d" fill="none" :stroke="color" stroke-width="1"/>
    
    <line v-bind="middleLineStyle()"/>

    <line class="tick-x" v-for="i in parseInt(numSample)" :key="i" v-bind="tickXAttr(i-1)" />
    <line class="tick-y" v-for="i in 20" :key="i" v-bind="tickYAttr(i-1)" />
    
  </svg>
</template>

<script>
import {computed, onMounted, ref, reactive} from 'vue'
import { numSample } from '@/js/capture'

const d3 = Object.assign({}, require('d3-scale'), require('d3-shape'))

const MARGIN = {top: 50, right: 5, bottom: 10, left: 10}
export default {
  props: {
    sensorData: Object,
    default: null
  },
  setup (props) {
    const root = ref(null)
    const box = reactive({width: 0, height: 0})

    const paths = computed(() => {
      if (!props.sensorData) return []
      const x = d3.scaleLinear().domain([0, numSample.value - 1]).range([MARGIN.left, box.width - MARGIN.right])  
      const y = d3.scaleLinear().domain([-2, 2]).range([box.height - MARGIN.bottom, MARGIN.top])
      const line = d3.line().x(d => x(d.x)).y(d => y(d.y))

      const colorScheme = ['#0352fc', '#3d7bff', '#7da7ff', '#ff5e00', '#ff8842', '#ffaf80']
      const ds = [[], [], [], [], [], []]

      props.sensorData.val.forEach((dAll, i) => {
        dAll.split(',').forEach((d, j) => {
          ds[j].push({x: i, y: d})
        })
      })

      return ds.map((d, i) => ({ color: colorScheme[i], d: line(d)}))
    })

    const onResize = () => {
      if (!root.value) return
      box.width = parseInt(root.value.getBoundingClientRect().width)
      box.height = parseInt(root.value.getBoundingClientRect().height)
    }

    const tickXAttr = index => {
      const width = box.width - MARGIN.right - MARGIN.left
      return {
        stroke: '#FFFFFF',
        x1: MARGIN.left + index / (numSample.value - 1) * width,
        y1: box.height,
        x2: MARGIN.left + index / (numSample.value - 1) * width,
        y2: box.height - MARGIN.bottom
      }
    }
    const tickYAttr = index => {
      const height = box.height - MARGIN.top - MARGIN.bottom
      return {
        stroke: '#FFFFFF',
        x1: 0,
        y1: MARGIN.top + height - index / 20 * height,
        x2: MARGIN.left,
        y2: MARGIN.top + height - index / 20 * height
        
      }
    }

    const middleLineStyle = () => {
      const height = MARGIN.top + (box.height - MARGIN.top - MARGIN.bottom) / 2
      return {
        stroke: '#FFFFFF',
        strokeWidth: 1.5,
        x1: MARGIN.left,
        y1: height,
        x2: box.width - MARGIN.right,
        y2: height
      }
    }

    const resizeObserver = new ResizeObserver(onResize)
    

    onMounted(() => {
      resizeObserver.observe(root.value)
      onResize()
    })
    

    return {root, paths, box, tickXAttr, tickYAttr, middleLineStyle, numSample}
  }
}
</script>
<style lang="scss">
.data-visual {
  background-color: black;
}
</style>