# odrv_stop.py
#
# This script writes zero velocity commands and idle axis states to both motors
#
# author: Tyler McCown (tylermccown@engineering.ucla.edu)
# created: 28 February 2020

import odrive
from odrive.enums import *

print("Finding an ODrive...")
odrv0 = odrive.find_any()
print("ODrive connected.\n")

odrv0.axis0.controller.vel_setpoint = 0
odrv0.axis1.controller.vel_setpoint = 0
odrv0.axis0.requested_state = AXIS_STATE_IDLE
odrv0.axis1.requested_state = AXIS_STATE_IDLE
print("Axes stopped.")
print("Completed.")
