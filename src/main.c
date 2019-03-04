#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "gsia.h"
#include "error.h"

int
main (int argc, char *argv[])
{
	if (argc < 4)
		return 0;

	int rc = 0;

	rc = gsia (argv[1], argv[2], argc - 3, (const char **) argv + 3);
	if (rc != E_SUCCESS)
		error (0, rc, "Error message");

	return rc == E_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
