/*
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#ifndef _PAM_SYSLOG_H_
#define _PAM_SYSLOG_H_

#include <security/pam_modules.h>

/**
 * pam_syslog()
 *
 * Syslogging function for errors and other information.
 *
 * @pamh
 * @priority
 * @fmt
 */
void pam_syslog(pam_handle_t *pamh, int priority, const char *fmt, ...);

#endif
