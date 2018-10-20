#ifndef ERROR_H
#define ERROR_H

#define E_SUCCESS 0

void error (int status, int errnum, const char *format, ...)
	__attribute__ ((__format__ (__printf__, 3, 4)));

#endif /* error.h */
