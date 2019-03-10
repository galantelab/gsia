#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "array.c"
#include "gsia.h"
#include "utils.h"
#include "error.h"

static struct option opt[] =
{
	{"version",       no_argument,       0, 'v'},
	{"help",          no_argument,       0, 'h'},
	{"ignore-case",   no_argument,       0, 'i'},
	{"save-diff",     required_argument, 0, 'd'},
	{"list-file",     required_argument, 0, 'l'},
	{"universe-file", required_argument, 0, 'u'},
	{"feature-file",  required_argument, 0, 'f'},
	{0,               0,                 0,  0 }
};

static const char *help_msg =
	PACKAGE_STRING "\n"
	"\n"
	"Usage: " PACKAGE_NAME " [-hv]\n"
	"       " PACKAGE_NAME " [-i] [-d FILE] -l FILE -u FILE -f FILE ..\n"
	"\n"
	"Compute set enrichment analysis\n"
	"\n"
	"Options:\n"
	"   -h, --help             Show help options\n"
	"   -v, --version          Show current version\n"
	"   -i, --ignore-case      Ignore case distinctions\n"
	"   -d, --save-diff        Save the differences among\n"
	"                          list, universe and feature files\n"
	"\n"
	"Mandatory Options:\n"
	"   -l, --list-file        LIST in test for observed successes\n"
	"   -u, --universe-file    Population entries LIST\n"
	"   -f, --feature-file     ARRAY of success state entries LIST\n"
	"                          It can be passed multiple times\n"
	"\n"
	"ARRAY is a string list of FILES separated by space, colon,\n"
	"semicolon or comma\n"
	"\n"
	"LIST is a FILE with one entry by line\n";

static const char *try_help_msg =
	"Try '" PACKAGE_NAME " --help' for more information";

int
main (int argc, char *argv[])
{
	if (argc == 1)
		{
			printf ("%s\n", help_msg);
			exit (EXIT_SUCCESS);
		}

	int rc = 0;
	const char *list_file = NULL;
	const char *universe_file = NULL;
	const char *diff_file = NULL;
	int ignore_case = 0;
	PtrArray *feature_file = NULL;

	feature_file = ptr_array_new (NULL, &rc);
	if (feature_file == NULL)
		goto Fail;

	int option_index = 0;
	int c;

	while (1)
		{
			c = getopt_long (argc, argv, "vhid:l:u:f:", opt, &option_index);
			if (c == -1)
				break;

			switch (c)
				{
				case 'i':
					{
						ignore_case = 1;
						break;
					}
				case 'd':
					{
						diff_file = optarg;
						break;
					}
				case 'l':
					{
						list_file = optarg;
						break;
					}
				case 'u':
					{
						universe_file = optarg;
						break;
					}
				case 'f':
					{
						char *scratch = NULL;
						char *token = strtok_r (optarg, ",;: ", &scratch);
						while (token != NULL)
							{
								rc = ptr_array_add (feature_file, token);
								if (rc != E_SUCCESS)
									goto Fail;
								token = strtok_r (NULL, ",;: ", &scratch);
							}
						break;
					}
				case 'v':
					{
						printf ("%s\n", PACKAGE_STRING);
						exit (EXIT_SUCCESS);
					}
				case 'h':
					{
						printf ("%s\n", help_msg);
						exit (EXIT_SUCCESS);
					}
				case '?':
				case ':':
					{
						fprintf (stderr, "%s\n", try_help_msg);
						exit (EXIT_FAILURE);
					}
				}
		}

	if (list_file == NULL)
		{
			error (0, 0, "Missing --list-file option\n%s", try_help_msg);
			goto Exit;
		}

	if (universe_file == NULL)
		{
			error (0, 0, "Missing --universe-file option\n%s", try_help_msg);
			goto Exit;
		}

	if (feature_file->len == 0)
		{
			error (0, 0, "Missing --feature-file option\n%s", try_help_msg);
			goto Exit;
		}

	ptr_array_uniq (feature_file, cmpstringp);

	rc = gsia (list_file, universe_file, feature_file->len,
			(const char **) feature_file->pdata, diff_file, ignore_case);

	if (rc != E_SUCCESS)
		goto Fail;

Exit:
	ptr_array_free (feature_file, 1);
	return rc == E_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;

Fail:
	error (0, rc, "Error message");
	goto Exit;
}
