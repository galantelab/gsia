#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsia], [0.1.0], [tmiller@mochsl.org.br])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB(gmp, __gmpz_init, ,
	[AC_MSG_ERROR([GNU MP not found, see https://gmplib.org/])])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h stdarg.h errno.h assert.h gmp.h])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
