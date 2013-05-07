/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>

#include <security/_pam_macros.h>
#include <security/pam_modules.h>

#include "pam_syslog.h"

#define MODULE "pam_smsauth"

void
pam_syslog(pam_handle_t *pamh, int priority, const char *fmt, ...)
{
	int save_errno = errno;
	const void *item = NULL;
	const char *service = NULL;
	va_list args;
	char *msgbuf = NULL;

	if (pam_get_item(pamh, PAM_SERVICE, &item) != PAM_SUCCESS || !item)
		service = "UNKNOWN SERVICE";
	else
		service = item;

	openlog(service, LOG_CONS | LOG_PID, LOG_AUTHPRIV);

	va_start(args, fmt);
	errno = save_errno;
	if (vasprintf (&msgbuf, fmt, args) < 0)
		msgbuf = NULL;
	va_end(args);

	if (!msgbuf) {
		syslog(LOG_AUTHPRIV|LOG_CRIT, "(%s) vasprintf: %m",
				MODULE);
		closelog();
		return;
	}

	syslog(LOG_AUTHPRIV|priority, "(%s) %s", MODULE, msgbuf);

	if (msgbuf != NULL) 
		free(msgbuf);

	closelog();
}
