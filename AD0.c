/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : AD0.c
**     Project   : Project
**     Processor : DFCVT305
**     Component : ADC
**     Version   : Component 01.630, Driver 01.19, CPU db: 3.00.036
**     Compiler  : CodeWarrior HCS12X C Compiler
**     Date/Time : 2016/8/16, 0:25
**     Abstract  :
**         This device "ADC" implements an A/D converter,
**         its control methods and interrupt/event handling procedure.
**     Settings  :
**          Component name                                 : AD0
**          A/D converter                                  : ADC0
**          Sharing                                        : Disabled
**          Interrupt service/event                        : Disabled
**          A/D channels                                   : 16
**            Channel0                                     : 
**              A/D channel (pin)                          : PAD00_AN00
**              A/D channel (pin) signal                   : AN00_KL15STAT
**            Channel1                                     : 
**              A/D channel (pin)                          : PAD01_AN01
**              A/D channel (pin) signal                   : AN01_5V3_AN
**            Channel2                                     : 
**              A/D channel (pin)                          : PAD02_AN02
**              A/D channel (pin) signal                   : AN02_5V1_AN
**            Channel3                                     : 
**              A/D channel (pin)                          : PAD03_AN03
**              A/D channel (pin) signal                   : AN03_START_AN
**            Channel4                                     : 
**              A/D channel (pin)                          : PAD04_AN04
**              A/D channel (pin) signal                   : AN04_M12A_IB2
**            Channel5                                     : 
**              A/D channel (pin)                          : PAD05_AN05
**              A/D channel (pin) signal                   : AN05_M12B_IB1
**            Channel6                                     : 
**              A/D channel (pin)                          : PAD06_AN06
**              A/D channel (pin) signal                   : AN06_IDID3_AN
**            Channel7                                     : 
**              A/D channel (pin)                          : PAD07_AN07
**              A/D channel (pin) signal                   : AN07_UBR_AN
**            Channel8                                     : 
**              A/D channel (pin)                          : PAD08_AN08
**              A/D channel (pin) signal                   : AN08_5V4_AN
**            Channel9                                     : 
**              A/D channel (pin)                          : PAD09_AN09
**              A/D channel (pin) signal                   : 
**            Channel10                                    : 
**              A/D channel (pin)                          : PAD10_AN10
**              A/D channel (pin) signal                   : 
**            Channel11                                    : 
**              A/D channel (pin)                          : PAD11_AN11
**              A/D channel (pin) signal                   : 
**            Channel12                                    : 
**              A/D channel (pin)                          : PAD12_AN12
**              A/D channel (pin) signal                   : 
**            Channel13                                    : 
**              A/D channel (pin)                          : PAD13_AN13
**              A/D channel (pin) signal                   : 
**            Channel14                                    : 
**              A/D channel (pin)                          : PAD14_AN14
**              A/D channel (pin) signal                   : 
**            Channel15                                    : 
**              A/D channel (pin)                          : PAD15_AN15
**              A/D channel (pin) signal                   : 
**          A/D resolution                                 : 12 bits
**          Conversion time                                : 1.600 µs
**          Sample time                                    : 4 clock periods
**          Discharge before sampling                      : Enabled
**          Result mode                                    : Right justified
**          Autoscan mode                                  : Disabled
**          AD conversions in STOP mode                    : Disabled
**          External trigger                               : Disabled
**          Internal trigger                               : Disabled
**          Number of conversions                          : 1
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Events enabled in init.                      : yes
**          CPU clock/speed selection                      : 
**            High speed mode                              : This component enabled
**            Low speed mode                               : This component disabled
**            Slow speed mode                              : This component disabled
**          High input limit                               : 1
**          Low input limit                                : 0
**          Get value directly                             : yes
**          Wait for result                                : yes
**     Contents  :
**         MeasureChan  - byte AD0_MeasureChan(bool WaitForResult, byte Channel);
**         GetChanValue - byte AD0_GetChanValue(byte Channel, void* Value);
**
**     Copyright : 1997 - 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/


/* MODULE AD0. */

#pragma MESSAGE DISABLE C5703          /* Disable warning C5703 "Parameter is not referenced" */
#pragma MESSAGE DISABLE C4002          /* Disable warning C4002 "Result not used is ignored" */
#pragma MESSAGE DISABLE C12056         /* Disable warning C12056 "SP debug info incorrect because of optimization or inline assembler" */

#include "AD0.h"

#pragma DATA_SEG AD0_DATA              /* Select data segment "AD0_DATA" */
#pragma CODE_SEG AD0_CODE
#pragma CONST_SEG AD0_CONST            /* Constant section for this module */

#define STOP            0U             /* STOP state           */
#define MEASURE         1U             /* MESURE state         */
#define CONTINUOUS      2U             /* CONTINUOUS state      */
#define SINGLE          3U             /* SINGLE state         */

static const word Table[] = {          /* Table of mask constants */
0x01U,0x02U,0x04U,0x08U,0x10U,0x20U,0x40U,0x80U,0x0100U,0x0200U,0x0400U,0x0800U,0x1000U,0x2000U,0x4000U,0x8000U};
static const byte Channels[] = {       /* Contents for the device control register */
0x00U,0x01U,0x02U,0x03U,0x04U,0x05U,0x06U,0x07U,0x08U,0x09U,0x0AU,0x0BU,0x0CU,0x0DU,0x0EU,0x0FU};
static word OutFlg;                    /* Measurement finish flag */
static byte SumChan;                   /* Number of measured channels */
volatile static byte ModeFlg;          /* Current state of device */
static word AD0_OutV[16];              /* Array of measured values */
/*
** ===================================================================
**     Method      :  AD0_MainMeasure (component ADC)
**
**     Description :
**         The method performs the conversion of the input channels in 
**         the polling mode.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void AD0_MainMeasure(void)
{
  if (ATD0STAT0_SCF) {                 /* Is a sequence completed? */
    AD0_OutV[SumChan] = ATD0DR0;       /* Save measured value */
    OutFlg |= Table[SumChan];          /* Value of measured channel is available */
    ModeFlg = STOP;                    /* Set the device to the stop mode */
  }
}

