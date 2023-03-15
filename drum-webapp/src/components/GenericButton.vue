<template>
  <div class="generic-button" :disabled="disabled" @click="onClick">
    <slot></slot>
  </div>
</template>

<script>
export default {
  props: {
    disabled: {
      type: Boolean,
      default: false
    }
  },
  emits: ['click'],
  setup(props, {emit}) {
    const onClick = e => emit('click', e)

    return {onClick}
  }
}
</script>

<style lang="scss">
.generic-button {
  padding: 6px 20px;
  border-radius: 20px;
  color: #FFFFFF;
  background-color: rgba(#9e9e9e, 0.5);
  cursor: pointer;
  margin-right: 10px;

  &:hover {
    background-color: #9e9e9e;
  }
}
</style>