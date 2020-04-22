# odrv_test.py
# 
# This script tests the basic functionality of the ODrive motor driver with its native python protocol
# author: Tyler McCown (tylermccown@engineering.ucla.edu)
# created: 28 February 2020

import odrive
from odrive.enums import *
import time
import numpy as np

print("ODrive py test")

# connect to drive
print("Finding an ODrive...")
odrv0 = odrive.find_any()
print("ODrive connected.\n")

# check bus voltage
print("Bus voltage: {:.3f} V\n".format(odrv0.vbus_voltage))

# check gpio pins
print("----- GPIO test -----")
for i in range(1, 4):
    print("voltage on GPIO{:d}: {:.3f} V".format(i, odrv0.get_adc_voltage(i)))
print("Test complete.\n")

# step velocity test
# print("----- Velocity test -----")
# time.sleep(1)
# odrv0.axis1.requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL
# print("Motor armed")
# time.sleep(1)
# for vel_ref in [100, 200, 300, 400, 500, 400, 300, 200, 100, 0, -100, -200, -300, -400, -500, -400, -300, -200, -100, 0]:
#     print("Target vel: {:.0f}".format(vel_ref), end=",\t")
#     odrv0.axis1.controller.vel_setpoint = vel_ref
#     time.sleep(2)
#     print("Estimated vel: {:.0f}". format(odrv0.axis1.encoder.vel_estimate))
# odrv0.axis1.requested_state = AXIS_STATE_IDLE
# print("Motor disarmed")
# print("Test complete.\n")

# sinusoidal velocity test
print("----- Sine test -----")
odrv0.axis1.requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL
print("Motor armed")
time.sleep(1)
ts = .01  # [s]
sine_per = 5  # [s]
t0 = time.time()
dt = np.zeros(65000)
data = np.zeros([65000, 3])
t_last = t0
t = t0
n_loops = 0
while t - t0 < 2*sine_per:
    t = time.time()
    vel_ref = 200*np.sin(2*np.pi/sine_per*t) + 400
    # print("Target vel: {:.0f}".format(vel_ref))
    odrv0.axis1.controller.vel_setpoint = vel_ref
    # dt[n_loops] = t - t_last
    data[n_loops, :] = [t, vel_ref, odrv0.axis1.encoder.pos_estimate]
    n_loops += 1
    t_last = t
    time.sleep(max(0, ts - (time.time() - t_last)))
odrv0.axis1.requested_state = AXIS_STATE_IDLE
data = data[np.where(data[:, 0] != 0)]
dt = dt[np.where(dt != 0)]  # slice nonzero values
# print("Mean time per loop: {:.3f} ms".format(np.mean(dt)*1000))
# print("Var time per loop: {:.2e} ms".format(np.var(dt)*1000))
# print("Max time per loop: {:.3f} ms".format(np.max(dt)*1000))
# print("Min time per loop: {:.2e} ms".format(np.min(dt)*1000))
print("Test complete.")

np.savetxt('data.csv', data, delimiter=',')
