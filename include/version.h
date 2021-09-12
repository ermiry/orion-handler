#ifndef _ORION_HANDLER_VERSION_H_
#define _ORION_HANDLER_VERSION_H_

#define ORION_HANDLER_VERSION			"0.1"
#define ORION_HANDLER_VERSION_NAME		"Version 0.1"
#define ORION_HANDLER_VERSION_DATE		"11/09/2021"
#define ORION_HANDLER_VERSION_TIME		"21:44 CST"
#define ORION_HANDLER_VERSION_AUTHOR	"Erick Salas"

// print full handler version information
extern void orion_handler_version_print_full (void);

// print the version id
extern void orion_handler_version_print_version_id (void);

// print the version name
extern void orion_handler_version_print_version_name (void);

#endif