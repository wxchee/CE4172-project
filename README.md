# Virtual Drum
Arduino Nano 33 BLE + web app implementation that plays drum sounds based on recognized hand gesture.

This project highlighted on deploying a machine learning model directly on a resource-constrained environment, particularly an Arduino Nano 33 BLE board. It demonstrated the possibility of combining machine learning technique with an embedded system by giving precedence to embedded system performance while concurrently optimizing the model to align with this specific use case.

This project was developed as part of the coursework for NTU's CE4172 Internet of Things: Tiny Machine Learning.

## Demo (watch on YouTube)
<a href="http://www.youtube.com/watch?feature=player_embedded&v=hx05in64bPQ" target="_blank">
 <img src="http://img.youtube.com/vi/hx05in64bPQ/mqdefault.jpg" alt="Watch the video" width="560" height="315" border="10" />
</a>

## Overview
There are 4 gesture available for each hand(left/right)

<img src="readme/drum_ann.png" width="400">

Gesture for left hand are shown below

<img src="readme/top.gif" width="130"><img src="readme/twist.gif" width="130"><img src="readme/side.gif" width="130"><img src="readme/down.gif" width="130">

Gesture for right hand are basically mirroring the same gesture of left hands:

<img src="readme/top-r.gif" width="130"><img src="readme/twist-r.gif" width="130"><img src="readme/side-r.gif" width="130"><img src="readme/down-r.gif" width="130">

## Hardware & Software used
A simple block diagram of the setup

<img src="readme/block-diagram.jpg" width="400">

### Hardware
- 2x Arduino Nano 33 BLE/Arduino Nano 33 BLE Sense board
- 2x 9v Lithium batteries
- 2x battery holders
- a computer to run the drum web app

### Software
- Arduino IDE
- A [web application](https://wxchee.github.io/demo/tinyml/webapp/index.html) made with Vue.js

## Try it yourself

Step 1: Uploads the sketch file + model header file to your Arduino Nano 33 BLE board.

Sketch file + trained model header file:
- [left hand](sketch/left/)
- [right hand](sketch/right/)


Step 2: Visits the [web application](https://wxchee.github.io/demo/tinyml/webapp/index.html)


Step 3: Power up the board, connect the board to the web app via the "Add peripheral" button on the top left corner of the screen (see below video)

https://github.com/wxchee/CE4172-project/assets/33355985/f5cb36b8-b456-4b26-85bc-45a406192109



Step 4: Performs any of the gesture as stated under the Overview to play drum sound.


# Development
An ideal threshold is determined to allow data sampling session to be triggered without the system being too sensitive to any movement.

<img src="readme/threshold.png" width="800">

Also, a minimal system response time is determined via empirical tests:

<img src="readme/response.png" width="800">

Upon tests, threshold value of `0.16` and the goal response time `150 millisecond` are chosen.

## Data collection
Eight set of gesture datas are collected with each stores 40 captures. Each capture consists of 15 samples, with each sample holding six values covering 3-axis gyroscope and accelerometer’s data, which made input size to be a total of 90. An example of a single capture data is shown below:

<img src="readme/example-sample.png" width="400">

## Model training
Two separate models have been trained with each specifically represents left and right hand gestures respectively, instead of utilizing a single model to distinguish among all eight gestures. This strategy helps to simplify the model complexity, which not only contribute to the inference speed, but also reduce the model file size, presenting a more suitable approach for a resource constraint environment such as an Arduino board.

An initial setup of model configuration below yields good inference accuracy:

![image](https://github.com/wxchee/CE4172-project/assets/33355985/cc3783a8-f5d7-4af5-90e4-c2b5ff1894c4)

Despite the good inference accuracy, an input size of 180 shows significant delay in data sampling time(~246ms). The system response time is mainly dominated by data sampling time. Therefore, input size needs to be reduced further.

<img width="602" alt="image" src="https://github.com/wxchee/CE4172-project/assets/33355985/0e4b009b-d95d-49f4-8a97-1d4d03d8f478">

Beside input size, further tests are also conducted to reduce model complexity(neurons count per layer) while maintaining optimal inference performance.

Upon tests, it is deemed that an input size of 90, a model featuring 32 neurons in the first layer, and 16 neurons in the second layer helps to reduce both data sampling and inference times while showing no significant reduction in inference accuracy. This setup also ensures that the response time remains within 150ms, which satisfies the goal response time.

Below is the final model training configuration:

![image](https://github.com/wxchee/CE4172-project/assets/33355985/d0938e04-01a5-4145-b0a0-1d90195a2205)


Below depicts the final model performance:

<img src="readme/loss.png" width="300"> <img src="readme/accuracy.png" width="300">

The model yields a good prediction accuracy on test data.

<img src="readme/predict_onehand.png" width="230">

The model is then converted into a header file format that can be imported into the Arduino executable.


## Web Application
Web application dev directory: [drum-webapp](drum-webapp/)
 - an interface to play drum sound, establish bluetooth connection with Arduino board
 - receive gesture inference result from Arduino board, play drum sound based on the result
 - Data collection
   - initiate a data sampling session, receive bulk response of collected gesture data from Arduino board
   - convert collected gesture data into csv format for download


