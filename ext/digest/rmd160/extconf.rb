# $RoughId: extconf.rb,v 1.3 2001/08/14 19:54:51 knu Exp $
# $Id$

require "mkmf"

$defs << "-DNDEBUG" << "-DHAVE_CONFIG_H"
$INCFLAGS << " -I$(srcdir)/.."

$objs = [ "rmd160init.#{$OBJEXT}" ]

dir_config("openssl")

if !with_config("bundled-rmd160") &&
    have_library("crypto") && have_header("openssl/ripemd.h")
  $objs << "rmd160ossl.#{$OBJEXT}"
else
  $objs << "rmd160.#{$OBJEXT}"
end

have_header("sys/cdefs.h")

$preload = %w[digest]

if try_compile("", flag = " -Wno-deprecated-declarations")
  $warnflags << flag
end
create_makefile("digest/rmd160")
