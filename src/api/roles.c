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

// displays all existing roles
unsigned int role_action_display (void) {

	unsigned int retval = 1;

	DoubleList *roles = roles_get_all ();
	if (roles) {
		if (roles->size) {
			clibs_log_success ("Got %lu roles from the db!", roles->size);

			for (ListElement *le = dlist_start (roles); le; le = le->next) {
				(void) printf ("\n");
				role_print ((Role *) le->data);
			}

			(void) printf ("\n");
		}

		else {
			clibs_log_warning ("NO roles found in the db!");
		}

		dlist_delete (roles);

		retval = 0;
	}

	else {
		clibs_log_error ("Failed to get roles from db!");
	}

	return retval;

}

// adds a new role with just name, no actions
unsigned int role_action_add (void) {

	unsigned int retval = 1;

	char rolename[64] = { '\0' };

	(void) printf ("\nEnter the new role's name: ");
	(void) scanf ("%s", rolename);

	Role found = { 0 };
	if (role_get_by_name (&found, rolename)) {
		Role *role = role_create (rolename);
		if (!role_save (role)) {
			clibs_log_success ("Added role %s to the db", rolename);
			retval = 0;
		}

		else {
			clibs_log_error ("Failed to add role %s to the db!", rolename);
		}
	}

	else {
		clibs_log_warning ("Role %s is already in the db!", rolename);
	}

	return retval;

}

static unsigned int role_action_update_name (Role *role) {

	unsigned int retval = 1;

	char rolename[64] = { '\0' };
	(void) printf ("\nEnter the new role's name: ");
	(void) scanf ("%s", rolename);

	Role found = { 0 };
	if (role_get_by_name (&found, rolename)) {
		role_set_name (role, rolename);
		retval = 0;
	}

	else {
		clibs_log_error ("Role %s is already in the db!", rolename);
	}

	return retval;

}

static unsigned int role_action_update_add_action (Role *role) {

	unsigned int retval = 1;

	char action_name[64] = { '\0' };
	(void) printf ("\nEnter the action to be added: ");
	(void) scanf ("%s", action_name);

	RoleAction action = { 0 };
	if (!action_get_by_name (&action, action_name)) {
		// check if the role already has the action
		if (!role_check_action (role, action_name)) {
			role_add_action (role, action_name);
		}

		else {
			clibs_log_warning (
				"Action %s is already in role %s!",
				action_name, role->name
			);
		}
	}

	else {
		clibs_log_error ("Action %s is NOT in the db!", action_name);
	}

	return retval;

}

static unsigned int role_action_update_remove_action (Role *role) {

	unsigned int retval = 1;

	char action_name[64] = { '\0' };
	(void) printf ("\nEnter the action to be removed: ");
	(void) scanf ("%s", action_name);

	// search action
	if (role_check_action (role, action_name)) {
		role_remove_action (role, action_name);
	}

	else {
		clibs_log_warning (
			"Action %s is NOT in role %s!",
			action_name, role->name
		);
	}

	return retval;

}

static unsigned int role_action_update_actual (
	const Role *role
) {

	unsigned int retval = 1;

	(void) printf ("\n\nRole after update: \n");
	role_print (role);

	input_clean_stdin ();
	(void) printf ("\nConfirm update (yes/no): ");
	char *confirm = input_get_line ();
	if (confirm) {
		// printf ("/%s/", confirm);

		if (!strcmp (confirm, "yes")) {
			if (!role_update (role)) {
				(void) printf ("\n");
				clibs_log_success ("Updated role!");
				(void) printf ("\n");

				retval = 0;
			}

			else {
				(void) printf ("\n");
				clibs_log_error ("Failed to update role!");
				(void) printf ("\n");
			}
		}

		free (confirm);
	}

	return retval;

}

// updates the role name
unsigned int role_action_update (void) {

	unsigned int retval = 1;

	char rolename[64] = { '\0' };
	(void) printf ("\nEnter the role you want to update: ");
	(void) scanf ("%s", rolename);

	Role found = { 0 };
	if (!role_get_by_name (&found, rolename)) {
		unsigned int op = 0;
		do {
			(void) printf ("\nRole Update\n");
			(void) printf ("1. Name\n");
			(void) printf ("2. Add action\n");
			(void) printf ("3. Remove action\n");
			(void) printf ("4. Update\n");
			(void) printf ("5. Exit\n");
			(void) printf ("Select option: ");
			(void) scanf ("%u", &op);

			switch (op) {
				case 1: (void) role_action_update_name (&found); break;
				case 2: (void) role_action_update_add_action (&found); break;
				case 3: (void) role_action_update_remove_action (&found); break;

				case 4: retval = role_action_update_actual (&found); break;

				case 5: break;

				default: break;
			}
		} while (op != 5);
	}

	else {
		clibs_log_error ("Failed to find role %s!", rolename);
	}

	return retval;

}
