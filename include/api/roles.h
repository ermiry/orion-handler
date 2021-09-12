#ifndef _API_ROLES_H_
#define _API_ROLES_H_

// displays all existing roles
extern int role_action_display (void);

// adds a new role with just name, no actions
extern int role_action_add (void);

// updates the role name
extern int role_action_update (void);

#endif