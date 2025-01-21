#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_shell.h"
#include "services.h"

/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/

#define DBG_MESSAGE_SHELL(severity, fmt_str, ...)   DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), QORAAL_SERVICE_SHELL, fmt_str, ##__VA_ARGS__)

#define SHELL_VERSION_STR   "Navaro Qoraal Demo v '" __DATE__ "'"
#define SHELL_HELLO         "Enter 'help' or '?' to view available commands. "
#define SHELL_PROMPT        "[Qoraal] #> "

/*===========================================================================*/
/* Service Local Functions                                                   */
/*===========================================================================*/

static void     shell_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg) ;
static int32_t  shell_out (void* ctx, uint32_t out, const char* str);
static int32_t  shell_get_line (char * buffer, uint32_t len) ;

SVC_SHELL_CMD_DECL("exit", qshell_exit, "");
SVC_SHELL_CMD_DECL("version", qshell_version, "");
SVC_SHELL_CMD_DECL("hello", qshell_hello, "");



/*===========================================================================*/
/* Service Local Variables and Types                                         */
/*===========================================================================*/

static bool                 _shell_exit = false ;
static LOGGER_CHANNEL_T     _shell_log_channel = { .fp = shell_logger_cb, .user = (void*)0, .filter = { { .mask = SVC_LOGGER_MASK, .type = SVC_LOGGER_SEVERITY_LOG | SVC_LOGGER_FLAGS_PROGRESS }, {0,0} } };

/*===========================================================================*/
/* Service Functions                                                         */
/*===========================================================================*/

/**
 * @brief       shell_service_ctrl
 * @details
 * @note        For code SVC_SERVICE_CTRL_STATUS, if the return value is E_NOIMPL
 *              the status will be determined by the svc_services module.
 *
 * @param[in] code
 * @param[in] arg
 *
 * @return      status
 *
 * @services
 */
int32_t
shell_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        break ;

    case SVC_SERVICE_CTRL_START:
        svc_logger_channel_add (&_shell_log_channel) ;
        break ;

    case SVC_SERVICE_CTRL_STOP:
        svc_logger_channel_remove (&_shell_log_channel) ;
        break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        res = E_NOIMPL ;
        break ;

    }

    return res ;
}

/**
 * @brief       shell_service_run
 * @details     Runs the shell service, processing input and executing commands
 *              until the "exit" command is issued.
 *
 * @param[in]   arg     Argument passed to the service.
 *
 * @return      status  The result of the shell execution.
 */
int32_t
shell_service_run (uintptr_t arg)
{
    DBG_MESSAGE_SHELL (DBG_MESSAGE_SEVERITY_INFO, "SHELL : : shell STARTED");

    SVC_SHELL_IF_T  qshell_if ;
    svc_shell_if_init (&qshell_if, 0, shell_out, 0) ;

    /*
     * Now process the input from the command line as shell commands until
     * the "exit" command is executed.
     */
    svc_shell_script_run (&qshell_if, "", "version", strlen("version")) ;
    svc_shell_script_run (&qshell_if, "", "hello", strlen("hello")) ;
    do {
        char line[1024];
        printf (SHELL_PROMPT) ;
        int len = shell_get_line (line, sizeof(line)) ;
        if (len > 0) {
            svc_shell_script_run (&qshell_if, "", line, len) ;
            
        }

    } while (!_shell_exit) ;

    DBG_MESSAGE_SHELL(DBG_MESSAGE_SEVERITY_LOG, "SHELL : : shell shutting down...");

    return EOK ;
}

/**
 * @brief       shell_out
 * @details     Handles shell output operations.
 *
 * @param[in]   ctx     The context for the output operation.
 * @param[in]   out     The output channel.
 * @param[in]   str     The string to output.
 *
 * @return      status  The result of the operation.
 */
int32_t
shell_out (void* ctx, uint32_t out, const char* str)
{
    if (str && (out && out < SVC_SHELL_IN_STD)) {
        printf ("%s", str) ;

    }

    return  SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       shell_get_line
 * @details     Reads a line of input from the user.
 *
 * @param[out]  buffer  The buffer to store the input line.
 * @param[in]   len     The maximum length of the buffer.
 *
 * @return      length  The length of the input line.
 */
int32_t
shell_get_line (char * buffer, uint32_t len)
{
    uint32_t i = 0 ;

    for (i=0; i<len; i++) {
        buffer[i] = getc (stdin) ;
        if (buffer[i] == '\n') break ;
        if (buffer[i] < 0) break ;

    }

    return i ;
}

/**
 * @brief       shell_logger_cb
 * @details     Callback function for logging messages from the shell.
 *
 * @param[in]   channel     The logger channel.
 * @param[in]   type        The type of log message.
 * @param[in]   facility    The logging facility.
 * @param[in]   msg         The log message to display.
 */
void
shell_logger_cb (void* channel, LOGGERT_TYPE_T type, uint8_t facility, const char* msg)
{
    printf("--- %s\n", msg) ;
}

/**
 * @brief       qshell_version
 * @details     Outputs the version of the Qoraal shell.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_version (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n", SHELL_VERSION_STR) ;
    return SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       qshell_hello
 * @details     Outputs hello text of the Qoraal shell.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_hello (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n\r\n", SHELL_HELLO) ;
    return SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       qshell_exit
 * @details     Exits the shell service.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_exit (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    _shell_exit = true ;
    return SVC_SHELL_CMD_E_OK ;
}