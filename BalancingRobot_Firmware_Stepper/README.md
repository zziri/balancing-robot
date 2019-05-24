
# Balancing Robot .ver2  

## Hardware  

### Circuit

#### I/O Board Circuit  

* MCU, WizFi210(WiFi Module), MPU6050(IMU Sensor) and various connectors are connected  

<img src="../img/br_circuit_io_board.JPG" width="800">

Left Header and Right Header are connected to CN7 and CN10(NUCLEO-F411RE Morpho headers) respectively  

#### Motor Driver Board  

* DRV8825(Stepper Driver) and various connectors are connected  

<img src="../img/br_circuit_motor_driver_board.JPG" width="800">
<br>

### MCU  

* Used **NUCLEO-F411RE** Board

<img src="../img/br_mcu.JPG" width="250"><br>

* <a href="https://www.st.com/resource/en/datasheet/stm32f411re.pdf">STM32F411RE Datasheet</a>

* IDE : <a href="http://www2.keil.com/mdk5/uvision/">Keil uVision5</a>

### IMU  

* <a href="https://github.com/dvlpr-shark/Balancing-Robot#imu-sensor">IMU Sensor</a>

### Motor  

#### Stepper  

* <a href="https://ko.wikipedia.org/wiki/%EC%8A%A4%ED%85%8C%ED%8D%BC%EB%AA%A8%ED%84%B0">Stepper Motor wikipedia</a>

#### Driver  

* Used **DRV8825**
<img src="../img/br_drv8825.JPG" width="200">

* <a href="http://www.ti.com/lit/ds/symlink/drv8825.pdf"> DRV8825 Datasheet</a>

* <a href="http://www.hobbytronics.co.uk/drv8825-stepper-motor-driver">Wiring Diagram</a>

### WiFi  

* Used **WizFi210**

<img src="../img/br_wizfi210.JPG" width="200">

* <a href="https://www.wiznet.io/wp-content/uploads/wiznethome/WiFi%20Module/WizFi_210_220/Document/WizFi210_DS_V120E.pdf">WizFi210 Datasheet</a>

## Software  

### Control algorithm  

* functions in the **app.c** file

```
void PostureControl(void)
{
  // this function is pid controller for posture control
}
```

```
void CentroidControl(void)
{
  // this function is pid controller for centroid control
}
```

* these are called from **timer period elapsed callback**
* in the **interrupt.c** file
* TIM2 frequency = 1KHz

```
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
```

### Motor drive task  

* **timer period elapsed callback** in the **interrupt.c** file
* TIM3 frequency = 50KHz

```
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
  if(htim->Instance == TIM3){
    // motor drive task ...
  }

  /*... codes ...*/
}
```

### Complementary filter algorithm

* Algorithm
  * <a href="https://alnova2.tistory.com/1085">Reference</a>

```
Filtered Angle = α × (Gyroscope Angle) + (1 − α) × (Accelerometer Angle) α = τ/(τ + Δt)

(Gyroscope Angle) = (Last Measured Filtered Angle) + ω×Δt  

Δt = sampling rate, τ = time constant greater than timescale of typical accelerometer noise
```

* in the **app.c** file

```
void GET_ANGLE(void)
{
  /*... codes ...*/

  else if(!(flag.set_angle_offset)){
    gotAngle = A*(gotAngle + gyroXrate*timePass) + (1 - A)*accXangle;   /* Complementary Filter */
  }

  /*... codes ...*/
}
```
