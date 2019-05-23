
# Balancing Robot .ver2  

## Hardware  

### Circuit

<img src="../img/br_circuit_io_board.JPG" width="500">
<img src="../img/br_circuit_motor_driver_board.JPG" width="500">
<br><br>

각 두 개의 보드로 설계하고 제작했습니다. Balancing_HW_v3(I/O Board)의 Left, Right Header는 NUCLEO-F411RE 보드에서 각각 CN7, CN10에 연결되는 부분을 의미합니다.

### MCU  

STM32 기반의 NUCLEO-F411RE 보드를 사용했습니다.

NUCLEO-F411RE
<img src="../img/br_mcu.JPG" width="200">
개발환경은 Keil uVision5(MDK-ARM V5)을 사용하여 프로젝트를 빌드하고 타겟 보드(NUCLEO-F411RE)에 다운로드했습니다.

### IMU  

로봇의 각도를 측정하기 위함

#### MPU6050  

결선된 그림 넣기
I2C 통신을 활용, 각도 계산

### Motor  

#### Stepper  

사용한 스테퍼 모터 그림

#### Driver  

DRV8825
그림 넣기

### WiFi  

WizFi210 사용
그림 넣기

## Software  

### Control algorithm  

app.c 파일에 있음
<pre><code>
코드넣기
</code></pre>

interrupt.c 파일의 timer 콜백함수에서 호출됨
코드 넣기

### Motor drive task  

interrupt.c 파일의 timer 콜백 함수에 있음
코드 넣기

### Complementary filter algorithm

app.c의 getangle함수에 있음
코드 넣기

## References

<a href="">상보필터 출처</a>
데이터 시트 링크
프로그래밍 메뉴얼 링크
