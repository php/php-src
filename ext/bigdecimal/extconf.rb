require 'mkmf'

have_func("labs", "stdlib.h")
have_func("llabs", "stdlib.h")

create_makefile('bigdecimal')
