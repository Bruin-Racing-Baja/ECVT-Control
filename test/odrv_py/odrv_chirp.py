# odrv_chirp.py
#
# Runs a chirp response on the ODrive + motor system
# author: Tyler McCown (tylermccown@engineering.ucla.edu)
# created: 15 March 2020

import odrive
from odrive.enums import *
import time
import numpy as np
import signals

print("ODrive chirp test")

# connect to drive
print("Finding an ODrive...")
odrv0 = odrive.find_any()
print("ODrive connected.\n")

amp = 200
start_freq = .1
stop_freq = 10
per = 20
offset = 400
signals.configure_chirp(amp, start_freq, stop_freq, per, offset)

print("----- chirp test -----")
odrv0.axis1.requested_state = AXIS_STATE_CLOSED_LOOP_CONTROL
print("Motor armed")
time.sleep(1)
ts = .01  # [s]
t0 = time.time()
t_last = t0
t = t0
n_loops = 0
n_max = round(20/ts)
data = np.zeros([n_max, 3])
while n_loops < n_max:
    t = time.time()
    vel_ref = signals.chirp(t - t0)
    odrv0.axis1.controller.vel_setpoint = vel_ref
    data[n_loops, :] = [t, vel_ref, odrv0.axis1.encoder.pos_estimate]
    n_loops += 1
    t_last = t
    time.sleep(max(0, ts - (time.time() - t_last)))
odrv0.axis1.requested_state = AXIS_STATE_IDLE
print("Motor disarmed")
print("Test complete.")

np.savetxt('chirp_data.csv', data, delimiter=',')
