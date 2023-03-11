import {ref} from 'vue'
const audioReady = ref(false)
const isLoading = ref(false)
let audioCtx = null

const drumTypes = {
  'General': [
    'acoustic/0-crash-1.wav',
    'acoustic/1-tom-1.wav',
    'acoustic/2-hihat.wav',
    'acoustic/3-snare-1.wav',
    'acoustic/4-tom-2.wav',
    'acoustic/5-crash-2.wav',
    'acoustic/6-kick-1.wav',
    'acoustic/7-snare-2.wav'
  ],
  'Analog': [
    'analog/0-crash-1.wav',
    'analog/1-clap.wav',
    'analog/2-hihat-1.wav',
    'analog/3-snare-1.wav',
    'analog/4-conga.wav',
    'analog/5-crash-2.wav',
    'analog/6-kick.wav',
    'analog/7-snare-2.wav'
  ],
  'Latin': [
    'latin/0-Shaker.wav',
    'latin/1-CoffeeCup.wav',
    'latin/2-Guiro.wav',
    'latin/3-Conga.wav',
    'latin/4-Bongo.wav',
    'latin/5-Conga-4.wav',
    'latin/6-Timbale-2.wav',
    'latin/7-WoodBlock.wav'
  ]
}

const fetchAudio = path => {
  return fetch(`audios/${path}`)
  .then(res => res.arrayBuffer())
  .then(bf => audioCtx.decodeAudioData(bf))
}

const buffers = {
  'General': null,
  'Analog': null,
  'Latin': null
}

const loadAudioDatas = cb => {
  isLoading.value = true
  audioCtx = new AudioContext()

  Promise.all([
    ...drumTypes['General'].map(path => fetchAudio(path)),
    ...drumTypes['Analog'].map(path => fetchAudio(path)),
    ...drumTypes['Latin'].map(path => fetchAudio(path)),
  ]).then(bfs => {
    buffers['General'] = bfs.slice(0, 8)
    buffers['Analog'] = bfs.slice(8, 16)
    buffers['Latin'] = bfs.slice(16, 24)
    
    setTimeout(() => {
      audioReady.value = true
      isLoading.value = false
      cb()
      console.log('audio data loaded')
    }, 1000);
    
  })
}

const play = (type, i) => {
  const track = audioCtx.createBufferSource()
  track.buffer = buffers[type][i]
  track.connect(audioCtx.destination)
  track.start(0)
}

export {audioReady, loadAudioDatas, play, isLoading}

