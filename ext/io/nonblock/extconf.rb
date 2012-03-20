require 'mkmf'
target = "io/nonblock"

hdr = %w"fcntl.h"
if have_macro("O_NONBLOCK", hdr) and
    (have_macro("F_GETFL", hdr) or have_macro("F_SETFL", hdr))
  create_makefile(target)
end
