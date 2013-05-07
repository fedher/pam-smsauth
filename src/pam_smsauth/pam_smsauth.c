/*
 * pam_smsauth 
 *
 * Allows remote and local user authentication through a sms that 
 * contains a validation token. In case of sms gateway error, it sends the
 * token through an email.
 *
 * (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define PAM_SM_AUTH

#include <security/pam_modules.h>
#include <security/_pam_macros.h>

#include "pam_prompt.h"
#include "pam_syslog.h"
#include "pam_parse_args.h"
#include "pam_data.h"
#include "pam_register_var.h"

#include "otp.h"
#include "user.h"
#include "sms.h"
#include "error.h"

pam_smsauth_arg_t pam_arg;
pam_smsauth_data_t pam_data;

/* --- authentication management functions --- */

PAM_EXTERN int 
pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	struct user_info user;
	const char *otp_input = NULL, *otp = NULL;
        char *err = NULL;
	int ctrl = 0;
	sms_result_t result = SMS_NONE;
	time_t time_now = time(NULL), time_expir = 0;
	int ret = PAM_AUTH_ERR;

	pam_arg_init(&pam_arg);	
	if (pam_arg_parse(&pam_arg, argc, argv, &ctrl) != PARSER_OK)
		pam_syslog(pamh, LOG_WARNING, "W: pam argument unknown.");

	pam_data_init(&pam_data);
	if ((ret = pam_data_set(&pam_data, pamh, err)) != PAM_SUCCESS)
		GOTO_ERR(ret, pamh, LOG_ERR, err);

	PAM_DEBUG(pamh, ctrl, "D: BEGIN: user: %s.", pam_data.user);
	PAM_DEBUG(pamh, ctrl, "D: time: %ld.", time_now);

	if (ctrl & PAM_SMSAUTH_EXCEPT)
		if (!strncmp(pam_data.user, pam_arg.except_user, 
					strlen(pam_arg.except_user)))
			RETURN_RET(PAM_SUCCESS, pamh, "I: except user: %s", 
					pam_data.user);

	user_init(&user);
	if (user_get_info(&user, pam_data.user))
		GOTO_ERR(PAM_IGNORE, pamh, LOG_NOTICE,
			"N: gecos not found for user %s.", pam_data.user);

	if (pam_is_registered_var(pam_data.user, "timestamp", VARINT))
		if (pam_unregister_expir_vars(time_now))
			PAM_DEBUG(pamh, ctrl, "D: old registers deleted.");

	if (!pam_is_registered_var(pam_data.user, "user", VARCHAR)) {
		time_expir = pam_arg.time_expir * 60 + time_now;

		if (!pam_register_init(pam_data.user, time_expir))
			GOTO_ERR(PAM_SYSTEM_ERR, pamh, LOG_ERR,
					"E: pam_register_init error.");
	
		PAM_DEBUG(pamh, ctrl, "D: time_expir: %ld.", time_expir);
	}

	if (!pam_is_registered_var(pam_data.user, "otp", VARCHAR)) {
		/* Generates the otp. */	
		if (!(user.otp = otp_gen(pam_arg.otp_len, pam_arg.otp_type)))
			GOTO_ERR(PAM_SYSTEM_ERR, pamh, LOG_ERR, 
					"E: otp generation error.");

		/* Sends the otp to the mobile. */
		result = sms_send(pam_arg.sock_path, JSON_CMD_REQ, &user);
		if (sms_on_err(pam_arg.sock_path, result, &user, ctrl, &ret)) {
			PAM_DEBUG(pamh, ctrl, "D: relaxed %s.", 
				ctrl & PAM_SMSAUTH_RELAXED ? "on" : "off");

			GOTO_ERR(ret, pamh, LOG_ERR, "E: sms_send: %s.", 
					err2str(SMS, result));
		}

		PAM_DEBUG(pamh, ctrl, "D: sms_send: %s.", err2str(SMS, result));

		/*
		 * Registers the otp (so it is not necessary to generate it 
		 * again) until the expiration time is reached.
		 */
		if (!pam_register_otp(pam_data.user, user.otp))
			GOTO_ERR(PAM_SYSTEM_ERR, pamh, LOG_ERR, 
					"E: couldn't register the otp.");

		PAM_DEBUG(pamh, ctrl, "D: otp %s was generated and registered.", 
				user.otp);
	}
	
	if (!(otp_input = otp_get_from_user(pamh)))
		GOTO_ERR(PAM_AUTHINFO_UNAVAIL, pamh, LOG_NOTICE, 
				"N: otp not input.");

	PAM_DEBUG(pamh, ctrl, "D: otp from user: %s.", otp_input); 

	/* Gets the otp saved into db. */
	if ((otp = pam_get_otp(pam_data.user)) == NULL)
		GOTO_ERR(PAM_SYSTEM_ERR, pamh, LOG_ERR,
				"E: couldn't get the otp from pam.");

	PAM_DEBUG(pamh, ctrl, "D: otp from pam: %s.", otp);

	ret = otp_cmp(otp_input, otp, pam_arg.otp_len);
	if (ret != PAM_SUCCESS)
		GOTO_ERR(PAM_AUTH_ERR, pamh, LOG_NOTICE, "N: invalid otp.");

out:
	pam_syslog(pamh, LOG_NOTICE, "N: %suser %s %sauthenticated.",
			(ctrl & PAM_SMSAUTH_DEBUG) ? "END: " : "", 
			pam_data.user,
			(ret == PAM_SUCCESS || ret == PAM_IGNORE) ? "" : 
			"not ");

	if (otp_input != NULL) 
		free((void *)otp_input);
	if (otp != NULL)
		free((void *)otp);

	return ret;
}

PAM_EXTERN int 
pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	return PAM_IGNORE;
}

#ifdef PAM_STATIC

/* static module data */

struct pam_module _pam_smsauth_modstruct = {
	"pam_smsauth",
	pam_sm_authenticate,
	pam_sm_setcred,
	NULL,
	NULL,
	NULL,
	NULL
};
#endif
