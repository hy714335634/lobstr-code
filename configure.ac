# Copyright (C) 2011-2014 Melissa Gymrek <mgymrek@mit.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([lobSTR],
	m4_esyscmd([config/git-version-gen .tarball-version]),
        [Melissa Gymrek mgymrek@mit.edu],
        [lobSTR])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Detect host (used later for Darwin/Mac-OS detection)
AC_CANONICAL_HOST

LT_INIT([static])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

## Check for GSL (GNU Scientific Library)
PKG_CHECK_MODULES([GSL],[gsl])


AX_PTHREAD([],[
   AC_MSG_ERROR([Pthread support is required for LobSTR])
])
AX_CHECK_ZLIB()

# Detect Mac-OS-X systems, and set USE_MACOS_DISPATCH in 'config.h'
# This value is used in './src/xsemaphore.h' .
AS_CASE([$host],
       [*-*-darwin*], [MT_USE_DISTACH=yes]
       [MT_USE_DISPATCH=""]
     )
if test "x$MT_USE_DISTACH" = "xyes" ; then
  AC_DEFINE([USE_MACOS_DISPTACH], 1, [True if compiling on Mac-OS-X, need to use dispatch/semaphore.h])
fi

AX_BOOST_BASE([1.38.0])
PKG_CHECK_MODULES([CPPUNIT],[cppunit])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# BamTools needs HAVE_FSEEK64
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO
AC_CHECK_FUNCS(fseek64, xpdf_cv_func_fseek64=yes, xpdf_cv_func_fseek64=no)
AC_CHECK_FUNCS(ftell64, xpdf_cv_func_ftell64=yes, xpdf_cv_func_ftell64=no)
if test "$xpdf_cv_func_fseek64" = yes -a "$xpdf_cv_func_ftell64" = yes; then
  AC_DEFINE(HAVE_FSEEK64)
fi

# To compile a static executable (before binary packaging?),
# use:
#    ./configure --enable-all-static
#
# NOTE:
#  1.  Using "./configure LDFLAGS=-static" will not work, because
#      libtool will override the LDFLAG files at times.
#  2.  The flag 'LT_LDFLAGS' is used in ./src/Makefile.am with LDADD.
#
# 'all-static' marco copied from subversion's configure.ac
AC_ARG_ENABLE(all-static,
              AS_HELP_STRING([--enable-all-static],
                             [Build completely static (standalone) binaries.]),
                             [
                              if test "$enableval" = "yes" ; then
                                      LT_LDFLAGS="-all-static $LT_LDFLAGS"
                              elif test "$enableval" != "no" ; then
                                      AC_MSG_ERROR([--enable-all-static doesn't accept argument])
                                      fi
                                      ])

AC_SUBST(LT_LDFLAGS)

# Add Debian-specific Hardening flags
# See:  https://wiki.debian.org/HardeningWalkthrough
# Requires 'dpkg-buildflags' from the 'dpkg-dev' package
#
# To use:
#    ./configure --enable-debian-hardening
#
AC_ARG_ENABLE(debian-hardening,
              AS_HELP_STRING([--enable-debian-hardening],
                             [Build with Debian-specific hardening flags]),
                             [
                              if test "$enableval" = "yes" ; then
				      :
                              elif test "$enableval" != "no" ; then
                                      AC_MSG_ERROR([--enable-debian-hardening doesn't accept argument])
                                      fi
                                      ])

AC_CHECK_PROG([DPKG_BUILDFLAGS],[dpkg-buildflags],[yes],[no],[])

if test "x$enable_debian_hardening" = "xyes" ; then
	if test "x$DPKG_BUILDFLAGS" != "xyes" ; then
		AC_MSG_ERROR([debian-hardening requires the program dpkg-buildflag, which was not found])
	fi
	DEBHARD_CFLAGS=`dpkg-buildflags --get CFLAGS`
	DEBHARD_CXXFLAGS=`dpkg-buildflags --get CXXFLAGS`
	DEBHARD_CPPFLAGS=`dpkg-buildflags --get CPPFLAGS`
	DEBHARD_LDFLAGS=`dpkg-buildflags --get LDFLAGS`
fi

# Set variables using shell commands
GIT_VERSION=${PACKAGE_VERSION}
AC_SUBST(GIT_VERSION)
MACHTYPE=`uname -m`
AC_SUBST(MACHTYPE)

## Add --enable-werror option to ./configure,
## To enable/disable treating compiler warnings as errors.
## If not specified AND we're compiling from .git repository,
## enable this automatically.
AC_ARG_ENABLE([werror],
  [AS_HELP_STRING([--enable-werror],
     [treat compiler warnings as errors (for developers)])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for werror option]) ;;
   esac
   werror=$enableval],
   []
)
if test "$werror" = yes; then
  WERROR_CFLAGS="-Werror"
  AC_SUBST([WERROR_CFLAGS])
fi

# Add pThread related settings,
#  see http://www.gnu.org/software/autoconf-archive/ax_pthread.html
# NOTE: CFLAGS/CXXFLAGS are also updated below.
LIBS="$PTHREAD_LIBS $LIBS"

# Compiler warnings to add
COMPILER_WARNINGS="-Wall -Wextra -Wswitch-default -Wno-strict-aliasing"

# Add an additional flag if using CLANG
AC_MSG_CHECKING([if compiling with clang])

AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([], [[
#ifndef __clang__
       not clang
#endif
]])],
[CLANG=yes], [CLANG=no])
AC_MSG_RESULT([$CLANG])

