#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <error.h>

void
error (int status, int errnum, const char *format, ...)
{
	if (fflush (stdout) == EOF)
		fprintf (stderr, "%s: Failed to flush stdout stream: %s\n",
				PACKAGE, strerror (errno));

	va_list ap;
	va_start (ap, format);

	fprintf (stderr, "%s: ", PACKAGE);
	vfprintf (stderr, format, ap);

	if (errnum)
		fprintf (stderr, ": %s", strerror (errnum));

	fprintf (stderr, "\n");

	va_end (ap);

	if (status)
		exit (status);
}
