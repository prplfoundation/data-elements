#                                              -*- Autoconf -*-
# Process this file with autoconf to produce configure script.
#

AC_PREREQ([2.65])
AC_INIT([wccf], [2.0.0], [bugs@appliedbroadband.com])
LT_INIT

AC_PREFIX_DEFAULT($WCCF_ROOT)
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AM_INIT_AUTOMAKE([parallel-tests color-tests subdir-objects])

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_LIBTOOL(libtool)

AC_ARG_ENABLE([gcov],
  [AS_HELP_STRING([--enable-gcov],
    [use Gcov to test the test suite])],
    [],
    [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h uuid.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES(
	Makefile
        sensors/Makefile
        processors/Makefile
        transmitters/Makefile
        utils/Makefile
)

AC_OUTPUT

