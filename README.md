# Tiny-YOLO-Project

<img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=C%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/OpenCV-5C3EE8?style=for-the-badge&logo=OpenCV&logoColor=white"> 

![output](./output/output.gif)

## 소개
Advanced Driver Assistance System (ADAS) 구현 과제로 운전 상황에 따른 경고를 실시간으로 제공하여 편의성과 안전성을 향상시키는 것을 목표합니다.

## 기능

1. **Lane Departure!**: 차량이 차선을 이탈할 때 시스템이 감지하여 운전자에게 “Lane departure!” 경고 문구를 표시한다.
 - Rect 객체를 통해 Left, Center, Right Area를 정의하고 Canny Edge Detection을 진행
 - 검출된 Edge를 HoughLines 함수를 통해 차선을 검출하고 각도를 조정해 이탈 판단

2. **Start Moving Alert**: 차량이 정지 상태이고 앞차가 정지 상태에서 움직이기 시작하면 시스템에서 “Start Moving!” 경고 문구를 표시한다.
 - Tiny-YOLO를 사용해 차량 감지
 - 검출된 Bbox의 크기와 위치를 통해 앞 차량의 움직임 여부를 판단
3. **Human Detection Warning**: 운전 중 근처에 사람이 감지되면 시스템에서 “Human detected nearby!” 경고 문구를 표시한다.
 - Tiny-YOLO를 통해 사람 감지
4. **Car Detection Warning**: 운전 중 근처에 자동차가 감지되면 시스템에서 “Car detected nearby!” 경고 문구를 표시한다.
 - Tiny-YOLO를 사용해 차량 감지



