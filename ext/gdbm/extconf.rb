require 'mkmf'

dir_config("gdbm")
if have_library("gdbm", "gdbm_open") and
   have_header("gdbm.h")
  create_makefile("gdbm")
end