/*
** ===================================================================
**     Method      :  AD0_MeasureChan (component ADC)
**
**     Description :
**         This method performs measurement on one channel. (Note: If
**         the <number of conversions> is more than one the conversion
**         of the A/D channel is performed specified number of times.)
**     Parameters  :
**         NAME            - DESCRIPTION
**         WaitForResult   - Wait for a result of
**                           conversion. If the <interrupt service> is
**                           disabled and at the same time a <number of
**                           conversions> is greater than 1, the
**                           WaitForResult parameter is ignored and the
**                           method waits for each result every time.
**         Channel         - Channel number. If only one
**                           channel in the component is set this
**                           parameter is ignored, because the parameter
**                           is set inside this method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED - Device is disabled
**                           ERR_BUSY - A conversion is already running
**                           ERR_RANGE - Parameter "Channel" out of range
** ===================================================================
*/
byte AD0_MeasureChan(bool WaitForResult,byte Channel)
{
  if (Channel >= 16U) {                /* Is channel number greater than or equal to 16 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if (ModeFlg != STOP) {               /* Is the device in different mode than "stop"? */
    return ERR_BUSY;                   /* If yes then error */
  }
  ModeFlg = SINGLE;                    /* Set state of device to the measure mode */
  SumChan = Channel;                   /* Set required channel */
  OutFlg &= (word)(~(word)Table[SumChan]); /* Output value isn't available */
  ATD0CTL5 = Channels[SumChan];        /* Start the conversion */
  if (WaitForResult) {                 /* Is WaitForResult TRUE? */
    while (ModeFlg == SINGLE) {        /* Calling MainMeasure while IDLE state occured */
      AD0_MainMeasure();               /* A/D converter handler */
    }
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD0_GetChanValue (component ADC)
**
**     Description :
**         Returns the last measured value of the required channel.
**         Format and width of the value is a native format of the A/D
**         converter.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Channel         - Channel number. If only one
**                           channel in the component is set then this
**                           parameter is ignored.
**       * Value           - Pointer to the measured value. Data
**                           type is a byte, a word or an int. It
**                           depends on the supported modes, resolution,
**                           etc. of the AD converter. See the Version
**                           specific information for the current CPU in
**                           <General Info>.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_NOTAVAIL - Requested value not
**                           available
**                           ERR_RANGE - Parameter "Channel" out of
**                           range
**                           ERR_OVERRUN - External trigger overrun flag
**                           was detected after the last value(s) was
**                           obtained (for example by GetValue). This
**                           error may not be supported on some CPUs
**                           (see generated code).
** ===================================================================
*/
byte AD0_GetChanValue(byte Channel,void* Value)
{
  if (Channel >= 16U) {                /* Is channel number greater than or equal to 16 */
    return ERR_RANGE;                  /* If yes then error */
  }
  if (ModeFlg != STOP) {               /* Is the device in any measure mode? */
    AD0_MainMeasure();
  }
  if ((OutFlg & Table[Channel]) == 0U) { /* Is measured value(s) available? */
    return ERR_NOTAVAIL;
  }
  *((word*)Value)= AD0_OutV[Channel];  /* Save measured values to the output buffer */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AD0_Init (component ADC)
**
**     Description :
**         Initializes the associated peripheral(s) and the component's 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void AD0_Init(void)
{
  OutFlg = 0U;                         /* No measured value */
  ModeFlg = STOP;                      /* Device isn't running */
  /* ATD0CTL4: SMP2=0,SMP1=0,SMP0=0,PRS4=0,PRS3=0,PRS2=0,PRS1=0,PRS0=1 */
  ATD0CTL4 = 0x01U;                    /* Set sample time and prescaler */
  /* ATD0CTL3: DJM=1,S8C=0,S4C=0,S2C=0,S1C=1,FIFO=0,FRZ1=0,FRZ0=0 */
  ATD0CTL3 = 0x88U;                    /* Set ATD control register 3 */
  /* ATD0CTL0: ??=0,??=0,??=0,??=0,WRAP3=1,WRAP2=1,WRAP1=1,WRAP0=1 */
  ATD0CTL0 = 0x0FU;                    /* Set wrap around */
  /* ATD0CTL1: ETRIGSEL=0,SRES1=1,SRES0=0,SMP_DIS=1,ETRIGCH3=1,ETRIGCH2=1,ETRIGCH1=1,ETRIGCH0=1 */
  ATD0CTL1 = 0x5FU;                    /* Set resolution and discharge */
  /* ATD0CTL2: ??=0,AFFC=1,ICLKSTP=0,ETRIGLE=0,ETRIGP=0,ETRIGE=0,ASCIE=0,ACMPIE=0 */
  ATD0CTL2 = 0x40U;                    /* Set ATD control register 2 */
}

/* END AD0. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.05 [04.46]
**     for the Freescale HCS12X series of microcontrollers.
**
** ###################################################################
*/
