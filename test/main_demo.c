#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "main.h"

static int32_t      qshell_demo_threads (SVC_SHELL_IF_T * pif, char** argv, int argc) ;

SVC_SHELL_CMD_LIST_START(demo, QORAAL_SERVICE_DEMO)
SVC_SHELL_CMD_LIST( "demo_threads", qshell_demo_threads,  "")
SVC_SHELL_CMD_LIST_END()

/**
 * @brief       demo_service_ctrl
 * @details
 * @note
 * @param[in] code
 * @param[in] arg
 * @return              status
 *
 * @power
 */
int32_t
demo_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        break ;

    case SVC_SERVICE_CTRL_START:
        SVC_SHELL_CMD_LIST_INSTALL(demo) ;
        break ;

    case SVC_SERVICE_CTRL_STOP:
        SVC_SHELL_CMD_LIST_UNINSTALL(demo) ;
        break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        res = E_NOIMPL ;
        break ;

    }

    return res ;
}


int32_t
qshell_demo_threads (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res = SVC_SHELL_CMD_E_OK ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD, "OK\r\n") ;

    return res ;
}

//==================================================================================================
//  Test timer
//==================================================================================================
void timer_callback(void *param) {
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "timer - expired '%s'", (char *)param);
}

void test_timer () {


    p_timer_t timer1, timer2, timer3;

    os_timer_create(&timer1, timer_callback, NULL);
    os_timer_create(&timer2, timer_callback, NULL);
    os_timer_create(&timer3, timer_callback, NULL);

    os_timer_set(&timer1, 1000, timer_callback, (void*)"Timer 1 (1 second)");
    os_timer_set(&timer2, 2000, timer_callback, (void*)"Timer 2 (2 seconds)");
    os_timer_set(&timer3, 500, timer_callback, (void*)"Timer 3 (0.5 seconds)");

    os_thread_sleep(2000);

    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "timer - is Timer 1 set? %d\n", os_timer_is_set(&timer1));
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "timer - is Timer 2 set? %d\n", os_timer_is_set(&timer2));
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "timer - is Timer 3 set? %d\n", os_timer_is_set(&timer3));

    os_timer_delete(&timer1);
    os_timer_delete(&timer2);
    os_timer_delete(&timer3);


    return ;
}


//==================================================================================================
//  Test events
//==================================================================================================
static SVC_EVENTS_HANDLER_T _event_handler ;

void events_callback (SVC_EVENTS_T id, void * ctx)
{
    if (id == SVC_EVENTS_USER+1) {
        DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "event - cb '%s'", (char *)ctx);
    }

}

void test_events ()
{

    svc_events_register (SVC_EVENTS_USER+1,  &_event_handler, events_callback, (void*)"Event 1") ;

    svc_events_signal (SVC_EVENTS_USER+1) ;

    os_thread_sleep (100) ;

}


//==================================================================================================
//  Test tasks
//==================================================================================================
static SVC_TASKS_DECL   (_test_task) ;

static void
test_tqask_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    if (reason == SERVICE_CALLBACK_REASON_RUN) {
        DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "task - cb '%s'", (char *)parm);

    }

    svc_tasks_complete (task) ;
}

void test_tasks (void)
{
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "task - schedule...") ;
    svc_tasks_schedule (&_test_task, test_tqask_cb, (uintptr_t)"Task 1",
            SERVICE_PRIO_QUEUE5, SVC_TASK_MS2TICKS(1500)) ;

    os_thread_sleep (1000) ;
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "task - ...") ;
    os_thread_sleep (1000) ;
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "task - ...") ;
    os_thread_sleep (1000) ;
    DBG_MESSAGE_T_LOG(DBG_MESSAGE_SEVERITY_REPORT, 0, "task - ...") ;
}


//==================================================================================================
//  Test threads
//==================================================================================================
static SVC_THREADS_DECL (_test_thread_inst);  

void _test_thread_complete(SVC_THREADS_T * service_thread, void* arg)
{
    DBG_MESSAGE_T_LOG (DBG_MESSAGE_SEVERITY_LOG, 0,
            "thread - complete!") ;

}

static void 
_test_thread (void *arg)
{
    DBG_MESSAGE_T_LOG (DBG_MESSAGE_SEVERITY_LOG, 0,
        "thread - started!") ;
    os_thread_sleep (1000) ;
    DBG_MESSAGE_T_LOG (DBG_MESSAGE_SEVERITY_LOG, 0,
        "thread - terminating!") ;

}

void test_threads ()
{
    svc_threads_create (&_test_thread_inst, _test_thread_complete, 1024, OS_THREAD_PRIO_5, _test_thread, (void*)0, "test_thread") ;


}

