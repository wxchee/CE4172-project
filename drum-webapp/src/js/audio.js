import {ref} from 'vue'
const audioReady = ref(false)
const isLoading = ref(false)
let audioCtx = null

const fetchAudio = path => {
  return fetch(path)
  .then(res => res.arrayBuffer())
  .then(bf => audioCtx.decodeAudioData(bf))
}

let buffers = []

const loadAudioDatas = cb => {
  isLoading.value = true
  audioCtx = new AudioContext()

  Promise.all([
    fetchAudio('audios/acoustic/0-crash-1.wav'),
    fetchAudio('audios/acoustic/1-tom-1.wav'),
    fetchAudio('audios/acoustic/2-hihat.wav'),
    fetchAudio('audios/acoustic/3-snare-1.wav'),
    fetchAudio('audios/acoustic/4-tom-2.wav'),
    fetchAudio('audios/acoustic/5-crash-2.wav'),
    fetchAudio('audios/acoustic/6-kick-1.wav'),
    fetchAudio('audios/acoustic/7-snare-2.wav')
  ]).then(bfs => {
    buffers = bfs
    
    setTimeout(() => {
      audioReady.value = true
      isLoading.value = false
      cb()
      console.log('audio data loaded')
    }, 1000);
    
  })
}

const play = i => {
  const track = audioCtx.createBufferSource()
  track.buffer = buffers[i]
  track.connect(audioCtx.destination)
  track.start(0)
}

export {audioReady, loadAudioDatas, play, isLoading}

