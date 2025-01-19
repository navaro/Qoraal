/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
    SPDX-License-Identifier: MIT

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

/*===========================================================================*/
#if 0
/*
 * System Initialization and Startup Documentation
 *
 * This section outlines the recommended procedure for starting the system.
 * The initialization sequence ensures the proper configuration of core components
 * and the scheduler before transitioning to runtime operation.
 */

/*
 * First, we define the services that make up the application.
 */
QORAAL_SERVC_LIST_START(_services_list)
QORAAL_SERVC_RUN_DECL("system", system_service_run, system_service_ctrl, 0, 6000, OS_THREAD_PRIO_7, QORAAL_SERVICE_SYSTEM, SVC_SERVICE_FLAGS_AUTOSTART)
QORAAL_SERVC_DECL("demo", demo_service_ctrl, 0, QORAAL_SERVICE_DEMO, 0)
QORAAL_SERVC_LIST_END()


/**
 * @brief Main application thread
 *
 * This thread serves as the entry point for the application logic once the scheduler
 * has been started. The system relies on this function to execute high-level tasks.
 *
 * @param arg Pointer to any arguments passed to the thread (unused in this case).
 */
void main_thread(void* arg)
{
    // Perform all post-scheduler initialization here.
    //

    // This command will start all services listed in _services_list.
    qoraal_svc_start();

    // If the services define the application, you can safely exit 
    // here, and the service threads will clean up the thread resources.
}

/**
 * @brief System Initialization
 *
 * This function performs essential setup operations before starting the scheduler.
 * It initializes core components and spawns the main application thread.
 */
void main_init(void)
{
    static SVC_THREADS_T thd;
    // Configuration for the Qoraal instance
    static const QORAAL_CFG_T qoraal_instance = {
        .malloc = platform_malloc,         // Memory allocation function
        .free = platform_free,             // Memory deallocation function
        .debug_print = platform_debug_print, // Debug message handler
        .debug_assert = platform_debug_assert, // Assertion failure handler
        .current_time = platform_current_time // Platform's current time function
    };

    // Initialize the Qoraal framework with the specified instance and services list
    qoraal_instance_init(&qoraal_instance);
    qoraal_svc_init(_services_list);

    // Perform other pre-scheduler initializations here...

    // Create the main application thread
    svc_threads_create(&thd, 0,
                4000, OS_THREAD_PRIO_12, main_thread, 0, "main_thread");
}

#if defined(CFG_OS_THREADX)
/**
 * @brief ThreadX-specific application definition
 *
 * This function is invoked during ThreadX initialization in os_sys_start().
 *
 */
void tx_application_define(void* first_unused_memory)
{
    main_init();
}
#endif

/**
 * @brief Main function
 *
 * This is the system entry point. It performs any necessary
 * pre-scheduler setup and starts the operating system scheduler. 
 *
 * @return This function does not return under normal circumstances.
 */
int main(void)
{
#if !defined(CFG_OS_THREADX)
    // Initialize the system for FreeRTOS, ChibiOS and POSIX
    main_init();
#endif

    // Start the operating system scheduler
    os_sys_start();

    /* 
     * Depending on the RTOS, if you get here, it should be in a threading context. 
     * Alternatively, main_thread can be called from here.
     */
    for (;;) os_thread_sleep(32768); 
}
#endif
/*===========================================================================*/


#ifndef __QORAAL_H__
#define __QORAAL_H__
#include <stdint.h>
#include <stddef.h>
#include "qoraal/os.h"
#include "qoraal/errordef.h"
#include "qoraal/debug.h"
#include "qoraal/svc/svc_services.h"

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

// Structure to define component dependencies
typedef struct {
    /**
     * @brief Allocate memory.
     * @param size The number of bytes to allocate.
     * @return Pointer to the allocated memory or NULL on failure.
     */
    void *(*malloc) (size_t size);

    /**
     * @brief Free allocated memory.
     * @param ptr Pointer to the memory to free.
     */
    void (*free) (void *ptr);

    /**
     * @brief Print a debug message.
     * @param message The debug string to print.
     */
    void (*debug_print) (const char *message);

    /**
     * @brief Assert with a debug message.
     * @param message The debug string to print.
     */
    void (*debug_assert) (const char *message);

    /**
     * @brief get system time
     * @return Seconds Since Epoch
     */
    uint32_t (*current_time) (void);

    /**
     * @brief Kick the WDT
     * @return Seconds before wdt will reset
     */
    uint32_t (*wdt_kick) (void);

} QORAAL_CFG_T;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern const QORAAL_CFG_T * 	_qoraal_instance ;


#ifdef __cplusplus
extern "C" {
#endif

int32_t     qoraal_instance_init (const QORAAL_CFG_T * instance);
int32_t     qoraal_svc_init (SVC_SERVICE_T * list) ;
int32_t     qoraal_svc_start (void) ;


static inline void * qoraal_malloc (size_t size) {
    if (_qoraal_instance) return _qoraal_instance->malloc (size);
    return 0;
}

static inline void qoraal_free (void *ptr) {
    if (_qoraal_instance) _qoraal_instance->free (ptr);
}

static inline void qoraal_debug_print (const char *message) {
    if (_qoraal_instance && _qoraal_instance->debug_print) _qoraal_instance->debug_print (message);
}

static inline void qoraal_debug_assert (const char *message) {
    if (_qoraal_instance && _qoraal_instance->debug_assert) _qoraal_instance->debug_assert (message);
}

static inline uint32_t qoraal_current_time (void) {
    if (_qoraal_instance && _qoraal_instance->current_time) return _qoraal_instance->current_time ();
    return os_sys_timestamp () / 1000 ;
}

static inline uint32_t qoraal_wdt_kick (void) {
    if (_qoraal_instance && _qoraal_instance->wdt_kick) return _qoraal_instance->wdt_kick ();
    return 0 ;
}

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
#endif /* __QORAAL_H__ */
