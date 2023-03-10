import {ref} from 'vue'
const audioReady = ref(false)

const audioCtx = new AudioContext()

const fetchAudio = path => {
  return fetch(path)
  .then(res => res.arrayBuffer())
  .then(bf => audioCtx.decodeAudioData(bf))
}

let buffers = []

const loadAudioDatas = () => {
  Promise.all([
    fetchAudio('audios/acoustic/0-crash-1.wav'),
    fetchAudio('audios/acoustic/1-hihat-open.wav'),
    fetchAudio('audios/acoustic/2-tom-1.wav'),
    fetchAudio('audios/acoustic/3-snare-1.wav'),
    fetchAudio('audios/acoustic/4-crash-2.wav'),
    fetchAudio('audios/acoustic/5-tom-2.wav'),
    fetchAudio('audios/acoustic/6-snare-2.wav'),
    fetchAudio('audios/acoustic/7-kick-1.wav')
  ]).then(bfs => {
    buffers = bfs

    audioReady.value = true
    console.log('audio data loaded')
  })
}

const play = i => {
  const track = audioCtx.createBufferSource()
  track.buffer = buffers[i]
  track.connect(audioCtx.destination)
  track.start(0)
}

loadAudioDatas()

export {audioReady, play}

