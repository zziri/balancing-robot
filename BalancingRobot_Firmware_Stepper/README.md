
# Balancing Robot .ver2  

## Hardware  

### Circuit

#### I/O Board Circuit  

* MCU, WizFi210(WiFi Module), MPU6050(IMU Sensor) and various connectors are connected  

<img src="../img/br_circuit_io_board.JPG" width="500">

Left Header and Right Header are connected to CN7 and CN10 respectively  

#### Motor Driver Board  

* DRV8825(Stepper Driver) and various connectors are connected  

<img src="../img/br_circuit_motor_driver_board.JPG" width="500">
<br><br>

### MCU  

* Used **NUCLEO-F411RE** Board

<img src="../img/br_mcu.JPG" width="200"><br>

* <a href="https://www.st.com/resource/en/datasheet/stm32f411re.pdf">STM32F411RE Datasheet</a>

* IDE : <a href="http://www2.keil.com/mdk5/uvision/">Keil uVision5</a>

### IMU  

* <a href="https://github.com/dvlpr-shark/Balancing-Robot#imu-sensor">IMU Sensor</a>

### Motor  

#### Stepper  

* <a href="https://ko.wikipedia.org/wiki/%EC%8A%A4%ED%85%8C%ED%8D%BC%EB%AA%A8%ED%84%B0">Stepper Motor wikipedia</a>

#### Driver  

* DRV8825
<img src="../img/br_drv8825.JPG" width="200">

* <a href="http://www.ti.com/lit/ds/symlink/drv8825.pdf"> DRV8825 Datasheet</a>

* <a href="http://www.hobbytronics.co.uk/drv8825-stepper-motor-driver">Wiring Diagram</a>

### WiFi  

WizFi210 사용
그림 넣기

## Software  

### Control algorithm  

* app.c
<pre><code>
// PID Posture Control
void PostureControl(void)
{
	static double preinput = 0.0;
	double input = centroid.control.result - angleResult;

	if(flag.restart_control)																																		/*if restart control*/
		preinput = 0.0, flag.restart_control = 0;																									/*clear preinput*/

	posture.control.p = posture.gain.p * input;																									/*Proportional control*/
	posture.control.i += posture.gain.i * input;																								/*Integral control*/
	if(posture.control.i > MAX_SPEED - MARGIN){																									/*Limit max output*/
		if(posture.control.i > 0)	posture.control.i = MAX_SPEED - MARGIN;
		else	posture.control.i = -MAX_SPEED - MARGIN;
	}
	posture.control.d = posture.gain.d * (input - preinput);																		/*Differential control*/
	posture.control.result = posture.control.p + posture.control.i + posture.control.d;					/*Sum PID Controller Result signal*/

	if(posture.control.result > MAX_SPEED - MARGIN){																						/*Limit max output*/
		if(posture.control.result > 0)	posture.control.i = MAX_SPEED - MARGIN;
		else	posture.control.result = -MAX_SPEED - MARGIN;
	}

	preinput = input;																																						/*renew preinput*/
}
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
