require 'mkmf'

# :stopdoc:

dir_config 'libffi'

pkg_config("libffi")
unless have_header('ffi.h')
  if have_header('ffi/ffi.h')
    $defs.push(format('-DUSE_HEADER_HACKS'))
  else
    abort "ffi.h is missing. Please install libffi."
  end
end

unless have_library('ffi') || have_library('libffi')
  abort "libffi is missing. Please install libffi."
end

have_header 'sys/mman.h'

config = File.read(RbConfig.expand(File.join($arch_hdrdir, "ruby/config.h")))
types = {"SIZE_T"=>"SSIZE_T", "PTRDIFF_T"=>nil, "INTPTR_T"=>nil}
types.each do |type, signed|
  if /^\#define\s+SIZEOF_#{type}\s+(SIZEOF_(.+)|\d+)/ =~ config
    if size = $2 and size != 'VOIDP'
      size = types.fetch(size) {size}
      $defs << format("-DTYPE_%s=TYPE_%s", signed||type, size)
    end
    if signed
      check_signedness(type.downcase, "stddef.h")
    end
  end
end

create_makefile 'fiddle'

# :startdoc:
