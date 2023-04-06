<template>
  <div class="data-visual">
    <svg ref="root" width="100%" height="100%">
      <template v-if="sensorData && sensorData.val[0] && sensorData.val[0].length === 1">
        <text>test</text>
      </template>
      <template v-else>
        <path v-for="({color, d}, i) in paths" :key="i" :d="d" fill="none" :stroke="color" stroke-width="1"/>
      </template>
      
      
      <line v-bind="middleLineStyle()"/>
      <g class="tick-x">
        <line v-for="(tx, i) in parseInt(numSample)" :key="i" v-bind="tickXAttr(tx-1)" />
        <text v-for="(tx, i) in tickX" :key="i" v-bind="tickXTextAttr(tx-1)">{{ tx }}</text>
      </g>
      
      <g class="tick-y" v-for="(ty, i) in tickY" :key="i">
        <line v-bind="tickYAttr(i)" />
        <text v-bind="tickYTextAttr(i)">{{ ty }}</text>
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
const MARGIN = {top: 50, right: 15, bottom: 40, left: 40}
const colorScheme = ['#0352fc', '#3d7bff', '#7da7ff', '#ff5e00', '#ff8842', '#ffaf80', '#9602d1', '#c902e3','#fc03f4']

const range = (start, stop, step, fp=0) => Array.from({length: Math.ceil((stop - start) / step) + 1}, (x, i) => (start + i * step).toFixed(fp))



export default {
  props: {
    sensorData: Object,
    default: null
  },
  setup (props) {
    const root = ref(null)
    const box = reactive({width: 0, height: 0})

    const tickX = computed(() => {
      const maxTick = 12
      let rawTickX = range(1, numSample.value, Math.ceil(numSample.value / maxTick))
      
      if (numSample.value > maxTick) {
        if (rawTickX[rawTickX.length - 1] > numSample.value) rawTickX.pop()

        const diffLast = numSample.value - rawTickX[rawTickX.length - 1]

        if (diffLast && diffLast < 2) {
          rawTickX.splice(rawTickX.length - 1, 1, numSample.value)
        } else {
          rawTickX.push(numSample.value)
        }
      }
      
      return rawTickX
    })

    const tickY = range(0, 1, 0.1, 1)

    
    const paths = computed(() => {
      if (!props.sensorData) return []
      
      const x = d3.scaleLinear().domain([0, numSample.value - 1]).range([MARGIN.left, box.width - MARGIN.right])  
      const y = d3.scaleLinear().domain([tickY[0], tickY[tickY.length - 1]]).range([box.height - MARGIN.bottom, MARGIN.top])
      const line = d3.line().x(d => x(d.x)).y(d => y(d.y))

      const ds = [[], [], [], [], [], []]

      props.sensorData.val.forEach((dAll, i) => {
        dAll.forEach((d, j) => {
          ds[j].push({x: i, y: d})
        })
      })

      return ds.filter(d => line(d) && line(d).indexOf("NaN") === -1).map((d, i) => {
        return { color: colorScheme[i], d: line(d)}
      })
    })

    const onResize = () => {
      if (!root.value) return
      box.width = parseInt(root.value.getBoundingClientRect().width)
      box.height = parseInt(root.value.getBoundingClientRect().height)
    }
    
    const tickXAttr = index => {
      const width = box.width - MARGIN.right - MARGIN.left
      const visibleTick = tickX.value.includes((index + 1).toString())
      return {
        stroke: '#FFFFFF',
        opacity: visibleTick ? 0.3 : 0.1,
        x1: MARGIN.left + index / (numSample.value - 1) * width,
        y1: MARGIN.top,
        x2: MARGIN.left + index / (numSample.value - 1) * width,
        y2: box.height - 24
      }
    }

    const tickXTextAttr = index => {
      const width = box.width - MARGIN.right - MARGIN.left
      return {
        fill: '#FFFFFF',
        'font-size': 12,
        'text-anchor': 'middle',
        transform: `translate(${MARGIN.left + index / (numSample.value - 1) * width}, ${box.height - 6})`
      }
    }

    const tickYAttr = index => {
      const height = box.height - MARGIN.top - MARGIN.bottom
      return {
        stroke: '#FFFFFF',
        opacity: 0.1,
        x1: MARGIN.left - 5,
        y1: MARGIN.top + height - index / (tickY.length - 1) * height,
        x2: box.width - MARGIN.right,
        y2: MARGIN.top + height - index / (tickY.length - 1) * height
        
      }
    }

    const tickYTextAttr = index => {
      const height = box.height - MARGIN.top - MARGIN.bottom
      return {
        fill: '#FFFFFF',
        'font-size': 12,
        'text-anchor': 'end',
        transform: `translate(${MARGIN.left - 10}, ${MARGIN.top + height - index / (tickY.length - 1) * height + 5})`
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
    

    return {root, paths, box, tickX, tickXAttr, tickXTextAttr, tickY, tickYAttr, tickYTextAttr, middleLineStyle, numSample, colorScheme}
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