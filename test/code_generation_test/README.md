# code_generation_test

This is a test of the Simulink code generation functionality, which compiles Simulink models to various embedded platforms.

`code_generation_lpf.png` includes an output plot from implementing a first order Butterworth filter coded in Simulink onto an Arduino Uno. The same implementation was attempted using a third party Arduino Nano but was unable to upload.

While automatic code generation would be extremely helpful for implementing the control law alone, it seems to prevent inclusion of other code which we would write ourselves, meaning that we cannot include other features such as data saving, wireless transmission, etc. It is also unclear whether the code runs when the microcontroller is not connected to a serial port.

