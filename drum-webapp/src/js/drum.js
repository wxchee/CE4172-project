import {reactive, ref} from 'vue'
import { play } from './audio'
const DRUM_TYPES = ['acoustic', 'analog', 'latin']
const highlights = reactive({0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0, 6: 0, 7: 0})
const drumType = ref(DRUM_TYPES[0])
const hCountDown = []

const onDrumHit = drumCode => {
  play(drumType.value, drumCode)
  setHightlight(drumCode)
}

const setHightlight = i => {
  highlights[i] = 1

  if (hCountDown[i]) {
    cancelAnimationFrame(hCountDown[i])
    hCountDown[i] = null
  }

  hCountDown[i] = () => {
    highlights[i] -= 0.1

    if (highlights[i] <= 0) {
      cancelAnimationFrame(hCountDown[i])
      hCountDown[i] = null
      highlights[i] = 0
    } else requestAnimationFrame(hCountDown[i])
  }

  requestAnimationFrame(hCountDown[i])
}

export {drumType, DRUM_TYPES, highlights, setHightlight, onDrumHit}
