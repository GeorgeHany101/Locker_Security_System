#ifndef TIMER_H_
#define TIMER_H_

#include"std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define TIMER0_ID  0
#define TIMER1_ID  1
#define TIMER2_ID  2

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

typedef enum {
    TIMER_NORMAL_MODE, TIMER_COMPARE_MODE
} Timer_ModeType;

typedef enum {
    TIMER_NO_CLOCK, TIMER_PRESCALE_1, TIMER_PRESCALE_8, TIMER_PRESCALE_64,
    TIMER_PRESCALE_256, TIMER_PRESCALE_1024
} Timer_ClockType;

typedef struct {
    uint16 timer_initialValue;
    uint16 timer_compareMatchValue; /* Used only in compare mode */
    uint8 timer_ID;                 /* TIMER0, TIMER1, or TIMER2 */
    Timer_ClockType timer_clock;
    Timer_ModeType timer_mode;
} Timer_ConfigType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Description :
 * Function to initialize the Timer driver.
 */
void Timer_init(const Timer_ConfigType * Config_Ptr);

/*
 * Description :
 * Function to disable the Timer.
 */
void Timer_deInit(uint8 timer_ID);

/*
 * Description :
 * Function to set the Call Back function address for the Timer interrupt.
 */
void Timer_setCallBack(void(*a_ptr)(void), uint8 timer_ID);
#endif /* TIMER_H_ */
