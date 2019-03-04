#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "error.h"

static char const * const local_e_msg[] = {
	"Success",
	"Operation failed",
	"Not enough argument",
	"File empty or has no valid entries",
	"There are no entries shared with universe"
};

char const *
strerrnum (int errnum)
{
	int domain, error_code;
	char const * msg = NULL;

	domain = get_error_domain (errnum);
	error_code = get_error_code (errnum);

	switch (domain)
		{
		case D_LOCAL:
			{
				msg = local_e_msg[error_code];
				break;
			}
		case D_SYSTEM:
			{
				msg = strerror (error_code);
				break;
			}
		}

	return msg;
}

void
error (int status, int errnum, const char *format, ...)
{
	if (fflush (stdout) == EOF)
		fprintf (stderr, "%s: Failed to flush stdout stream: %s\n",
				PACKAGE, strerror (errno));

	va_list ap;
	char const *errnum_msg;

	va_start (ap, format);

	fprintf (stderr, "%s: ", PACKAGE);
	vfprintf (stderr, format, ap);

	if (errnum)
		{
			errnum_msg = strerrnum (errnum);
			if (errnum_msg != NULL)
				fprintf (stderr, ": %s", errnum_msg);
		}

	fprintf (stderr, "\n");

	va_end (ap);

	if (status)
		exit (status);
}
