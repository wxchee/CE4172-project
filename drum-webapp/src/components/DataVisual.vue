<template>
  <div class="data-visual">
    <svg ref="root" width="100%" height="100%">
      <path v-for="({color, d}, i) in paths" :key="i" :d="d" fill="none" :stroke="color" stroke-width="1"/>
      
      <line v-bind="middleLineStyle()"/>

      <line class="tick-x" v-for="i in parseInt(numSample)" :key="i" v-bind="tickXAttr(i-1)" />
      <g class="tick-y" v-for="i in 20" :key="i">
        <line v-bind="tickYAttr(i-1)" />
        <text v-bind="tickYTextAttr(i-1)">{{ (-1 + i/20 * 2).toFixed(1) }}</text>
      </g>
      
    </svg>

    <div class="legend">
      <p v-for="(d, i) in ['aX', 'aY', 'aZ', 'gX', 'gY', 'gZ']" :key="i">
        <span :style="{backgroundColor: colorScheme[i]}"></span>
        {{ d }}
      </p>
    </div>
  </div>
  
</template>

<script>
import {computed, onMounted, ref, reactive} from 'vue'
import { numSample } from '@/js/capture'

const d3 = Object.assign({}, require('d3-scale'), require('d3-shape'))
const MARGIN = {top: 30, right: 5, bottom: 10, left: 40}
const colorScheme = ['#0352fc', '#3d7bff', '#7da7ff', '#ff5e00', '#ff8842', '#ffaf80']

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
        x2: MARGIN.left * 0.2,
        y2: MARGIN.top + height - index / 20 * height
        
      }
    }

    const tickYTextAttr = index => {
      const height = box.height - MARGIN.top - MARGIN.bottom
      return {
        fill: '#FFFFFF',
        'font-size': 12,
        'text-anchor': 'end',
        transform: `translate(${MARGIN.left - 10}, ${MARGIN.top + height - index / 20 * height + 3})`
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
    

    return {root, paths, box, tickXAttr, tickYAttr, tickYTextAttr, middleLineStyle, numSample, colorScheme}
  }
}
</script>
<style lang="scss">
.data-visual {
  position: relative;
  .legend {
    position: absolute;
    right: 5px;
    top: 5px;
    
    p {
      display: inline;
      padding-right: 10px;
      font-size: 14px;
      span {
        display: inline-block;
        width: 10px;
        height: 10px;
      }
    }
    
  }
}
</style>