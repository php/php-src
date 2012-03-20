require 'mkmf'

$defs << "-D""BADMESS=0"
create_makefile("sdbm")
