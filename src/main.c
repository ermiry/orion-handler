#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <signal.h>

#include <clibs/types/types.h>
#include <clibs/types/string.h>

#include <clibs/input.h>
#include <clibs/version.h>

#include <clibs/utils/log.h>
#include <clibs/utils/utils.h>

#include <cmongo/mongo.h>

#include "orion.h"
#include "version.h"

#include "models/role.h"
#include "models/action.h"

#include "api/roles.h"
#include "api/actions.h"

typedef enum HandlerAction {

	HANDLER_ACTION_NONE,		// no action to be performed

	HANDLER_ACTION_ROLES,		// select to work with roles
	HANDLER_ACTION_ACTIONS,		// select to work with actions
	HANDLER_ACTION_USERS,		// select to work with users

	HANDLER_ACTION_RESET,		// reset database to default values
	HANDLER_ACTION_CLEAR		// clear database

} HandlerAction;

typedef enum RoleActionType {

	ROLE_ACTION_NONE,

	ROLE_ACTION_DISPLAY,		// displays existing roles
	ROLE_ACTION_ADD,			// adds a new role
	ROLE_ACTION_UPDATE			// updates an existing role

} RoleActionType;

typedef enum ActionAction {

	ACTION_ACTION_NONE,

	ACTION_ACTION_DISPLAY,		// displays existing actions
	ACTION_ACTION_ADD,			// adds a new action
	ACTION_ACTION_UPDATE		// updates an existing action

} ActionAction;

#pragma region end

static unsigned int handler_end (void) {

	if (mongo_get_status () == MONGO_STATUS_CONNECTED) {
		// close connection handlers
		actions_model_end ();
		
		roles_model_end ();

		mongo_disconnect ();
	}

	clibs_log_line_break ();
	clibs_log_success ("End!");

	clibs_log_end ();

	return 0;

}

static void end (int dummy) {

	handler_end ();

	exit (0);

}

#pragma endregion

#pragma region auth

// handle authentication using mongo
static unsigned int handler_auth_mongo (void) {

	unsigned int retval = 1;

	bool connected_to_mongo = false;

	if (!mongo_connect ()) {
		// test mongo connection
		if (!mongo_ping_db ()) {
			unsigned int errors = 0;

			clibs_log_success ("Connected to Mongo DB!");

			errors |= actions_model_init ();

			errors |= roles_model_init ();

			connected_to_mongo = true;

			retval = errors;
		}
	}

	if (!connected_to_mongo) {
		clibs_log_error ("Failed to connect to mongo");
	}

	return retval;

}

static unsigned int handler_auth_local_db (void) {

	unsigned int retval = 1;

	char password[64] = { '\0' };

	(void) printf ("\n");
	(void) printf ("Enter password: ");
	(void) input_password (password);
	(void) printf ("\n");

	mongo_set_host (HOST_IP);
	mongo_set_port (MONGO_PORT);
	mongo_set_username (MONGO_USER);
	mongo_set_password (password);

	mongo_set_db_name (MONGO_DB_NAME);
	mongo_set_app_name (MONGO_APP_NAME);

	if (!mongo_uri_generate ()) {
		retval = handler_auth_mongo ();
	}

	return retval;

}

static unsigned int handler_auth_cloud_db (void) {

	char url[256] = { '\0' };

	(void) printf ("\n");
	(void) printf ("Enter mongo uri: ");
	(void) input_password (url);
	(void) printf ("\n");

	mongo_set_uri (url);

	mongo_set_db_name (MONGO_DB_NAME);
	mongo_set_app_name (MONGO_APP_NAME);

	return handler_auth_mongo ();

}

static unsigned int handler_auth (const String *db) {

	unsigned int retval = 1;

	if (!strcmp ("local", db->str)) retval = handler_auth_local_db ();
	else if (!strcmp ("cloud", db->str)) retval = handler_auth_cloud_db ();
	else {
		clibs_log_line_break ();
		clibs_log_error ("Unknown db type: %s\n", db->str);
	}

	return retval;

}

#pragma endregion

#pragma region handler

static void print_help (void) {

	(void) printf ("\n");
	(void) printf ("Usage: ./bin/handler [OPTION]\n");
	(void) printf ("-h\t\t\tPrint this help\n");
	(void) printf ("-R [OPTION]\t\tSelect a role action\n");
	(void) printf ("\t--display \tDisplays all existing roles\n");
	(void) printf ("\t--add     \tCreate a new role\n");
	(void) printf ("\t--update  \tUpdate an existing role\n");
	(void) printf ("-A [OPTION]\t\tSelect an actions action\n");
	(void) printf ("\t--display \tDisplays all existing actions\n");
	(void) printf ("\t--add     \tCreate a new action\n");
	(void) printf ("\t--update  \tUpdate an existing action\n");
	// (void) printf ("-U [OPTION]\t\tSelect a user action\n");
	// (void) printf ("\t--count   \tReturns the user's count x role\n");
	// (void) printf ("\t--get     \tSearch a user by username\n");
	// (void) printf ("\t--create  \tCreate a new user with specified values\n");
	// (void) printf ("\t--pswd    \tAbility to resets a user's password\n");
	// (void) printf ("\t--delete  \tAbility to delete a user\n");
	// (void) printf ("\t--god     \tDisplay god user menu\n");
	// (void) printf ("\t--first   \tReset all users first time flag\n");
	// (void) printf ("\t--drop    \tSelect which users to drop by role\n");
	(void) printf ("\n");

}

