/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: delay_types.h
 *
 * Code generated for Simulink model 'delay'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 9.2 (R2019b) 18-Jul-2019
 * C/C++ source code generated on : Sat Feb  1 18:20:55 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_delay_types_h_
#define RTW_HEADER_delay_types_h_
#include "rtwtypes.h"

/* Custom Type definition for MATLABSystem: '<Root>/Analog Input' */
#include "MW_SVD.h"
#ifndef typedef_d_cell_delay_T
#define typedef_d_cell_delay_T

typedef struct {
  char_T f1;
  char_T f2;
  char_T f3;
  char_T f4;
  char_T f5;
  char_T f6;
  char_T f7;
  char_T f8;
  char_T f9[2];
  char_T f10[2];
  char_T f11[2];
  char_T f12[2];
} d_cell_delay_T;

#endif                                 /*typedef_d_cell_delay_T*/

#ifndef typedef_f_codertarget_arduinobase_int_T
#define typedef_f_codertarget_arduinobase_int_T

typedef struct {
  d_cell_delay_T AvailablePwmPinNames;
} f_codertarget_arduinobase_int_T;

#endif                               /*typedef_f_codertarget_arduinobase_int_T*/

#ifndef typedef_codertarget_arduinobase_inter_T
#define typedef_codertarget_arduinobase_inter_T

typedef struct {
  boolean_T matlabCodegenIsDeleted;
  int32_T isInitialized;
  boolean_T isSetupComplete;
  real_T SampleTime;
  f_codertarget_arduinobase_int_T *Hw;
  MW_Handle_Type MW_ANALOGIN_HANDLE;
  uint8_T PinAnalog;
} codertarget_arduinobase_inter_T;

#endif                               /*typedef_codertarget_arduinobase_inter_T*/

/* Parameters (default storage) */
typedef struct P_delay_T_ P_delay_T;

/* Forward declaration for rtModel */
typedef struct tag_RTM_delay_T RT_MODEL_delay_T;

#endif                                 /* RTW_HEADER_delay_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
