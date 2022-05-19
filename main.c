

/**
 * main.c
 */

/**
 * Librerias
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"



/*
 * Variables
 * */

uint8_t RGB = 0;
uint8_t STATUSR = 0;
uint8_t STATUSG = 0;
uint8_t STATUSB = 0;
uint8_t FLGR = 0;
uint8_t FLGG = 0;
uint8_t FLGB = 0;
char ENTRY;
/*
 * FUNCIONES
 * */
extern void Timer0IntHandler(void);
extern void UARTIntHandler(void);
void setup(void);

/*
 * Interrupciones
 * */
void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    if(GPIOPinRead(GPIO_PORTF_BASE, RGB)){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    }
    else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, RGB);
    }
}

int main(void){
    setup();
    while(1){
    }
}

void setup(void){
    IntMasterEnable();//GENERAL INTERRUPTS

    //Clock 40MHz
    SysCtlClockSet(SYSCTL_OSC_MAIN| SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    //CONFIG TMR0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ((SysCtlClockGet())/1) - 1);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    IntEnable(INT_TIMER0A);
    TimerEnable(TIMER0_BASE, TIMER_A);

    //CONFIG UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}
    GPIOPinConfigure(0x00000001); //RX
    GPIOPinConfigure(0x00000401); //TX
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    UARTIntRegister(UART0_BASE, UARTIntHandler);
    UARTEnable(UART0_BASE);
}


void UARTIntHandler(void){
    UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
    ENTRY = UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, ENTRY);
    if(ENTRY == 'r'){
        RGB = 2;
        FLGR = 1;
        FLGG = 0;
        FLGB = 0;
        STATUSR++;
        STATUSG = 0;
        STATUSB = 0;
    }
    if(ENTRY == 'g'){
        RGB = 8;
        FLGR = 0;
        FLGG = 1;
        FLGB = 0;
        STATUSG++;
        STATUSR = 0;
        STATUSB = 0;
    }
    if(ENTRY == 'b'){
        RGB = 4;
        FLGR = 0;
        FLGG = 0;
        FLGB = 1;
        STATUSB++;
        STATUSR = 0;
        STATUSG = 0;
    }
    if(FLGR == 1 && STATUSR == 2){
        RGB = 0;
        STATUSR = 0;
        STATUSG = 0;
        STATUSB = 0;
    }
    if(FLGG == 1 && STATUSG == 2){
        RGB = 0;
        STATUSR = 0;
        STATUSG = 0;
        STATUSB = 0;
    }
    if(FLGB == 1 && STATUSB == 2){
        RGB = 0;
        STATUSR = 0;
        STATUSG = 0;
        STATUSB = 0;
    }
}
