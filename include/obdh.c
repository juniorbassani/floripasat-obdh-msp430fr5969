/*
 * obdh.c
 *
 * Copyright (C) 2017-2019, Universidade Federal de Santa Catarina.
 *
 * This file is part of FloripaSat-OBDH.
 *
 * FloripaSat-OBDH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the F    int i;
 * ree Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FloripaSat-OBDH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FloripaSat-OBDH.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

 /**
 * \brief Main functions of the OBDH module
 *
 * \author Elder Tramontin
 */

#include "obdh.h"

void create_tasks( void ) {
    /**
     * Create queues to communicate between tasks
     */
    obdh_status_queue          = xQueueCreate( 5, sizeof( satellite_data.obdh_status ) );
    imu_queue                  = xQueueCreate( 5, sizeof( satellite_data.imu ) );
    obdh_misc_queue            = xQueueCreate( 5, sizeof( satellite_data.obdh_misc ) );
    obdh_uptime_queue          = xQueueCreate( 5, sizeof( satellite_data.obdh_uptime ) );
    solar_panels_queue         = xQueueCreate( 5, sizeof( satellite_data.solar_panels_sensors ) );
    main_radio_queue           = xQueueCreate( 1, sizeof( satellite_data.main_radio ) );
    eps_queue                  = xQueueCreate( 5, sizeof( eps_package_t ) );
    ttc_queue                  = xQueueCreate( 1, sizeof( uint8_t ) );
    tx_queue                   = xQueueCreate( 1, sizeof( uint8_t ) );
    payload_rush_queue         = xQueueCreate( 5, sizeof( satellite_data.payload_rush) );
    payload_brave_queue        = xQueueCreate( 5, sizeof( satellite_data.payload_brave) );
    status_eps_queue           = xQueueCreate( 1, sizeof(uint8_t) );
    status_payload_rush_queue  = xQueueCreate( 1, sizeof(uint8_t) );
    command_to_payload_rush_queue = xQueueCreate( 5, sizeof(uint8_t) ); // definir tamanho dessa queue
    status_payload_brave_queue = xQueueCreate( 1, sizeof(uint8_t) );
    status_mem1_queue          = xQueueCreate( 1, sizeof(uint8_t) );
    status_imu_queue           = xQueueCreate( 1, sizeof(uint8_t) );
    eps_charge_queue        = xQueueCreate( 1, sizeof(uint8_t) );

    /**
     * Create the semaphores to synchronize the use of shared resources (mutual exclusion)
     */
    spi1_semaphore   = xSemaphoreCreateMutex();
    i2c0_semaphore   = xSemaphoreCreateMutex();
    fsp_semaphore    = xSemaphoreCreateMutex();
    flash_semaphore  = xSemaphoreCreateMutex();

    /**
     * Create each task: links with a routine, allocates the requested task
     * stack size, sets the priority, passes parameters and get a handler
     */
    xTaskCreate( wdt_task, "WDT", configMINIMAL_STACK_SIZE, NULL, 5, &wdt_task_handle );
    xTaskCreate( communications_task, "Communications", 6 * configMINIMAL_STACK_SIZE, NULL, 5, &communications_task_handle );
    xTaskCreate( store_data_task, "StoreData", 11 * configMINIMAL_STACK_SIZE, NULL , 5, &store_data_task_handle);
    xTaskCreate( housekeeping_task, "Housekeeping", configMINIMAL_STACK_SIZE, NULL, 5, &housekeeping_task_handle);
    xTaskCreate( ttc_interface_task, "TT&C", 4 * configMINIMAL_STACK_SIZE, NULL, 5, &ttc_interface_task_handle );
    //xTaskCreate( eps_interface_task, "EPS", configMINIMAL_STACK_SIZE, NULL, EPS_INTERFACE_TASK_PRIORITY, &eps_interface_task_handle );
    //xTaskCreate( imu_interface_task, "IMU", configMINIMAL_STACK_SIZE, NULL, IMU_INTERFACE_TASK_PRIORITY, &imu_interface_task_handle);
    //xTaskCreate( solar_panels_interface_task, "SolarPanels", configMINIMAL_STACK_SIZE, NULL, SOLAR_PANELS_INTERFACE_TASK_PRIORITY, &solar_panels_interface_task_handle);
    xTaskCreate( payload_rush_interface_task, "PayloadRush", configMINIMAL_STACK_SIZE, NULL, 5, &payload_rush_interface_task_handle );
    //xTaskCreate( runtime_stats_task, "RuntimeStats", 125, NULL, RUNTIME_STATS_TASK_PRIORITY, NULL );
#ifdef _DEBUG
    //xTaskCreate( debug_task, "DEBUG", 4 * configMINIMAL_STACK_SIZE, NULL, DEBUG_TASK_PRIORITY, &debug_task_handle);
#endif
}

