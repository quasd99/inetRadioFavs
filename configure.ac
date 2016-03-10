#####################################################
# setup for gtkmm 3 Projects
#####################################################

AC_INIT([iradiofavs], [0.01], [niessen.frank@gmx.de], [iradiofavs], [http://www.soundicion.de])
AC_PREREQ([2.69])
AC_CHECK_FUNCS([asprintf])
AC_CONFIG_SRCDIR([src/iradiofavs.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 subdir-objects -Wall -Werror])
AC_CONFIG_FILES([Makefile])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

# CFLAGS and LIBS
PKG_CHECK_MODULES([iradiofavs], [gtkmm-3.0 >= 3.8.0])

# m4 convention
AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT
