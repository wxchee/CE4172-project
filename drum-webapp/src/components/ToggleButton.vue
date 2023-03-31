<template>
  <div class="toggle-button" :class="{checked: modelValue === labels[1]}">
    <span class="label before">{{ labels[0] }}</span>
    <span class="toggle" @click="onToggle"></span>
    <span class="label after">{{ labels[1] }}</span>
  </div>
</template>

<script>
import { updateDeviceParam } from '@/js/device'

export default {
  props: {
    labels: Array,
    modelValue: String
  },
  emits: ['update:modelValue'],
  setup (props, {emit}) {
    const onToggle = () => {
      emit('update:modelValue', props.modelValue === props.labels[0] ? props.labels[1] : props.labels[0])
      updateDeviceParam()
    }

    return {onToggle}
  }
}
</script>

<style lang="scss">
.toggle-button {
  display: inline-block;
  color: #FFFFFF;
  text-align: center;

  span {
    display: inline-block;
    vertical-align: middle;
    margin-right: 5px;
  }

  .label:last-of-type { opacity: 0.5; }

  &.checked {
    .label:first-of-type { opacity: 0.5;}
    .label:last-of-type { opacity: 1;}

    .toggle::before {
      left: calc(100% - 24px);
    }
  }
  
  .toggle {
    position: relative;
    display: inline-block;
    width: 50px;
    height: 25px;
    background-color: #FFFFFF;
    border-radius: 25px;
    cursor: pointer;

    &:hover::before {
      opacity: 1;
    }

    &::before {
      content: '';
      position: absolute;
      opacity: 0.5;
      top: 50%;
      transform: translateY(-50%);
      width: 20px;
      height: 20px;
      border-radius: 50%;
      background-color: grey;
      left: 4px;
      transition: opacity 0.2s, left 0.2s;
    }
  }


}
</style>