void gpio_setup() {
    P1OUT &= ~BIT0;
    P4OUT &= ~BIT6;

    P1DIR = BIT0;                  // Set P1.0 to output direction and P1.1 to input
    P4DIR = BIT6;

    rf4463_gpio_init();
}

void uart_setup(void)
{
    // Configure GPIO
    P2SEL1 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    P2SEL0 &= ~(BIT0 | BIT1);
    PJSEL0 |= BIT4 | BIT5;                    // For XT1

    // XT1 Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL4 &= ~LFXTOFF;

    do {
        CSCTL5 &= ~LFXTOFFG;                    // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__ACLK;                // CLK = ACLK
    UCA0BR0 = 3;                              // 9600 baud
    UCA0MCTLW |= 0x5300;                      // 32768/9600 - INT(32768/9600)=0.41
                                            // UCBRSx value = 0x53 (See UG)
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // Initialize eUSCI
}

void clock_setup(void)
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                    // Unlock CS registers
    CSCTL1 = DCORSEL | DCOFSEL_4;             // Set DCO to 16MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // Set SMCLK = MCLK = DCO,
                                              // ACLK = VLOCLK
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers
}

void setup_hardware( void ) {
    //uint8_t test_result;

    taskDISABLE_INTERRUPTS();

    gpio_reset();

    /**
     *  Configure and reset the watchdog timers
     */
    wdti_setup(WATCHDOG, WD_16_SEC);
    //WDTCTL = WDTPW | WDTHOLD;

    clock_setup();

    //wdte_setup();
    //wdte_reset_counter();
//
    //test_result = clocks_setup();   /**< Setup clocks                                                       */
//
    //uart0_setup(9600);              /**< Setup UART                                                         */
//
    ///*
     //* Print some booting messages
     //*/
    //debug(BOOTING_MSG);
    //debug(UART_INFO_MSG);
    //if(test_result == TEST_SUCESS) {
        //debug(CLOCK_INFO_MSG);
    //}
    //else {
        //debug(CLOCK_FAIL_MSG);
    //}
//
    ///*
     //* Setup I2C interfaces 0, 1 and 2
     //*/
    i2c_setup(0);
    //i2c_setup(1);
    //i2c_setup(2);
//
    //debug(I2C_INFO_MSG);            /**< Setup I2C                                                          */
//
    /*
     * Setup SPI interfaces 0 and 1
     */
    spi_setup(0);
    spi_setup(1);

    uart_setup();
//
    //debug(SPI_INF_MSG);             /**< Setup SPI                                                          */
//
    //adc_setup();                    /**< Setup ADC                                                          */
//
    //debug(ADC_INFO_MSG);
//
    //sspi_setup();                   /**< Setup software SPI                                                 */
//
    gpio_setup();                   /**< Setup all GPIO pins according each function                        */
//
    update_reset_value();           /**< Read the previous value, increment it and store again              */
    restore_time_counter();         /**< Read the time counter after a reset and restore it value to RAM    */

    /* Disable the GPIO power-on default high-impedance mode. */
    PM5CTL0 &= ~LOCKLPM5;

    //debug("\n --- Boot completed ---\n");
}

