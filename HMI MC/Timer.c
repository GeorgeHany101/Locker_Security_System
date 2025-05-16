#include "Timer.h"
#include "common_macros.h"
#include <avr/io.h> /* To use the UART Registers */
#include <avr/interrupt.h>


/* Global variables to store the address of callback functions */
static volatile void (*g_timer0CallbackPtr)(void) = NULL_PTR;
static volatile void (*g_timer1CallbackPtr)(void) = NULL_PTR;
static volatile void (*g_timer2CallbackPtr)(void) = NULL_PTR;

/* ISR Definitions */
ISR(TIMER0_OVF_vect)
{
    if(g_timer0CallbackPtr != NULL_PTR)
    {
        (*g_timer0CallbackPtr)();
    }
}

ISR(TIMER0_COMP_vect)
{
    if(g_timer0CallbackPtr != NULL_PTR)
    {
        (*g_timer0CallbackPtr)();
    }
}

ISR(TIMER1_OVF_vect)
{
    if(g_timer1CallbackPtr != NULL_PTR)
    {
        (*g_timer1CallbackPtr)();
    }
}

ISR(TIMER1_COMPA_vect)
{
    if(g_timer1CallbackPtr != NULL_PTR)
    {
        (*g_timer1CallbackPtr)();
    }
}

ISR(TIMER2_OVF_vect)
{
    if(g_timer2CallbackPtr != NULL_PTR)
    {
        (*g_timer2CallbackPtr)();
    }
}

ISR(TIMER2_COMP_vect)
{
    if(g_timer2CallbackPtr != NULL_PTR)
    {
        (*g_timer2CallbackPtr)();
    }
}

void Timer_init(const Timer_ConfigType * Config_Ptr)
{
    switch(Config_Ptr->timer_ID)
    {
        case TIMER0_ID:
            /* Set initial value */
            TCNT0 = Config_Ptr->timer_initialValue;
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                OCR0 = Config_Ptr->timer_compareMatchValue; /* Set compare match value */
                TCCR0 = (1<<WGM01); /* Set to CTC mode */
            }
            /* Set clock prescaler */
            TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->timer_clock);
            /* Enable interrupt */
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                SET_BIT(TIMSK, OCIE0); /* Enable Timer0 Compare Interrupt */
            } else {
                SET_BIT(TIMSK, TOIE0); /* Enable Timer0 Overflow Interrupt */
            }
            break;
        case TIMER1_ID:
            /* Set initial value */
            TCNT1 = Config_Ptr->timer_initialValue;
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                OCR1A = Config_Ptr->timer_compareMatchValue;
                TCCR1A = (1<<WGM12); /* Set to CTC mode */
            }
            TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->timer_clock);
            /* Enable interrupt */
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                SET_BIT(TIMSK, OCIE1A);
            } else {
                SET_BIT(TIMSK, TOIE1);
            }
            break;
        case TIMER2_ID:
            TCNT2 = Config_Ptr->timer_initialValue;
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                OCR2 = Config_Ptr->timer_compareMatchValue;
                TCCR2 = (1<<WGM21);
            }
            TCCR2 = (TCCR2 & 0xF8) | (Config_Ptr->timer_clock);
            if (Config_Ptr->timer_mode == TIMER_COMPARE_MODE) {
                SET_BIT(TIMSK, OCIE2);
            } else {
                SET_BIT(TIMSK, TOIE2);
            }
            break;
    }
}

void Timer_deInit(uint8 timer_ID)
{
    switch(timer_ID)
    {
        case TIMER0_ID:
            TCCR0 = 0;
            CLEAR_BIT(TIMSK, OCIE0);
            CLEAR_BIT(TIMSK, TOIE0);
            break;
        case TIMER1_ID:
            TCCR1A = 0;
            TCCR1B = 0;
            CLEAR_BIT(TIMSK, OCIE1A);
            CLEAR_BIT(TIMSK, TOIE1);
            break;
        case TIMER2_ID:
            TCCR2 = 0;
            CLEAR_BIT(TIMSK, OCIE2);
            CLEAR_BIT(TIMSK, TOIE2);
            break;
    }
}

void Timer_setCallBack(void(*a_ptr)(void), uint8 timer_ID)
{
    switch(timer_ID)
    {
        case TIMER0_ID:
            g_timer0CallbackPtr = a_ptr;
            break;
        case TIMER1_ID:
            g_timer1CallbackPtr = a_ptr;
            break;
        case TIMER2_ID:
            g_timer2CallbackPtr = a_ptr;
            break;
    }
}