if test "x$CLANG" = "xyes"; then
   COMPILER_WARNINGS="$COMPILER_WARNINGS -Wno-error=unused-command-line-argument"
fi

# Custom Flags to add to CFLAGS/CXXFLAGS
LOBSTR_CFLAGS="-D_GIT_VERSION=\"\\\"${GIT_VERSION}\\\"\" -D_MACHTYPE=\"\\\"${MACHTYPE}\\\"\""

# Set the final value for CFLAGS/CXXFLAGS
CFLAGS="$CFLAGS $COMPILER_WARNINGS $PTHREAD_CFLAGS $LOBSTR_CFLAGS"
CXXFLAGS="$CXXFLAGS $COMPILER_WARNINGS $PTHREAD_CFLAGS $LOBSTR_CFLAGS $BOOST_CPPFLAGS"

# Add Debian-Hardning flags
if test "x$enable_debian_hardening" = "xyes" ; then
	CFLAGS="$CFLAGS $DEBHARD_CFLAGS"
	CXXFLAGS="$CXXFLAGS $DEBHARD_CXXFLAGS"
	CPPFLAGS="$CPPFLAGS $DEBHARD_CPPFLAGS"
	LDFLAGS="$LDFLAGS $DEBHARD_LDFLAGS"
fi

AC_CONFIG_FILES([
   m4/Makefile
   Makefile
   src/Makefile
   scripts/Makefile
   models/Makefile
   tests/Makefile
])

AC_OUTPUT

dnl
dnl Report Configuration
dnl
AC_MSG_RESULT([])
AC_MSG_RESULT([ Configuration summary for $PACKAGE])
AC_MSG_RESULT([    version $VERSION])
AC_MSG_RESULT([    CFLAGS:   $CFLAGS])
AC_MSG_RESULT([    CPPFLAGS: $CPPFLAGS])
AC_MSG_RESULT([    CXXFLAGS: $CXXFLAGS])
AC_MSG_RESULT([    LDFLAGS:  $LDFLAGS])
AC_MSG_RESULT([])
AC_MSG_RESULT([ Default installtion directories:])
AC_MSG_RESULT([    programs: ${prefix}/bin/ ])
AC_MSG_RESULT([    support scripts: ${prefix}/share/$PACKAGE/scripts])
AC_MSG_RESULT([    pre-computed model files: ${prefix}/share/$PACKAGE/models])
AC_MSG_RESULT([])
AC_MSG_RESULT([ To change installtion path, use:])
AC_MSG_RESULT([    ./configure --prefix NEW-PATH])
AC_MSG_RESULT([])
