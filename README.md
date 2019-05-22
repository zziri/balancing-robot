
<!--
추가해야 할 것
1. 밸런싱 로봇 개요
2. 제어 블록선도
3. 하드웨어 설명
    기구물
    회로도
    각 부품 설명
4. 참조한 것들
    상보필터 알고리즘
    STM32 문서(프로그래밍 메뉴얼, 데이터시트)
    OScopeCtrl Class 라이브러리
    상민이형 WizFi 설정 메뉴얼
    논문들...
-->

# Balancing Robot
<br>
<br>

## Overview
<br>

### 1. Inverted Pendulum mechanism
<br>

<img src="img/br_mechanism.JPG" width="500">
source : https://kocoafab.cc/make/view/719
<br><br>

* 로봇이 기울면 모터를 움직여서 중심을 잡는다
* 이것을 위해서 로봇이 기울어진 각도를 알 수 있는 센서 필요

### 2. IMU Sensor
<br>

* IMU(관성측정장비) : 가속도계, 회전 속도계, 자력계 등<br>

  * [IMU wikipedia](https://ko.wikipedia.org/wiki/%EA%B4%80%EC%84%B1_%EC%B8%A1%EC%A0%95_%EC%9E%A5%EB%B9%84)

MPU6050<br>
<img src="img/br_imu_sensor.JPG" width="250">

*

## Control
<!--
제어 블록선도 그림
-->
<br>

## Hardware
- Circuit
<br>

## References
<br>
