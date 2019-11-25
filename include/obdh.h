/*
 * obdh.h
 *
 * Copyright (C) 2017-2019, Universidade Federal de Santa Catarina.
 *
 * This file is part of FloripaSat-OBDH.
 *
 * FloripaSat-OBDH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef INCLUDE_OBDH_H_
#define INCLUDE_OBDH_H_

#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../hal/obdh_hal.h"
//#include "../driver/clocks.h"
#include "../driver/adc.h"
//#include "../driver/uart.h"
#include "../driver/i2c.h"
#include "../driver/spi.h"
//#include "../driver/timer_b.h"
#include "../driver/wdti.h"
//#include "../interface/wdte.h"
#include "../interface/eps.h"
//#include "../include/antenna.h"
#include "../src/task_queues.h"
#include "../src/communications_task.h"
//#include "../src/debug_task.h"
//#include "../src/eps_interface_task.h"
#include "../src/housekeeping_task.h"
//#include "../src/imu_interface_task.h"
#include "../src/payload_rush_interface_task.h"
#include "../src/store_data_task.h"
//#include "../src/solar_panels_interface_task.h"
#include "../src/ttc_interface_task.h"
#include "../src/wdt_task.h"
//#include "../src/runtime_stats_task.h"
//#include "../rf4463/rf4463.h"

/**
 * \brief Instantiation of the OS tasks
 *
 * \return None
 */
void create_tasks(void);

/**
 * \brief Initialization of peripheral modules: internal WDT, external WDT, Clocks, I2C, SPI, UART, ADC.
 *
 * \return None
 */
void setup_hardware(void);

/**
 * \brief Put the MCU in a low power state during 45 minutes. Wake-up each seconds and count the elapsed time.
 *
 * \return None
 */
void hibernate(void);

/**
 * \brief Reset the non-volatile memory.
 *
 * \return None.
 */
void reset_memory(void);

#endif // INCLUDE_OBDH_H_
