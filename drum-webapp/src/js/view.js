import { reactive } from "vue"
import { updateDeviceParam } from "./device"

const VIEWS = [{index: 0, label: 'Demo'}, {index: 1, label:'Data Collection'}]

const view = reactive({
  index: 0,
  label: VIEWS[0]
})

const setView = newMode => {
  view.index = newMode.index
  view.label = newMode.label
  // updateDeviceParam(newMode.index)
  updateDeviceParam()
}

export {VIEWS, view, setView}
