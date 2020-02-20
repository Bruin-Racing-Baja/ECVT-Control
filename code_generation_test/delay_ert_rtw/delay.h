/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: delay.h
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

#ifndef RTW_HEADER_delay_h_
#define RTW_HEADER_delay_h_
#include <stddef.h>
#ifndef delay_COMMON_INCLUDES_
# define delay_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "MW_AnalogIn.h"
#endif                                 /* delay_COMMON_INCLUDES_ */

#include "delay_types.h"
#include "MW_target_hardware_resources.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               ((rtm)->Timing.t)
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  codertarget_arduinobase_inter_T obj; /* '<Root>/Analog Input' */
  f_codertarget_arduinobase_int_T gobj_0;/* '<Root>/Analog Input' */
  f_codertarget_arduinobase_int_T gobj_1;/* '<Root>/Analog Input' */
  f_codertarget_arduinobase_int_T gobj_2;/* '<Root>/Analog Input' */
  f_codertarget_arduinobase_int_T gobj_3;/* '<Root>/Analog Input' */
} DW_delay_T;

/* Parameters (default storage) */
struct P_delay_T_ {
  real_T AnalogInput_SampleTime;       /* Expression: -1
                                        * Referenced by: '<Root>/Analog Input'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_delay_T {
  const char_T *errorStatus;
  RTWSolverInfo solverInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    time_T stepSize0;
    uint32_T clockTick1;
    SimTimeStep simTimeStep;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block parameters (default storage) */
extern P_delay_T delay_P;

/* Block states (default storage) */
extern DW_delay_T delay_DW;

/* Model entry point functions */
extern void delay_initialize(void);
extern void delay_step(void);
extern void delay_terminate(void);

/* Real-time Model object */
extern RT_MODEL_delay_T *const delay_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<Root>/Constant' : Unused code path elimination
 * Block '<Root>/Gain' : Unused code path elimination
 * Block '<Root>/Gain1' : Unused code path elimination
 * Block '<Root>/Scope' : Unused code path elimination
 * Block '<Root>/Step' : Unused code path elimination
 * Block '<Root>/Sum' : Unused code path elimination
 * Block '<Root>/x[k-1]' : Unused code path elimination
 * Block '<Root>/x[k]' : Unused code path elimination
 * Block '<Root>/y[k-1]' : Unused code path elimination
 * Block '<Root>/y[k]' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'delay'
 */
#endif                                 /* RTW_HEADER_delay_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
