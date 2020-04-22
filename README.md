# Baja Bruin Racing
Control design and development code for Baja Bruin Racing.

- Sketches with the prefix `actuator_` are meant to run on the actuator alone.
- Sketches with the prefix `eCVT_` are meant to run on the whole powertrain.
- `actuator_position_control` can run either on the standalone actuator or the whole powertrain.

# Actuator (Good Boy)
We have a Thompson Max Jac linear actuator.

## wiring
- 1: pot 5V (green)
- 2: pot signal (brown)
- 3: motor + (white)
- 4: motor - (red)
- 5: pot GND (black)

## potentiometer limits
- min reading: 163 (on car)
- max reading: 254 (on car)
- engagement: 245

## PWM values
- max: 2000
- min: 1000
- stop: 1515

# Control Design

## Actuator Position Controllers
- A single sketch exists to control the actuator position using potentiometer feedback and velocity PWM control signals. It is a P controller with `Kp = 4`. Works very well.

## eCVT RPM Controllers

The controller which maximized performance for complexity was a unity gain P controller, stored in `eCVT_Competition_Controller`. It is likely that more complicated controllers were not correctly implemented and thus did not perform as expected.

# Display

## wiring
- 1: SCL (green)
- 2: SDA (white)
- 3: NC (brown)
- 4: 5V (red)
- 5: GND (black)
