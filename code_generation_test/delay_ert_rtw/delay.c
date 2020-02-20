/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: delay.c
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

#include "delay.h"
#include "delay_private.h"

/* Block states (default storage) */
DW_delay_T delay_DW;

/* Real-time model */
RT_MODEL_delay_T delay_M_;
RT_MODEL_delay_T *const delay_M = &delay_M_;

/* Forward declaration for local functions */
static void delay_SystemCore_release(const codertarget_arduinobase_inter_T *obj);
static void delay_SystemCore_delete(const codertarget_arduinobase_inter_T *obj);
static void matlabCodegenHandle_matlabCodeg(codertarget_arduinobase_inter_T *obj);
static void arduino_AnalogInput_set_pinNumb(codertarget_arduinobase_inter_T *obj,
  f_codertarget_arduinobase_int_T *iobj_0);
static void delay_SystemCore_release(const codertarget_arduinobase_inter_T *obj)
{
  if ((obj->isInitialized == 1L) && obj->isSetupComplete) {
    MW_AnalogIn_Stop(obj->MW_ANALOGIN_HANDLE);
  }
}

static void delay_SystemCore_delete(const codertarget_arduinobase_inter_T *obj)
{
  delay_SystemCore_release(obj);
}

static void matlabCodegenHandle_matlabCodeg(codertarget_arduinobase_inter_T *obj)
{
  if (!obj->matlabCodegenIsDeleted) {
    obj->matlabCodegenIsDeleted = true;
    delay_SystemCore_delete(obj);
  }
}

static void arduino_AnalogInput_set_pinNumb(codertarget_arduinobase_inter_T *obj,
  f_codertarget_arduinobase_int_T *iobj_0)
{
  iobj_0->AvailablePwmPinNames.f1 = '2';
  iobj_0->AvailablePwmPinNames.f2 = '3';
  iobj_0->AvailablePwmPinNames.f3 = '4';
  iobj_0->AvailablePwmPinNames.f4 = '5';
  iobj_0->AvailablePwmPinNames.f5 = '6';
  iobj_0->AvailablePwmPinNames.f6 = '7';
  iobj_0->AvailablePwmPinNames.f7 = '8';
  iobj_0->AvailablePwmPinNames.f8 = '9';
  iobj_0->AvailablePwmPinNames.f9[0] = '1';
  iobj_0->AvailablePwmPinNames.f9[1] = '0';
  iobj_0->AvailablePwmPinNames.f10[0] = '1';
  iobj_0->AvailablePwmPinNames.f10[1] = '1';
  iobj_0->AvailablePwmPinNames.f11[0] = '1';
  iobj_0->AvailablePwmPinNames.f11[1] = '2';
  iobj_0->AvailablePwmPinNames.f12[0] = '1';
  iobj_0->AvailablePwmPinNames.f12[1] = '3';
  obj->Hw = iobj_0;
  obj->PinAnalog = 1U;
}

/* Model step function */
void delay_step(void)
{
  uint16_T tmp;
  codertarget_arduinobase_inter_T *obj;

  /* MATLABSystem: '<Root>/Analog Input' */
  if (delay_DW.obj.SampleTime != delay_P.AnalogInput_SampleTime) {
    delay_DW.obj.SampleTime = delay_P.AnalogInput_SampleTime;
  }

  obj = &delay_DW.obj;
  MW_AnalogIn_Start(delay_DW.obj.MW_ANALOGIN_HANDLE);
  obj->MW_ANALOGIN_HANDLE = MW_AnalogIn_GetHandle((uint32_T)
    delay_DW.obj.PinAnalog);
  MW_AnalogInSingle_ReadResult(delay_DW.obj.MW_ANALOGIN_HANDLE, &tmp, 3);

  /* End of MATLABSystem: '<Root>/Analog Input' */

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   */
  delay_M->Timing.t[0] =
    (++delay_M->Timing.clockTick0) * delay_M->Timing.stepSize0;

  {
    /* Update absolute timer for sample time: [0.02s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The resolution of this integer timer is 0.02, which is the step size
     * of the task. Size of "clockTick1" ensures timer will not overflow during the
     * application lifespan selected.
     */
    delay_M->Timing.clockTick1++;
  }
}

/* Model initialize function */
void delay_initialize(void)
{
  /* Registration code */
  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&delay_M->solverInfo, &delay_M->Timing.simTimeStep);
    rtsiSetTPtr(&delay_M->solverInfo, &rtmGetTPtr(delay_M));
    rtsiSetStepSizePtr(&delay_M->solverInfo, &delay_M->Timing.stepSize0);
    rtsiSetErrorStatusPtr(&delay_M->solverInfo, (&rtmGetErrorStatus(delay_M)));
    rtsiSetRTModelPtr(&delay_M->solverInfo, delay_M);
  }

  rtsiSetSimTimeStep(&delay_M->solverInfo, MAJOR_TIME_STEP);
  rtsiSetSolverName(&delay_M->solverInfo,"FixedStepDiscrete");
  rtmSetTPtr(delay_M, &delay_M->Timing.tArray[0]);
  delay_M->Timing.stepSize0 = 0.02;

  {
    codertarget_arduinobase_inter_T *obj;
    MW_AnalogIn_TriggerSource_Type trigger_val;

    /* Start for MATLABSystem: '<Root>/Analog Input' */
    delay_DW.obj.matlabCodegenIsDeleted = true;
    delay_DW.obj.isInitialized = 0L;
    delay_DW.obj.SampleTime = -1.0;
    delay_DW.obj.matlabCodegenIsDeleted = false;
    arduino_AnalogInput_set_pinNumb(&delay_DW.obj, &delay_DW.gobj_0);
    delay_DW.obj.SampleTime = delay_P.AnalogInput_SampleTime;
    obj = &delay_DW.obj;
    delay_DW.obj.isSetupComplete = false;
    delay_DW.obj.isInitialized = 1L;
    obj->MW_ANALOGIN_HANDLE = MW_AnalogInSingle_Open((uint32_T)
      delay_DW.obj.PinAnalog);
    trigger_val = MW_ANALOGIN_SOFTWARE_TRIGGER;
    MW_AnalogIn_SetTriggerSource(delay_DW.obj.MW_ANALOGIN_HANDLE, trigger_val,
      0UL);
    delay_DW.obj.isSetupComplete = true;
  }
}

/* Model terminate function */
void delay_terminate(void)
{
  /* Terminate for MATLABSystem: '<Root>/Analog Input' */
  matlabCodegenHandle_matlabCodeg(&delay_DW.obj);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