void vConfigureTimerForRunTimeStats( void )
{
    /* Configure a timer that is used as the time base for run time stats.  See
    http://www.freertos.org/rtos-run-time-stats.html */

    /* Ensure the timer is stopped. */
    TA1CTL = 0;

    /* Start up clean. */
    TA1CTL |= TACLR;

    /* Run the timer from the ACLK/8, continuous mode, interrupt enable. */
    TA1CTL = TASSEL_1 | ID__8 | MC__CONTINUOUS | TAIE;
}

void hibernate(void) {
    //uint8_t seconds_counter = 0;
//
    //start_timer_b();                    /**< configure and start counting time over the timer B */
//
    //do {
        ///*
         //*  Reset Watchdog timers
         //*/
        //wdte_reset_counter();
        //wdti_reset_counter();
//
        //low_power_mode_sleep();         /**< Enter in Low-power mode */
        ///**< Wake-up after a interrupt event */
//
        ///*
         //* Count 1 minute and store the value in a flash memory
         //*/
        //if(seconds_counter++ == 60) {
            //seconds_counter = 0;
            //update_time_counter();
        //}
//
    //} while (read_time_counter() < MINUTES_BEFORE_DEPLOY_ANTENNAS); /**< loop until reach 45 minutes */
//
    //stop_timer_b();                     /**< stop counting time over the timer B */
}

void reset_memory(void) {
    //flash_erase(SEGD_ADDR);
    //flash_erase(SEGC_ADDR);
    //flash_erase(SEGB_ADDR);
    //flash_erase(SEGA_ADDR);
}

void vApplicationTickHook( void ) {
    /*
     * Tick hook functions execute within the context of the tick interrupt so must be kept very short,
     *  use only a moderate amount of stack space,
     *  and not call any FreeRTOS API functions whose name does not end with ‘FromISR()’.
     */
    static unsigned long ulCounter = 0;

    /* Is it time to toggle the LED again? */
    ulCounter++;

    if( ( ulCounter & 0xff ) == 0 ) {
        /* Once in 256 tick counts, do that */
    }
}
/*-----------------------------------------------------------*/

/* The MSP430X port uses this callback function to configure its tick interrupt.
This allows the application to choose the tick interrupt source.
configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
interrupt vector for the chosen tick interrupt source.  This implementation of
vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
void vApplicationSetupTimerInterrupt( void )
{
const unsigned short usACLK_Frequency_Hz = 32768;

	/* Ensure the timer is stopped. */
	TA0CTL = 0;

	/* Run the timer from the ACLK. */
	TA0CTL = TASSEL_1;

	/* Clear everything to start with. */
	TA0CTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

	/* Enable the interrupts. */
	TA0CCTL0 = CCIE;

	/* Start up clean. */
	TA0CTL |= TACLR;

	/* Up mode. */
	TA0CTL |= MC_1;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void ) {
    /* Called on each iteration of the idle task.  In this case the idle task
    just enters a low(ish) power mode. */
    __bis_SR_register( LPM1_bits + GIE );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void ) {
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues or
    semaphores. */
    taskDISABLE_INTERRUPTS();
    for( ;; );

}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) {
    ( void ) pxTask;
    ( void ) pcTaskName;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void v4RunTimeStatsTimerOverflow( void )
{
    TA1CTL &= ~TAIFG;

    /* 16-bit overflow, so add 17th bit. */
    ulRunTimeCounterOverflows += 0x10000;
    __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
}

/*-----------------------------------------------------------*/

//int _system_pre_init( void )
//{
//    /* Stop Watchdog timer. */
//    WDTCTL = WDTPW + WDTHOLD;

    ///* Return 1 for segments to be initialised. */
    //return 1;
//}
