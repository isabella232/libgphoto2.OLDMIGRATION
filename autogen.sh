#!/bin/sh
# Run this to generate all the initial makefiles, etc.
# This was lifted from the Gimp, and adapted slightly by
# Raph Levien.
# Since then, it has been rewritten quite a lot by misc. people.

# Call this file with AUTOCONF_SUFFIX and AUTOMAKE_SUFFIX set
# if you want us to call a specific version of autoconf or automake. 
# E.g. if you want us to call automake-1.6 instead of automake (which
# seems to be quite advisable if your automake is not already version 
# 1.6) then call this file with AUTOMAKE_SUFFIX set to "-1.6".

# Cases which are known to work:
# 2002-07-14: Debian GNU/Linux unstable with:
#   autoconf 2.53
#   automake 1.4-p5 and 1.6 (both)
#   gettext  0.10.40
#   libtool  1.4.2a
# 2002-07-14: Redhat Linux 7.3 with:
#   autoconf 2.53
#   automake 1.5 (not 1.4-p5)
#   gettext  0.11.1
#   libtool  1.4.2

DIE=0
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
test "$srcdir" = "." && srcdir=`pwd`

PROJECT=libgphoto2

# failure subroutine.
# syntax: do-something || fail
fail() {
    status=$?
    echo "Last command failed with status $status in directory $(pwd)."
    echo "Aborting."
    exit $status
}

(autoconf${AUTOCONF_SUFFIX} --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have autoconf installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/gnu/autoconf/"
    DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have libtool installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/gnu/libtool/"
    DIE=1
}

(automake${AUTOMAKE_SUFFIX} --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have automake installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/gnu/automake/"
    DIE=1
}

(gettextize --version) < /dev/null > /dev/null 2>&1 || {
    echo
    echo "You must have gettext installed to compile $PROJECT."
    echo "Download the appropriate package for your distribution,"
    echo "or get the source tarball at ftp://ftp.gnu.org/gnu/gettext/"
    DIE=1
}

if test "$DIE" -eq 1; then
    exit 1
fi

test -f libgphoto2/gphoto2.h || {
        echo "You must run this script in the top-level $PROJECT directory"
        exit 1
}

case "$CC" in
*xlc | *xlc\ * | *lcc | *lcc\ *) am_opt=--include-deps;;
esac

ACLOCAL_FLAGS="-I ${srcdir}/libgphoto2_port/m4 ${ACLOCAL_FLAGS}"

gettext_version=`gettextize --version 2>&1 | sed -n 's/^.*GNU gettext.* \([0-9]*\.[0-9.]*\).*$/\1/p'`
case $gettext_version in
0.10.*)
	;;
	
*)
	gettext_opt="$gettext_opt --intl";;
esac

# Clean automake stuff; must be done *before* subprojects
find . -name '*.am' | 
while read file
do # remove all .in files with a corresponding .am file
	rm -f $(echo "$file" | sed s/\.am\$/.in/g)
done

# do not change the sequence without understanding the implications
for dir in libgphoto2_port .
do 
(
    cd "$dir" || fail
    echo "Processing ${dir} aka $(pwd)"

    # We explicitly delete files and directories which are not contained 
    # in the CVS repository and which are generated by the build tools
    # called here.
    # This makes changing build tool versions much easier, and removes
    # the need for any --force parameters to the build tools which may
    # do something we do want or something we do not.

    echo "Cleaning stuff generated by gettextize"
    rm -f po/Makevars.template po/Makevars po/Makefile.in.in ABOUT-NLS
    for file in codeset.m4 gettext.m4 glibc21.m4 iconv.m4 isc-posix.m4 \
		lcmessage.m4 progtest.m4
    do
	rm -f "m4/$file"
    done
    rm -rf intl/
    grep "intl/Makefile" configure.in > /dev/null &&
    ( sed -e 's#^AC_OUTPUT(\[.*intl/Makefile po/Makefile.in#AC_OUTPUT(\[#' \
    configure.in >configure.in.new && mv configure.in.new configure.in )
    grep "SUBDIRS.*intl" Makefile.am > /dev/null &&
    ( sed -e 's#\(^SUBDIRS.*\) intl#\1#' Makefile.am >Makefile.am.new \
    && mv Makefile.am.new Makefile.am )

    echo "Running gettextize $gettext_opt"
    echo "########################################################################"
    echo "# If gettextize asks you to acknowledge some stuff and press Return,   #"
    echo "# just do that. autogen.sh already does what gettextize wants done.    #"
    echo "########################################################################"
    gettextize $gettext_opt || fail
    if test -f po/Makevars.template
    then
	cp po/Makevars.template po/Makevars
    fi

    case $gettext_version in
    0.11.?)
    	;;
    *)
    	grep "intl/Makefile" configure.in > /dev/null ||
	( sed -e 's#^AC_OUTPUT(\[#AC_OUTPUT(\[ intl/Makefile po/Makefile.in#' \
	configure.in >configure.in.new && mv configure.in.new configure.in )
	grep "SUBDIRS.*intl" Makefile.am > /dev/null ||
	( sed -e 's#\(^SUBDIRS.*\)$#\1 intl#' Makefile.am >Makefile.am.new \
	&& mv Makefile.am.new Makefile.am )
    esac

    echo "Cleaning stuff generated by libtoolize"
    rm -f ltmain.sh config.guess config.sub

    echo "Running libtoolize"
    libtoolize || fail

    echo "Cleaning stuff generated by aclocal"
    rm -f aclocal.m4

    echo "Running aclocal${AUTOMAKE_SUFFIX} $ACLOCAL_FLAGS"
    aclocal${AUTOMAKE_SUFFIX} $ACLOCAL_FLAGS || fail

    echo "Cleaning stuff generated by autoheader"
    rm -f config.h.in libgphoto2/config.h.in

    echo "Running autoheader${AUTOCONF_SUFFIX}"
    autoheader${AUTOCONF_SUFFIX} || fail

    echo "Cleaning stuff generated by automake"
    rm -f depcomp install-sh missing mkinstalldirs
    rm -f stamp-h* libgphoto2/stamp-h*

    echo "Running automake${AUTOMAKE_SUFFIX} --add-missing --gnu $am_opt"
    automake${AUTOMAKE_SUFFIX} --add-missing --gnu $am_opt || fail

    echo "Cleaning stuff generated by autoconf"
    rm -f configure
    rm -rf autom4te*.cache/

    echo "Running autoconf${AUTOCONF_SUFFIX}"
    autoconf${AUTOCONF_SUFFIX} || fail

    echo "Finished in directory $(pwd)"
) || fail
done

echo 
echo "$PROJECT is now ready for configuration."
