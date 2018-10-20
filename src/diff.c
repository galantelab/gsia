#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <error.h>
#include <errno.h>
#include <assert.h>

struct line_array
{
	size_t num_lines;
	size_t buf_size;
	char **lines;
};

int
line_array_init (struct line_array *la)
{
	assert (la != NULL);
	int errnum = 0;

	*la = (struct line_array){.num_lines = 0, .buf_size = BUFSIZ};
	la->lines = (char **) calloc (BUFSIZ, sizeof (char **));

	if (la->lines == NULL)
		errnum = errno;

	return errnum;
}

void
line_array_clear (struct line_array *la)
{
	assert (la != NULL);

	for (int i = 0; i < la->num_lines; i++)
		free (la->lines[i]);

	free (la->lines);
}

char *
chomp (char *line)
{
	if (line == NULL)
		return NULL;

	size_t len = strlen (line);

	if (len && line[len - 1] == '\n')
		line[len - 1] = '\0';

	return line;
}

int
line_array_read_file (struct line_array *la, const char *filename)
{
	assert (la != NULL);
	assert (filename != NULL);

	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	size_t nread;
	int errnum = 0;

	// Clear errno
	errno = 0;

	fp = fopen (filename, "r");

	if (fp == NULL)
		goto Fail;

	while (!feof (fp))
		{
			nread = getline (&line, &len, fp);

			// End of file or error
			if (nread == EOF)
				goto Fail;

			char *line_copy = chomp (strdup (line));

			if (line_copy == NULL)
				goto Fail;

			la->lines[la->num_lines++] = line_copy;

			if (la->num_lines == la->buf_size)
				{
					la->buf_size += BUFSIZ;
					la->lines = realloc (la->lines, sizeof (char **) * la->buf_size);

					if (la->lines == NULL)
						goto Fail;
				}
		}

Exit:
	if (line != NULL) free (line);
	if (fp != NULL) fclose (fp);
	return errnum;

Fail:
	errnum = errno;
	goto Exit;
}

int
main (int argc, char *argv[])
{
	if (argc == 1)
		return 0;

	struct line_array la;
	int errnum = 0;
	int ret_val = EXIT_SUCCESS;

	errnum = line_array_init (&la);

	if (errnum != E_SUCCESS)
		{
			error (0, errnum, "Failed to initialize line_array");
			goto Fail;
		}

	errnum = line_array_read_file (&la, argv[1]);

	if (errnum != E_SUCCESS)
		{
			error (0, errnum, "Failed to read file '%s'", argv[1]);
			goto Fail;
		}

	printf ("file: %s\nnumber of lines: %d\n", argv[1], la.num_lines);
	for (int i = 0; i < la.num_lines; i++)
		printf ("%s\n", la.lines[i]);

Exit:
	line_array_clear (&la);
	return ret_val;

Fail:
	ret_val = EXIT_FAILURE;
	goto Exit;
}
