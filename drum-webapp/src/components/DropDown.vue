<template>
  <select @change="e => onChange(e)">
    <option v-for="(opt, i) in options" :key="i" :value="opt">{{ optionVals[opt] }}</option>
  </select>
</template>

<script>

export default {
  props: ['modelValue', 'options', 'optionVals'],
  emits: ['update:modelValue', 'change'],
  setup (props, {emit}) {
    const onChange = e => {
      emit('update:modelValue', e.target.value)
      emit('change', e.target.value)
    }

    return {onChange}
  }
}
</script>