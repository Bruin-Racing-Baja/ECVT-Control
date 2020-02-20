/*
 * delay.cpp
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "delay".
 *
 * Model version              : 1.1
 * Simulink Coder version : 9.2 (R2019b) 18-Jul-2019
 * C++ source code generated on : Sat Feb  1 17:04:24 2020
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Atmel->AVR
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#include "delay.h"
#include "delay_private.h"

/* Model step function */
void delayModelClass::step()
{
  /* Outport: '<Root>/Output' incorporates:
   *  Delay: '<Root>/Delay'
   */
  delay_Y.Output = delay_DW.Delay_DSTATE;

  /* Update for Delay: '<Root>/Delay' incorporates:
   *  Inport: '<Root>/Input'
   */
  delay_DW.Delay_DSTATE = delay_U.Input;

  /* Matfile logging */
  rt_UpdateTXYLogVars((&delay_M)->rtwLogInfo, (&(&delay_M)->Timing.taskTime0));

  /* signal main to stop simulation */
  {                                    /* Sample time: [0.02s, 0.0s] */
    if ((rtmGetTFinal((&delay_M))!=-1) &&
        !((rtmGetTFinal((&delay_M))-(&delay_M)->Timing.taskTime0) > (&delay_M)
          ->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus((&delay_M), "Simulation finished");
    }
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++(&delay_M)->Timing.clockTick0)) {
    ++(&delay_M)->Timing.clockTickH0;
  }

  (&delay_M)->Timing.taskTime0 = (&delay_M)->Timing.clockTick0 * (&delay_M)
    ->Timing.stepSize0 + (&delay_M)->Timing.clockTickH0 * (&delay_M)
    ->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
void delayModelClass::initialize()
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));
  rtmSetTFinal((&delay_M), 10.0);
  (&delay_M)->Timing.stepSize0 = 0.02;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    (&delay_M)->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo((&delay_M)->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs((&delay_M)->rtwLogInfo, (NULL));
    rtliSetLogT((&delay_M)->rtwLogInfo, "tout");
    rtliSetLogX((&delay_M)->rtwLogInfo, "");
    rtliSetLogXFinal((&delay_M)->rtwLogInfo, "");
    rtliSetLogVarNameModifier((&delay_M)->rtwLogInfo, "rt_");
    rtliSetLogFormat((&delay_M)->rtwLogInfo, 4);
    rtliSetLogMaxRows((&delay_M)->rtwLogInfo, 0);
    rtliSetLogDecimation((&delay_M)->rtwLogInfo, 1);
    rtliSetLogY((&delay_M)->rtwLogInfo, "");
    rtliSetLogYSignalInfo((&delay_M)->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs((&delay_M)->rtwLogInfo, (NULL));
  }

  /* states (dwork) */
  (void) std::memset(static_cast<void *>(&delay_DW), 0,
                     sizeof(DW_delay_T));

  /* external inputs */
  delay_U.Input = 0.0;

  /* external outputs */
  delay_Y.Output = 0.0;

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime((&delay_M)->rtwLogInfo, 0.0, rtmGetTFinal
    ((&delay_M)), (&delay_M)->Timing.stepSize0, (&rtmGetErrorStatus((&delay_M))));

  /* InitializeConditions for Delay: '<Root>/Delay' */
  delay_DW.Delay_DSTATE = 0.0;
}

/* Model terminate function */
void delayModelClass::terminate()
{
  /* (no terminate code required) */
}

/* Constructor */
delayModelClass::delayModelClass() : delay_M()
{
  /* Currently there is no constructor body generated.*/
}

/* Destructor */
delayModelClass::~delayModelClass()
{
  /* Currently there is no destructor body generated.*/
}

/* Real-Time Model get method */
RT_MODEL_delay_T * delayModelClass::getRTM()
{
  return (&delay_M);
}