static unsigned int handler_action_roles (const RoleActionType role_action) {

	unsigned int retval = 1;

	switch (role_action) {
		case ROLE_ACTION_NONE: break;

		case ROLE_ACTION_DISPLAY: retval = role_action_display (); break;

		case ROLE_ACTION_ADD: retval = role_action_add (); break;

		case ROLE_ACTION_UPDATE: retval = role_action_update (); break;

		default: break;
	}

	return retval;

}

static unsigned int handler_action_actions (const ActionAction action_action) {

	unsigned int retval = 1;

	switch (action_action) {
		case ACTION_ACTION_NONE: break;

		case ACTION_ACTION_DISPLAY: retval = action_action_display (); break;

		case ACTION_ACTION_ADD: retval = action_action_add (); break;

		// case ACTION_ACTION_UPDATE: retval = action_action_update (); break;

		default: break;
	}

	return retval;

}

static void handler_internal (
	const HandlerAction handler_action,
	const RoleActionType role_action,
	const ActionAction action_action
) {

	if (handler_action != HANDLER_ACTION_NONE) {
		switch (handler_action) {
			case HANDLER_ACTION_NONE: break;

			case HANDLER_ACTION_ROLES:
				(void) handler_action_roles (role_action);
				break;

			case HANDLER_ACTION_ACTIONS:
				(void) handler_action_actions (action_action);
				break;

			case HANDLER_ACTION_RESET:
				break;

			case HANDLER_ACTION_CLEAR:
				break;

			default: break;
		}
	}

	else {
		print_help ();
	}

}

static void handler (const int argc, char **argv) {

	HandlerAction handler_action = HANDLER_ACTION_NONE;

	RoleActionType role_action = ROLE_ACTION_NONE;
	ActionAction action_action = ACTION_ACTION_NONE;

	// int j = 0;
	if (argc > 1) {
		const char *curr_arg = NULL;
		for (int i = 1; i < argc; i++) {
			curr_arg = argv[i];

			if (!strcmp (curr_arg, "-h")) print_help ();

			// roles
			else if (!strcmp (curr_arg, "-R")) {
				handler_action = HANDLER_ACTION_ROLES;

				int next = (i + 1);
				if (next < argc) {
					if (!strcmp (argv[next], "--display")) {
						role_action = ROLE_ACTION_DISPLAY;
					}

					else if (!strcmp (argv[next], "--add")) {
						role_action = ROLE_ACTION_ADD;
					}

					else if (!strcmp (argv[next], "--update")) {
						role_action = ROLE_ACTION_UPDATE;
					}
				}

				else {
					clibs_log_error ("No role action selected!");
				}

				i++;
			}

			// actions
			else if (!strcmp (curr_arg, "-A")) {
				handler_action = HANDLER_ACTION_ACTIONS;

				int next = (i + 1);
				if (next < argc) {
					if (!strcmp (argv[next], "--display")) {
						action_action = ACTION_ACTION_DISPLAY;
					}

					else if (!strcmp (argv[next], "--add")) {
						action_action = ACTION_ACTION_ADD;
					}

					else if (!strcmp (argv[next], "--update")) {
						action_action = ACTION_ACTION_UPDATE;
					}
				}

				else {
					clibs_log_error ("No action action selected!");
				}

				i++;
			}

			else {
				clibs_log_warning ("Unknown argument: %s", curr_arg);
			}
		}
	}

	handler_internal (
		handler_action,
		role_action,
		action_action
	);

}

#pragma endregion

int main (int argc, char **argv) {

	String *db = NULL;

	(void) signal (SIGINT, end);

	clibs_log_init ();

	clibs_version_print_full ();

	orion_handler_version_print_full ();

	if (argc >= 3 && !strcmp (argv[argc - 2], "--db")) {
		db = str_new (argv[argc - 1]);
		(void) printf ("Selected db: %s\n", db->str);

		argc -= 2;
	}

	else {
		db = str_new ("local");
	}

	// handle authentication
	if (!handler_auth (db)) {
		handler (argc, argv);
	}

	str_delete (db);

	return handler_end ();

}
