#include <stdint.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"


int32_t     platform_init () ;
int32_t     platform_start () ;

void *      platform_malloc (QORAAL_HEAP heap, size_t size) ;
void        platform_free (QORAAL_HEAP heap, void *mem) ;
void        platform_print (const char *format) ;
void        platform_assert (const char *format) ;
uint32_t    platform_wdt_kick (void) ;
uint32_t    platform_current_time (void) ;
void        platform_wait_for_exit (SVC_SERVICES_T service_id) ;