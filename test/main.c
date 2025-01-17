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
#include "main.h"


#define SHELL_VERSION_STR       "Navaro Qoraal Demo v '" __DATE__ "'"
#define SHELL_PROMPT            "# >"


SVC_SHELL_CMD_DECL( "exit", qshell_exit, "");
SVC_SHELL_CMD_DECL( "version", qshell_version, "");

static int32_t  qshell_get_line (char * buffer, uint32_t len) ;
static int32_t  qshell_out (void* ctx, uint32_t out, const char* str);
static void     qoraal_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg) ;
static void     qoraal_print (const char *format) ;
static void     qoraal_assert (const char *format) ;



/*
 * Run until the exit flag is set.
 */
static bool             _shell_exit = false ;




QORAAL_SERVC_LIST_START(_services_list)
QORAAL_SERVC_RUN_DECL("system",  system_service_run, system_service_ctrl, 0, 6000, OS_THREAD_PRIO_7, QORAAL_SERVICE_SYSTEM, SVC_SERVICE_FLAGS_AUTOSTART)
QORAAL_SERVC_DECL("demo", demo_service_ctrl, 0, QORAAL_SERVICE_DEMO, 0)
QORAAL_SERVC_LIST_END()

int main(void)
{
    static const QORAAL_CFG_T       qoraal_cfg = { .malloc = malloc, .free = free, .debug_print = qoraal_print, .debug_assert = qoraal_assert, .current_time = 0, .wdt_kick = 0};
 
    qoraal_instance_init (&qoraal_cfg) ;
    qoraal_svc_init (_services_list) ;

    os_sys_start () ;

    qoraal_svc_start () ;

    static LOGGER_CHANNEL_T     log_channel = { .fp = qoraal_logger_cb, .user = (void*)0, .filter = { { .mask = SVC_LOGGER_MASK, .type = SVC_LOGGER_SEVERITY_LOG | SVC_LOGGER_FLAGS_PROGRESS }, {0,0} } };
    SVC_SHELL_IF_T              qshell_if ;
    svc_logger_channel_add (&log_channel) ;
    svc_shell_if_init (&qshell_if, 0, qshell_out, 0) ;
    DBG_MESSAGE_DEMO (DBG_MESSAGE_SEVERITY_REPORT, "Qoraal system started...") ;

    /*
     * Now process the input from the command line as shell commands until
     * the "exit" command was executed.
     */
    svc_shell_script_run (&qshell_if, "", "version", strlen("version")) ;
    do {
        char line[512];
        printf (SHELL_PROMPT) ;
        int len = qshell_get_line (line, 512) ;
        if (len > 0) {
            svc_shell_script_run (&qshell_if, "", line, len) ;
            

        }

    } while (!_shell_exit) ;


    return 0;
}

int32_t
qshell_out (void* ctx, uint32_t out, const char* str)
{
    if (str && (out && out < SVC_SHELL_IN_STD)) {
        printf ("%s", str) ;

    }

    return  SVC_SHELL_CMD_E_OK ;
}

int32_t
qshell_get_line (char * buffer, uint32_t len)
{
    uint32_t i = 0 ;

    for (i=0; i<len; i++) {
        buffer[i] = getc (stdin) ;
        if (buffer[i] == '\n') break ;
        if (buffer[i] < 0) break ;
    }

    return i ;
}

void
qoraal_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg)
{
    printf("LOGGER: %s\n", msg) ;
}

void
qoraal_print (const char *format)
{
    printf (format) ;
}

void
qoraal_assert (const char *format)
{
    printf (format) ;
    exit (1) ;
}

static int32_t
qshell_exit (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    _shell_exit = true ;
    return SVC_SHELL_CMD_E_OK ;
}

static int32_t
qshell_version (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n", SHELL_VERSION_STR) ;
    return SVC_SHELL_CMD_E_OK ;
}


