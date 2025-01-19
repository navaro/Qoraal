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
#include "services.h"

/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/

/*===========================================================================*/
/* Local Functions                                                           */
/*===========================================================================*/

static void     wait_for_exit (void) ;
static void     qoraal_print (const char *format) ;
static void     qoraal_assert (const char *format) ;

QORAAL_SERVC_LIST_START(_qoraal_services_list)
QORAAL_SERVC_RUN_DECL("system",  system_service_run, system_service_ctrl, 0, 6000, OS_THREAD_PRIO_7, QORAAL_SERVICE_SYSTEM, SVC_SERVICE_FLAGS_AUTOSTART)
QORAAL_SERVC_RUN_DECL("shell",  shell_service_run, shell_service_ctrl, 0, 6000, OS_THREAD_PRIO_7, QORAAL_SERVICE_SHELL, SVC_SERVICE_FLAGS_AUTOSTART)
QORAAL_SERVC_DECL("demo", demo_service_ctrl, 0, QORAAL_SERVICE_DEMO, 0)
QORAAL_SERVC_LIST_END()

/*===========================================================================*/
/* Service Local Variables and Types                                         */
/*===========================================================================*/

static const QORAAL_CFG_T       _qoraal_cfg = { .malloc = malloc, .free = free, .debug_print = qoraal_print, .debug_assert = qoraal_assert, .current_time = 0, .wdt_kick = 0};

/*===========================================================================*/
/* Main entry point                                                          */
/*===========================================================================*/

int 
main (void)
{
 
    qoraal_instance_init (&_qoraal_cfg) ;
    qoraal_svc_init (_qoraal_services_list) ;

    os_sys_start () ;

    qoraal_svc_start () ;

    DBG_MESSAGE_SERVICES (DBG_MESSAGE_SEVERITY_REPORT, "Qoraal system started...") ;

    wait_for_exit () ;
    return 0;
}

void
qoraal_print (const char *format)
{
    printf ("%s", format) ;
}

void
qoraal_assert (const char *format)
{
    printf (format) ;
    abort () ;
}

static OS_SEMAPHORE_DECL    (_main_stop_sem) ;
void status_callback (SVC_SERVICES_T  id, int32_t status)
{
    if (status == SVC_SERVICE_STATUS_STOPPED && id == QORAAL_SERVICE_SHELL) {
        os_sem_signal (&_main_stop_sem) ;
    }
}

void
wait_for_exit (void)
{
    os_sem_init (&_main_stop_sem, 0) ;
    SVC_SERVICE_HANDLER_T  handler ;
    svc_service_register_handler (&handler, status_callback) ;
    os_sem_wait (&_main_stop_sem) ;
    svc_service_unregister_handler (&handler) ;
}
