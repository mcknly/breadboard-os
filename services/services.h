/******************************************************************************
 * @file services.h
 *
 * @brief Defines all of the services available on the system. The individual
 *        service function implementations and FreeRTOS APIs for registering the
 *        associated tasks are contained in the source files for the respective
 *        services. Queues used to pass data between services are implemented
 *        in service_queues.h.
 *
 * @author Cavin McKinley (MCKNLY LLC)
 *
 * @date 02-14-2024
 *
 * @copyright Copyright (c) 2024 Cavin McKinley (MCKNLY LLC)
 *            Released under the MIT License
 *
 * SPDX-License-Identifier: MIT
 ******************************************************************************/

#ifndef SERVICES_H
#define SERVICES_H

#include <stdbool.h>
#include "hardware_config.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "lfs.h"


/*******************************************************************************
 * Services - Dynamic tasks to run in FreeRTOS.
 *            Services can be launched at boot or started/suspended/stopped at
 *            any time via 'taskmanager', the base service.
 *
 *            If a new service is added, a SERVICE_NAME must be defined for it,
 *            and the service implementation and FreeRTOS task launcher must be
 *            created in a separate source file (see "heartbeat_service.c" for
 *            an example). The service's "descriptor" then needs to be added
 *            to the service_descriptors[] array in services.c, which associates
 *            the service's main function pointer with its name string, and
 *            defines whether the service should run at boot.
 *
 *            The service schedule within FreeRTOS is determined by 3 parameters:
 *            Priority, Repeat, and Delay. Upon any given scheduler tick, the OS
 *            will run the task (service) with the highest priority. After a task
 *            runs, it will check if it should immediately run again based on the
 *            REPEAT parameter; if it has already repeated the specified number
 *            of times it will block itself for the specified number of ticks
 *            based on the DELAY parameter. This essentially means a task's run
 *            percentage (within a priority level) is given by REPEAT/DELAY.
 *
 *            Pro tip: uncomment add_definitions(-DSCHED_TEST_DELAY) in the
 *            top-level CMakeLists.txt to burn extra cycles in each service, and
 *            then use the 'bin/top' CLI command to show FreeRTOS task runtime
 *            percentages to help tune the scheduler! Don't forget to comment
 *            this back out after testing!
 *
 *            Note that by default, 1 OS tick is 1 ms.
 *            This can be changed in FreeRTOSConfig.h, see 'configTICK_RATE_HZ'
*******************************************************************************/

// stringify helper - use xstr() to convert #define to a usable string
#define str(s) # s
#define xstr(s) str(s)

// service names for spawning and interacting with rtos tasks - no quotes around them.
// note - max name length is 15 characters, defined by configMAX_TASK_NAME_LEN in FreeRTOS.h
#define SERVICE_NAME_TASKMAN    taskmanager
#define SERVICE_NAME_CLI        cli
#define SERVICE_NAME_USB        usb
#define SERVICE_NAME_STORMAN    storagemanager
#define SERVICE_NAME_WATCHDOG   watchdog
#define SERVICE_NAME_HEARTBEAT  heartbeat

// freertos task priorities for the services.
// as long as configUSE_TIME_SLICING is set, equal priority tasks will share time.
#define PRIORITY_TASKMAN   1
#define PRIORITY_CLI       1
#define PRIORITY_USB       2
#define PRIORITY_STORMAN   3
#define PRIORITY_WATCHDOG  1
#define PRIORITY_HEARTBEAT 1

// number of sequential time slices to run each service before beginning the
// delay interval set below. If a service should run most of the time, set REPEAT
// to a higher number and DELAY to a lower number.
#define REPEAT_TASKMAN      1
#define REPEAT_CLI          1
#define REPEAT_USB          1
#define REPEAT_STORMAN      1
#define REPEAT_WATCHDOG     1
#define REPEAT_HEARTBEAT    1

// OS ticks to block after each execution of a service (sets max execution interval).
// higher priority services should include some delay time to allow lower priority
// tasks to execute. Since the delay happens at the end of the service loop, the
// service should finish its current work before blocking itself with the delay.
// Note that if a service has delay of 0 and priority is > 0, the IDLE task will
// always be blocked and FreeRTOS will not be able to perform task cleanup (i.e. freeing RAM).
#define DELAY_TASKMAN      20
#define DELAY_CLI          1     // CLI delay could be increased at the expense of character I/O responsiveness
#define DELAY_USB          5
#define DELAY_STORMAN      100
#define DELAY_WATCHDOG     100
#define DELAY_HEARTBEAT    5000  // Example heartbeat service "beats" every 5 seconds when started

