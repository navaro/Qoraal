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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal/common/mlog.h"

#ifndef CFG_OS_STATIC_DECLARATIONS
#error "CFG_OS_CHIBIOS, CFG_OS_FREERTOS, CFG_OS_THREADX or CFG_OS_POSIX must be defined when compiling qoraal"
#endif 

const QORAAL_CFG_T * _qoraal_instance = 0 ;

int32_t
qoraal_instance_init (const QORAAL_CFG_T * instance)
{
    if (!_qoraal_instance) {
        _qoraal_instance = instance;

        if (!_qoraal_instance->malloc) qoraal_debug_assert ("qoraal malloc required") ;
        if (!_qoraal_instance->free) qoraal_debug_assert ("qoraal free required") ;

    }
    return EOK;
}


int32_t
qoraal_svc_init (SVC_SERVICE_T * list)
{
    if (!_qoraal_instance) {
       return E_UNEXP ;

    }
    
    svc_events_init () ;
    svc_tasks_init (0) ;
    svc_wdt_init () ;
    svc_logger_init (SERVICE_PRIO_QUEUE3) ;
    svc_threads_init () ;
    svc_shell_init () ;
    if (list) {
        svc_service_services_init (list) ;
    }

    return EOK ;
}

int32_t
qoraal_svc_start (void)
{
    if (!_qoraal_instance) {
       return E_UNEXP ;

    }

    svc_events_start () ;
    svc_tasks_start () ;
    svc_wdt_start () ;
    svc_logger_start () ;
    svc_threads_start () ;
    svc_shell_start () ;
    svc_service_services_start (SVC_SERVICE_FLAGS_AUTOSTART) ;

    return EOK ;
}

