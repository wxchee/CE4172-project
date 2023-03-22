import {ref} from 'vue'
import { setupDrumKeyboardInput } from '@/js/drum'

const audioReady = ref(false)
const isAudioLoading = ref(false)
let audioCtx = null

const drumAudioFiles = {
  'acoustic': [
    '0-crash-1.wav',
    '1-tom-1.wav',
    '2-hihat.wav',
    '3-snare-1.wav',
    '4-tom-2.wav',
    '5-crash-2.wav',
    '6-kick-1.wav',
    '7-snare-2.wav'
  ],
  'analog': [
    '0-crash-1.wav',
    '1-clap.wav',
    '2-hihat-1.wav',
    '3-snare-1.wav',
    '4-conga.wav',
    '5-crash-2.wav',
    '6-kick.wav',
    '7-snare-2.wav'
  ],
  'latin': [
    '0-Guiro.wav',
    '1-Conga-4.wav',
    '2-Shaker.wav',
    '3-Conga.wav',
    '4-Bongo.wav',
    '5-CoffeeCup.wav',
    '6-Timbale-2.wav',
    '7-WoodBlock.wav'
  ]
}

const fetchAudio = path => {
  return fetch(path)
  .then(res => res.arrayBuffer())
  .then(bf => audioCtx.decodeAudioData(bf))
}

const buffers = {
  'acoustic': null,
  'analog': null,
  'latin': null
}

const loadAudioDatas = () => {
  isAudioLoading.value = true
  audioCtx = new AudioContext()

  Promise.all([
    ...drumAudioFiles['acoustic'].map(path => fetchAudio(`audios/acoustic/${path}`)),
    ...drumAudioFiles['analog'].map(path => fetchAudio(`audios/analog/${path}`)),
    ...drumAudioFiles['latin'].map(path => fetchAudio(`audios/latin/${path}`)),
  ]).then(bfs => {
    buffers['acoustic'] = bfs.slice(0, 8)
    buffers['analog'] = bfs.slice(8, 16)
    buffers['latin'] = bfs.slice(16, 24)
    
    setupDrumKeyboardInput()
    setTimeout(() => {
      audioReady.value = true
      isAudioLoading.value = false
      console.log('audio data loaded')
    }, 300)
    
  })
}

const play = (type, i) => {
  const track = audioCtx.createBufferSource()
  track.buffer = buffers[type][i]
  track.connect(audioCtx.destination)
  track.start(0)
}

export {audioReady, loadAudioDatas, play, isAudioLoading}

