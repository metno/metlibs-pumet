#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([puMet], [3.2.0], [diana@met.no])
AM_INIT_AUTOMAKE
METNO_SET_VERSION_INFO


AC_CONFIG_SRCDIR([src/miSymbol.cc])
AC_CONFIG_HEADER([include/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Language stack
AC_LANG_PUSH(C++)

# Checks for libraries.
AC_CXX_HAVE_STD
AC_CXX_HAVE_STL


# pkg-config path
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$prefix/lib/pkgconfig

# check for puTools
PKG_CHECK_MODULES([puTools], [putools])
METNO_CHECK_LIB([puTools], [main], , , , [$puTools_LIBS], 
                [the puTools package])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

METNO_CHECK_HEADERS([puTools/miString.h puTools/miTime.h puTools/miCommandLine.h], , , , [$puTools_CFLAGS],[the puTools package])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC
AC_CHECK_FUNCS([pow sqrt])

#making our own .pc file, checking that it is ok and forfilled
AC_CONFIG_FILES([pumet.pc])
AC_OUTPUT

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:.
PKG_CHECK_MODULES([own_dependencies], [${PACKAGE_TARNAME}])


AC_CONFIG_FILES([Makefile 
		include/Makefile
                src/Makefile])
AC_OUTPUT
