#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

enum domain_e
{
	D_LOCAL = 0,
	D_SYSTEM
};

enum local_e
{
	E_SUCCESS = 0,
	E_FAILED,
	E_NOARG,
	E_NOLINE,
	E_NOSHRED
};

#define set_error_code(domain, error_code) ((domain << 16) | (error_code))
#define get_error_domain(errnum) (errnum >> 16)
#define get_error_code(errnum)  (errnum & ~((errnum >> 16) << 16))

char const * strerrnum (int errnum);

void error (int status, int errnum, const char *format, ...)
	__attribute__ ((__format__ (__printf__, 3, 4)));

#endif /* error.h */
