AC_INIT([libsecretary], [0.1.1], [brandizzi+libsecretary@gmail.com])
AC_CONFIG_SRCDIR([secretary.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB

AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
