#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <check.h>
#include <check_gsia.h>

int
main (void)
{
	int number_failed = 0;
	SRunner *sr = NULL;

	sr = srunner_create (make_hyper_suite ());
	srunner_add_suite (sr, make_array_suite ());
	srunner_add_suite (sr, make_comm_suite ());
	/*srunner_set_tap (sr, "-");*/

	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return number_failed;
}
