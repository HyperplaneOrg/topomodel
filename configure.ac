# FILE:
#  configure.ac
#
# LICENSE:
#  This file is part of topomodel.
#
#  topomodel is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  topomodel is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with topomodel. If not, see http://www.gnu.org/licenses/.
#
#  Copyright (C) 2015 Andrew Michaelis

AC_INIT([topomodel], [0.0.1-beta])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE()

AC_CONFIG_MACRO_DIR([m4])

# Check for programs...
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_CC

# Look for mpicc (if applicable)
if test -n "$MPICC_PROG"; then
   cc_save="$CC"
   CC="$MPICC_PROG"
   AC_SUBST(MPICC_PROG)
fi

# force some flags for gcc
if test "x$CC" = "xgcc"; then
   CFLAGS="$CFLAGS -Wall"
fi

# force some flags intel icc
if test "x$CC" = "xicc"; then
   CFLAGS="$CFLAGS -warn all"
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(off_t)
   
AC_C_BIGENDIAN

# Check for headers
AC_CHECK_HEADERS([limits.h]) 
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([getopt.h]) 
AC_CHECK_HEADERS([sys/types.h]) 
AC_CHECK_HEADERS([sys/mman.h]) 
AC_CHECK_HEADERS([sys/stat.h]) 
AC_CHECK_HEADERS([fcntl.h]) 
AC_CHECK_HEADERS([pthread.h])
AC_HEADER_TIME

# Check for functions
AC_CHECK_FUNCS([getopt strtok_r])
AC_CHECK_FUNCS([getopt_long getenv])
AC_CHECK_LIB([z], [inflate])
AC_CHECK_LIB([m], [ceil])
AC_CHECK_LIB([pthread], [pthread_create])

AC_DEFINE([USER_PATH_MAX], [2048], [If PATH_MAX missing, no limits.h, then use this])

# Data dir helper install path
CPPFLAGS="$CPPFLAGS "'-DDATADIR="\"$(datadir)\""'

AC_OUTPUT(src/Makefile man/Makefile Makefile) 

echo ""
echo "                       Configure Summary"
echo "                       ================="
echo ""
echo "Compiling Options:"
echo "--------------------"
echo ""
echo "         C Compiler: $CC"
echo "             CFLAGS: $CFLAGS"
echo "           CPPFLAGS: $CPPFLAGS"
echo "            LDFLAGS: $LDFLAGS"
echo "               LIBS: $LIBS"
echo ""
echo "Platform Type Sizes:"
echo "--------------------"
echo ""
echo "        SIZEOF float : $ac_cv_sizeof_float"
echo "        SIZEOF double: $ac_cv_sizeof_double"
echo "         SIZEOF short: $ac_cv_sizeof_short"
echo "           SIZEOF int: $ac_cv_sizeof_int"
echo "          SIZEOF long: $ac_cv_sizeof_long"
echo "        SIZEOF size_t: $ac_cv_sizeof_size_t"
echo "         SIZEOF off_t: $ac_cv_sizeof_off_t"
echo ""
echo "Miscellaneous:"
echo "--------------------"
echo "             PREFIX: $prefix"
echo "           pthreads: $ac_cv_lib_pthread_pthread_create"
echo ""

