require 'mkmf'

$defs << "-DHAVE_CONFIG_H"
$INCFLAGS << " -I$(srcdir)/.."

create_makefile('digest/bubblebabble')
