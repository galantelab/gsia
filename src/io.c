#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <utils.h>
#include <io.h>

int
read_file_lines (PtrArray *array, const char *filename)
{
	assert (array != NULL);
	assert (filename != NULL);

	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	size_t nread;
	int errnum = 0;

	errno = 0;

	fp = fopen (filename, "r");

	if (fp == NULL)
		goto Fail;

	while (!feof (fp))
		{
			nread = getline (&line, &len, fp);

			// End of file or error
			if (nread == EOF)
				{
					if (errno)
						goto Fail;
					break;
				}

			chomp (line);
			trim (line);

			// Line is empty
			if (*line == '\0')
				continue;

			char *line_copy = strdup (line);

			if (line_copy == NULL)
				goto Fail;

			errnum = ptr_array_add (array, line_copy);

			if (errnum != E_SUCCESS)
				goto Exit;
		}

Exit:
	if (line != NULL) free (line);
	if (fp != NULL) fclose (fp);
	return errnum;

Fail:
	errnum = set_error_code (D_SYSTEM, errno);
	goto Exit;
}
