# Baja Bruin Racing
Control design and development code for Baja Bruin Racing ECVT.

- [Model 20](#Model%2020)
    <!-- * [Actuator - "BERTHA"](#Actuator%20-%20"BERTHA") -->
    <!-- * [Control hardware](##Control%20hardware) -->
    <!-- * [Controller - "BECCY"](##Controller%20-%20"BECCY") -->
- [Model 19 - "Blue Car"](##Model%2019---"Blue-Car")
    <!-- * [Actuator - "Good Boy"](#Actuator%20-%20"Good%20Boy") -->
    <!-- * [Control hardware](##Control%20hardware) -->
    <!-- * [Display](##Display) -->


All scripts are organized into three main folders:
- `test` contains test code for all hardware including sensors, microcontrollers, IC's, etc. They are often single use and do not include wiring diagrams.
- `system_id` contains system identification code for independent actuators or the whole car. Wiring diagram are shown in EasyEDA and are the overall car schematic or just the isolated actuator components.
- `control` contains control code for independent actuators or the whole car. Wiring diagram are shown in EasyEDA and are the overall car schematic or just the isolated actuator components.

# Model 20

## Actuator - "BERTHA"

Model 20 uses the Baja ECVT Reworked THomson Actuator (BERTHA) mechanism, Baja Bruin Racing's first ever custom actuation system. It is based on a [Racerstar BLDC motor](https://www.racerstar.com/racerstar-5065-brh5065-200kv-6-12s-brushless-motor-without-gear-for-balancing-scooter-p-115.html) which drives a ball-screw mechanism. The mechanism is similar to the Thomson Max Jac used in Model 19, but with greatly increased shift speed. Additional sensors include a potentiometer for absolute position feedback, unipolar digital hall effect sensors for limit detection, and hard stop limit switches to prevent overshifting. The BLDC has internal digital hall effect sensors to closed loop speed control through a supporting ESC. The motor driver of choice is an [ODrive 3.6](https://odriverobotics.com), accepting a commanded velocity through serial communication and running an internal closed loop velocity controller. 

### Wiring

#### Motor
The BLDC three-phase power wires may be connected to the motor driver power channels in any order. If the observed velocity is not in the intended direction, any two power wires may be switched to reverse the direction. The ODrive has two possible channels for power and sensor feedback, and each has been internally tuned for a different motor. The correct motor channel is marked on the back of each Racerstar motor.

The encoder wires have been fixed in a Dupont connector so that their order matches that of the ODrive headers, listed as follows:
- red - 5V
- blue - A
- white - B
- yellow - Z
- black - GND

The order of the encoder channels may be changed if desired by recalibrating the ODrive channel after the wires have been switched to the desired order.

#### Potentiometer
The potentiometer has not yet been wired in the final housing.

#### Limit switches
The limit switches have not yet been wired in the final housing.

## Control hardware

## Controller - "BECCY"
The control law has not yet been designed.

# Model 19 - "Blue Car"

## Actuator - "Good Boy"
Blue Car uses a Thompson Max Jac ball-screw linear actuator. The mechanism is a brushed DC motor which is geared to drive a ball-screw. A Syren motor driver is used to power the actuator based on an RC servo signal interpreted as an open loop commanded velocity. A built-in potentiometer provides optional position feedback which can be read by the microcontroller.

### wiring
1. pot 5V (green)
2. pot signal (brown)
3. motor + (white)
4. motor - (red)
5. pot GND (black)

### potentiometer limits (check these after system has been reassembled)
- min reading: 163 (on car)
- max reading: 254 (on car)
- engagement: 245

### PWM values
- max: 2000
- min: 1000
- stop: 1515

## Control hardware

An Arduino Uno was used as the microcontroller to drive this setup. Engine RPM was measured with an analog hall effect sensor that triggered once per engine revolution, concurrent with the spark plug firing. Gearbox RPM was measured with a bipolar digital hall effect sensor that triggered four terms per rotation of the compound gear, thanks to four magnets or alternating polarity pressed into the gear.

At this time there is no complete wiring diagram for all hardware on Blue Car. [thomaskost17](https://github.com/thomaskost17) is designing a reworked schematic which makes use of the ATMEGA328P-PU microcontroller alone rather than the full Arduino platform. A schematic of this project is available on EasyEDA as `Blue Car Rework`.

## Control Design

### Actuator Position Controllers
A single sketch exists to control the actuator position using potentiometer feedback and velocity PWM control signals. It is a P controller with `Kp = 4`. Works very well.

### ECVT RPM Controllers

The controller which maximized performance for complexity was a unity gain P controller, stored in `eCVT_Competition_Controller`. It is likely that more complicated controllers were not correctly implemented and thus did not perform as expected.

## Display

This component was never successfully implemented with the full system. It worked in bench tests as an isolated component, but not on the car with all other components. It is suspected that the Arduino was unable to supply enough output current to drive all of the sensors and the display, and that the long I2C wires which spanned the length of the car were unable to preserve the signal integrity.

### wiring
1. SCL (green)
2. SDA (white)
3. NC (brown)
4. 5V (red)
5. GND (black)
