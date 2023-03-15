import { onDrumHit } from '@/js/drum'

const DRUM_CODES = {
  'q': 0,
  'w': 1,
  'a': 2,
  's': 3,
  'e': 4,
  'r': 5,
  'd': 6, 
  'f': 7
}
Object.keys(DRUM_CODES).forEach(character => {
  DRUM_CODES[character.toUpperCase()] = DRUM_CODES[character]
})


const setupKeyboardInput = () => {
  window.addEventListener('keypress', e => {
    if (DRUM_CODES[e.key] || DRUM_CODES[e.key] === 0) onDrumHit(DRUM_CODES[e.key])
  })
}

export {setupKeyboardInput}
