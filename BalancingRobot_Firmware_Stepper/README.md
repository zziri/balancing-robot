
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

* main function in the **app.c** file

```
void PostureControl(void)
{
  // this function is pid controller for posture control
}
```


<pre><code>
void CentroidControl(void)
{
  // this function is pid controller for centroid control
}
</code></pre>

* these are called from **timer period elapsed callback**
* in the **interrupt.c** file
* TIM2 frequency = 1KHz

<pre><code>
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
  /*... codes ...*/
  else if(htim->Instance == TIM2){
		/*... codes ...*/

		PostureControl();

		/*... codes ...*/

		CentroidControl();

		/*... codes ...*/
	}
  /*... codes ...*/
}
</code></pre>

### Motor drive task  

* **timer period elapsed callback** in the **interrupt.c** file
* TIM3 frequency = 50KHz

<pre><code>
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM3){
		// motor drive task ...
  }

  /*... codes ...*/
}
</code></pre>

### Complementary filter algorithm

app.c의 getangle함수에 있음
코드 넣기

## References

<a href="">상보필터 출처</a>
데이터 시트 링크
프로그래밍 메뉴얼 링크
