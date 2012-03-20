require 'mkmf'
$INCFLAGS << " -I$(topdir) -I$(top_srcdir)"
create_makefile('coverage')
