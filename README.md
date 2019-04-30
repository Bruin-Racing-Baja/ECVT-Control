# Baja Bruin Racing
Microcontroller code for Bruin Racing's Baja SAE team.

Sketches with the prefix `actuator_` are meant to run on the actuator alone. Sketches with the prefix `eCVT_` are meant to run on the whole powertrain.

# Actuator (Good Boy)
We have two Thompson Max Jac linear actuators. One is from 2018 and is referred to as the old actuator, one is from 2019 and is referred to as the new actuator. They can be distinguished by the sticker on the side, which has a date printed on it. The distinction is important because the actuators have different PWM stop values and different potentiometer limits.

## wiring
- 1: pot 5V (green)
- 2: pot signal (brown)
- 3: motor + (white)
- 4: motor - (red)
- 5: pot GND (black)

## potentiometer limits (OUTDATED)
- min reading: 125 (on car)
- max reading: 762 (on car)
- engagement: 509

## PWM values
- max: 2000
- min: 1000
- stop (old): 1460
- stop (new): 1520

# Control Design

## Actuator Position Controllers
- A single sketch exists to control the actuator position using potentiometer feedback and velocity PWM control signals. It is a P controller with `Kp = 4`. Works very well.

## eCVT RPM Controllers

### "Birthday Controller"
- PI controller with `Kp = 6.26, Ki = 3.11`
    - anti-windup clamping such that integrated error term cannot exceed output signal saturation
    - software limit switches on potentiometer
    - 4-count moving average filter on RPM feedback
- reference: engine RPM
- output: actuator PWM
- feedback: engine RPM (calculated from hall effect)
- performance: works fairly well but is known to have an oscillatory transient during launch.

### "14th Birthday Controller"
- PID controller with `Kp = 14, Ki = 2.744, Kd = 0.626`
    - anti-windup clamping such that integrated error term cannot exceed output signal saturation
    - software limit switches on potentiometer
    - 4-count moving average filter on RPM feedback
    - requires hard coded logic to shift out if RPM measurement is below 500, otherwise actuator will shift in for unknown reason
    - reference signal switches between max torque and max power depending on engine RPM
    - ~~feedforward shift out bias when brakes are applied to avoid engine stalling~~
- reference: engine RPM
- output: actuator PWM
- feedback:
    - engine RPM (calculated from hall effect)
    - ~~gearbox RPM (calculated from hall effect)`~~
    - ~~brake on/off (measured from brake sensor)~~
- performance:
    - no noticable oscillation or overshoot
    - max torque reference leads to times very similar to mechanical CVT
    - selection of reference signal is currently based on engine RPM and performs very poorly. should be based on gearbox RPM

### Slow Lead-Lag
- lead-lag compensator with `K = 5, z1 = 5.6, p1 = 11.4, z2 = 0.949, p2 = 0.0949`
    - anti-windup clamping such that integrated error term cannot exceed output signal saturation
    - software limit switches on potentiometer
    - 4-count moving average filter on RPM feedback
- reference: engine RPM
- output: actuator PWM
- feedback: engine RPM (calculated from hall effect)
performance: does not shift in. noise in engine RPM signal appears to be too high. could try to solve by making an even slower controller or applying a more aggressive filter. also might be doing anti windup incorrectly which would lead to faulty output signals.

# Display

## wiring
- 1: SCL (green)
- 2: SDA (white)
- 3: NC (brown)
- 4: 5V (red)
- 5: GND (black)