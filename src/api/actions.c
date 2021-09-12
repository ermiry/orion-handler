#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <clibs/types/types.h>
#include <clibs/types/string.h>

#include <clibs/collections/dlist.h>

#include <clibs/input.h>

#include <clibs/utils/log.h>
#include <clibs/utils/utils.h>

#include "models/action.h"
#include "models/role.h"

static void action_action_display_internal (
	const DoubleList *actions, const DoubleList *roles
) {

	Role *role = NULL;
	RoleAction *role_action = NULL;
	unsigned int count = 0;
	for (ListElement *le = dlist_start (actions); le; le = le->next) {
		role_action = (RoleAction *) le->data;

		printf ("\n");
		action_print (role_action);

		count = 0;
		for (ListElement *le_role = dlist_start (roles); le_role; le_role = le_role->next) {
			role = (Role *) le_role->data;

			if (role_check_action (role, role_action->name)) {
				(void) printf ("\t%s", role->name);
				count += 1;
			}
		}

		if (!count) {
			clibs_log_warning ("Action has not been assigned to any role");
		}

		(void) printf ("\n");
	}

	(void) printf ("\n");

}

// displays all existing actions
unsigned int action_action_display (void) {

	unsigned int retval = 1;

	DoubleList *actions = actions_get_all ();
	if (actions) {
		if (actions->size) {
			DoubleList *roles = roles_get_all ();
			if (roles) {
				action_action_display_internal (actions, roles);

				dlist_delete (roles);

				retval = 0;
			}

			else {
				clibs_log_error ("Failed to get roles!");
			}
		}

		else {
			clibs_log_warning ("No actions!");
		}

		dlist_delete (actions);
	}

	else {
		clibs_log_error ("Failed to get actions!");
	}

	return retval;

}

// adds a new action with name & description
unsigned int action_action_add (void) {

	unsigned int retval = 1;

	char action_name[64] = { '\0' };
	(void) printf ("\nEnter the new action's name: ");
	(void) scanf ("%s", action_name);

	RoleAction found = { 0 };
	if (action_get_by_name (&found, action_name)) {
		input_clean_stdin ();
		(void) printf ("\nEnter the new action's description: ");
		char *action_description = input_get_line ();
		if (action_description) {
			// add the action to the db
			RoleAction *action = action_create (action_name, action_description);
			if (!action_save (action)) {
				clibs_log_success ("Added action %s to the db", action_name);
				retval = 0;
			}

			else {
				clibs_log_error ("Failed to add action %s to the db!", action_name);
			}

			action_delete (action);

			free (action_description);
		}
	}

	else {
		clibs_log_warning (
			"RoleAction %s is already in the db!",
			action_name
		);
	}

	return retval;

}
