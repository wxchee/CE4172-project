<template>
  <div class="loading-visual">
    <div>Loading...</div>
  </div>
</template>

<style lang="scss">
.loading-visual {
  position: fixed;
    z-index: 1;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background-color: rgba(black, 0.9);

    & > div {
      position: absolute;
      color: white;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
    }
}
</style>