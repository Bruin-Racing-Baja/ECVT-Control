# Baja Bruin Racing / ECVT Control
Control design and development code for Baja Bruin Racing ECVT.

- [Development environment](#development-environment)
- [Model 20](#model-20)
- [Model 19 - Blue Car](#model-19---blue-car)


All scripts are organized into three main folders:
- `test` contains test code for all hardware including sensors, microcontrollers, IC's, etc. They are often single use and do not include wiring diagrams.
- `system_id` contains system identification code for independent actuators or the whole car. Wiring diagrams are shown in EasyEDA and are the overall car schematic or just the isolated actuator components.
- `control` contains control code for independent actuators or the whole car. Wiring diagrams are shown in EasyEDA and are the overall car schematic or just the isolated actuator components.

The first section in this document is a short tutorial for the primary development environment used in this repo. The following sections are some documentation about the control systems for various years.

# Development environment

## Installation

The development platform primarily used is [VS Code](https://code.visualstudio.com/) with the [PlatformIO extension](https://platformio.org/install/ide?install=vscode). If PIO Home does not open upon installation, it can be accessed from the PlatformIO toolbar on the left side of VS Code under `QUICK ACCESS > PIO Home > Open`. Be sure to install the appropriate board packages from the `Boards` tab of PIO Home before starting a new project.

## Arduino libraries Environment variable

Platform IO allows sketches to be compiled and uploaded to an embededded device directly from VS Code. As such, the IDE must have access to any libraries which your code depends on, typically installed in `~/Documents/Arduino/libraries`. Because various users have different paths, adding the path to your libraries folder will not work for everyone and will result in code that only you can compile. This is not desirable for collaborative development. The simplest solution is for each user to define a system environment variable which points to their libraries directory, and point the IDE towards the environment variable.

To define a system environment variable in Windows, open the control panel (the Windows 7 version) and navigate to `System and Security > System > Advanced system settings > Environment Variables`, then click `New` in the `System variables` window (below). Enter `ARDUINO_LIB` for the variable name, then browse to your libraries directory for the variable value. **All users must use the name `ARDUINO_LIB` in order for this to work**. Instructions on adding the environment variable to the compilation path are given [below](##creating-a-new-project).

## Creating a new project

In the `Home` tab of PIO Home, select `New Project` from the Quick Access menu. This opens the Project Wizard window. Name the project, select the board (see [note](##installation) above about installing boards), and select the appropriate location. This will create a new project in the location directory that you selected and add the project to your VS Code workspace. If your workspace was already populated then you will need to open a new window whose workspace contains only the new project in order to use the native build feature. You will know that your workspace is correctly setup if you see a row of icons at the bottom left corresponding to Platform IO features. To verify, the project can be compiled with `ctrl-alt-B` or by clicking the check mark icon at the bottom left. A terminal session should be appear and run the compilation routine. The project can be uploaded to the connected board with `ctrl-alt-U` or by clicking the right arrow icon at the bottom left.

If your project uses any locally installed libraries then you will need to point Platform IO to your local libraries directory. The best way to do this for code portability is to use an environment variable which points towards your local library. Instructions on defining an environment variable is given [above](##arduino-libraries-environment-variable). Once the variable is defined, add the following line to your `platform.ini` under the environment which you will be using
```ini
lib_extra_dirs = ${sysenv.ARDUINO_LIB}
```

# Model 20

## Actuator - BERTHA

Model 20 uses the Baja ECVT Reworked THomson Actuator (BERTHA) mechanism, Bruin Racing's first ever custom actuation system. It is based on a [Racerstar BLDC motor](https://www.racerstar.com/racerstar-5065-brh5065-200kv-6-12s-brushless-motor-without-gear-for-balancing-scooter-p-115.html) which drives a ball-screw mechanism. The mechanism is similar to the Thomson Max Jac used in Model 19, but with greatly increased shift speed. Additional sensors include a potentiometer for absolute position feedback, unipolar digital hall effect sensors for limit detection, and hard stop limit switches to prevent overshifting. The BLDC has internal digital hall effect sensors to closed loop speed control through a supporting ESC. The motor driver of choice is an [ODrive 3.6](https://odriverobotics.com), accepting a commanded velocity through serial communication and running an internal closed loop velocity controller. 

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

## Controller - BECCY
The control law has not yet been designed.

# Model 19 - Blue Car

## Actuator - Good Boy
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
