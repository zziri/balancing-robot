
# Balancing Robot  


## Overview  

### Inverted Pendulum mechanism  

<img src="img/br_mechanism.JPG" width="500">  

* 로봇이 기울면 모터를 움직여서 중심을 잡는다
* 이것을 위해서 로봇이 기울어진 각도를 알 수 있는 센서 필요

### IMU Sensor  

* IMU(관성측정장비) : 가속도계, 회전 속도계, 자력계 등  

<img src="img/br_imu_sensor.JPG" width="250" title="MPU6050">  

* MPU6050
    * 회전 속도계, 가속도계가 결합된 형태의 센서 모듈
    * I2C 통신 인터페이스 포함

## Control  

### PID Control  

* 비례-적분-미분 제어기
    * 비례항 : 현재 상태에서의 오차값 크기에 비례한 제어작용
    * 적분항 : 오차값 크기를 적분하여 제어작용, 정상상태 오차를 줄이는 역할
    * 미분항 : 출력값의 급격한 변화를 줄여 오버슈트를 줄이고 안정성 향상

### Block Diagram  

<img src="img/br_control_block_diagram.JPG" width = "700">  

## Reference  

https://kocoafab.cc/make/view/719  

https://ko.wikipedia.org/wiki/관성_측정_장비    

https://www.makerlab-electronics.com/product/triple-axis-accelerometer-gyro-breakout-mpu6050/  

https://ko.wikipedia.org/wiki/PID_제어기  


