import {reactive, ref} from 'vue'
import { play } from './audio'

const DRUM_TYPES = ['acoustic', 'analog', 'latin']
const highlights = reactive({0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0, 6: 0, 7: 0})
const drumType = ref(DRUM_TYPES[0])
const hCountDown = []

const DRUM_CODES = { 'q': 0, 'w': 1, 'a': 2, 's': 3, 'e': 4, 'r': 5, 'd': 6, 'f': 7}
Object.keys(DRUM_CODES).forEach(character => {
  DRUM_CODES[character.toUpperCase()] = DRUM_CODES[character]
})

const setupDrumKeyboardInput = () => {
  window.addEventListener('keypress', e => {
    if (DRUM_CODES[e.key] || DRUM_CODES[e.key] === 0) onDrumHit(DRUM_CODES[e.key])
  })
}

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

export {drumType, DRUM_TYPES, highlights, setHightlight, onDrumHit, setupDrumKeyboardInput}
