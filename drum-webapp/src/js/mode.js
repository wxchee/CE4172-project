import { reactive } from "vue"
import { updateServerMode } from "./device"

const MODES = [{index: 0, label: 'Demo'}, {index: 1, label:'Data Collection'}]

const mode = reactive({
  index: 0,
  label: MODES[0]
})

const setMode = newMode => {
  mode.index = newMode.index
  mode.label = newMode.label
  updateServerMode(newMode.index.toString())
}

export {MODES, mode, setMode}