// FreeRTOS stack sizes for the services - "stack" in this sense is dedicated heap memory for a task.
// local variables within a service/task use this stack space.
// If pvPortMalloc is called within a task, it will allocate directly from shared FreeRTOS heap.
// Use 'bin/ps' command to show a service's min stack (memory usage high water mark)
// to determine if too much/too little has been allocated.
#define STACK_TASKMAN   512
#define STACK_CLI       1024
#define STACK_USB       1024
#define STACK_STORMAN   1024
#define STACK_WATCHDOG  configMINIMAL_STACK_SIZE // 256 by default
#define STACK_HEARTBEAT configMINIMAL_STACK_SIZE


/************************
 * Service Functions
*************************/

// below are functions for launching the services.
// pointers to these are added to a service's .service_func descriptor item in
// services.c

/**
* @brief Start the taskmanager service.
*
* The taskmanager service is responsible for dynamically starting, suspending,
* stopping, and resuming services (FreeRTOS tasks). It does this by receiving a
* service name and action to perform from the taskmanager queue.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t taskman_service(void);

/**
* @brief Start the CLI service.
*
* The CLI (Command Line Interface) service is an instance of microshell that
* provides the ability to interact with the system, run routines, and see text
* output using a serial terminal connected to a UART (or USB) on the MCU. It has
* additional wrappers and a printing queue which allows other services to print
* strings out from the CLI while remaining thread-safe.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t cli_service(void);

/**
* @brief Start the USB service.
*
* The USB service manages the data pipe to/from the USB controller, operating in
* device mode, via the TinyUSB library. It is intended to run continuously at a
* low priority, checking for available TX/RX data in a non-blocking fashion.
* Data links between other tasks and USB endpoints are facilitated via data queues.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t usb_service(void);

/**
* @brief Start the storagemanager service.
*
* The storagemanager service is responsible for maintaining the filesystem(s) in
* onboard flash that can be used to store persistent settings, logs, etc. It does
* this by implementing littlefs instances. All data transport to/from flash is
* done via the storagemanager queue.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t storman_service(void);

/**
* @brief Start the watchdog service.
*
* The watchdog service enables the system watchdog timer, and then resets the
* timer periodically, preventing the timer from expiring and resetting the MCU.
* If for some reason we are stuck in the weeds somewhere and the watchdog
* cannot be serviced, a system reset will hopefully restore sanity.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t watchdog_service(void);

/**
* @brief Start the heartbeat service.
*
* The heartbeat service is an example service which demonstrates the ability to
* dynamically start a task, and suspend/resume/kill it with the taskmanager.
* It also demonstrates the usage of the CLI printing queue by placing a string
* to print in the queue every 5 seconds (the heartbeat message) - 12 bpm - effectively
* simulating the heartbeat of a blue whale.
*
* @param none
*
* @return 32-bit integer corresponding to FreeRTOS return status defined in projdefs.h
*/
BaseType_t heartbeat_service(void);


BaseType_t wifi_service(void);

/************************
 * Service Descriptors
*************************/

// service function pointer typedef
typedef BaseType_t (*service_func_t)(void);

// service descriptor structure to hold the service name, service function pointer and startup flag
typedef struct service_desc_t {
    // string version of the service name, used when comparing against user input
    const char * const name;

    //Defines wether or not this service should be automatically launched by taskmanager at boot
    const bool startup;

    //Function pointer to the service that creates the respective FreeRTOS task - declared in services.h
    service_func_t service_func;
} service_desc_t;

// holds all the services that can be launched with taskmanager.
// these can be in any order, the corresponding FreeRTOS tasks will be launched in this order.
// note: taskmanager itself is not in this array (it is a base service).
// edit services.c to add your own services!
extern const service_desc_t service_descriptors[];

// number of service descriptors in the array is used to iterate through services
// for startup and interaction
extern const size_t service_descriptors_length;


#endif /* SERVICES_H */
