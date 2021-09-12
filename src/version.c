#include <clibs/utils/log.h>

#include "version.h"

// print full handler version information
void orion_handler_version_print_full (void) {

	clibs_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nOrion Handler Version: %s", ORION_HANDLER_VERSION_NAME
	);

	clibs_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Release Date & time: %s - %s", ORION_HANDLER_VERSION_DATE, ORION_HANDLER_VERSION_TIME
	);

	clibs_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Author: %s\n", ORION_HANDLER_VERSION_AUTHOR
	);

}

// print the version id
void orion_handler_version_print_version_id (void) {

	clibs_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nOrion Handler Version ID: %s\n", ORION_HANDLER_VERSION
	);

}

// print the version name
void orion_handler_version_print_version_name (void) {

	clibs_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nOrion Handler Version: %s\n", ORION_HANDLER_VERSION_NAME
	);

}
