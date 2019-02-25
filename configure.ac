#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gsia], [0.3.0], [tmiller@mochsl.org.br])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

LT_PREREQ([2.2])
LT_INIT([dlopen])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([gslcblas], [cblas_dgemm], [have_gslcblas=yes])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm], [have_gsl=yes])

if ! test x$have_gslcblas = xyes -a x$have_gsl = xyes; then
	AC_MSG_WARN([GNU Scientific Library not found, see https://www.gnu.org/software/gsl/])
fi

PKG_CHECK_MODULES([CHECK], [check], ,
	[AC_MSG_WARN([check not found, It will not be possible to run 'make check'])])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_ASSERT
AC_CHECK_HEADERS([gsl/gsl_randist.h gsl/gsl_cdf.h check.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_OUTPUT
