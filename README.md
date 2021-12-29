# plant-robot
>  PNU 2021 **CP34001: EMBEDDED SYSTEM DESIGN & LAB**  TERM PROJECT

##### 화분을 기르는 가장 스마트한 방법, Plant Robot.
* 블루투스로 로봇과 연결해 화분의 온도, 습도, 조도를 모바일 앱에서 확인해보세요!
* 모바일 앱으로 화분과 연결된 로봇을 움직여보세요!

## Structure

![str](https://user-images.githubusercontent.com/76420409/147640526-c42400f4-eaee-4cdb-acaf-a101c9805af6.png)

Plant Robot은 총 3 부분으로 이루어져있습니다.
* 로봇과 연결해 상태를 확인하고 로봇을 조정할 수 있는 [**모바일앱**](https://github.com/icarusicarus/plant-robot/tree/main/App)
* 온도, 습도, 조도를 센서를 통해 확인하고 습도가 낮으면 가습기를 켜고 장애물을 만나면 멈추게 조정하는 [**제어부**(STM32F107)](https://github.com/icarusicarus/plant-robot/tree/main/STM32Control)
* 앱과 메인보드의 제어에 따라 로봇의 보행을 담당하는 [**구동부**(OpenCR+Dynamixel)](https://github.com/icarusicarus/plant-robot/tree/main/Hexapod)



## Preview
![hexa](https://user-images.githubusercontent.com/76420409/147641255-77678d24-0727-4871-9918-2de10e63f389.gif)


