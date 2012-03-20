require 'mkmf'

if RbConfig::CONFIG['GCC'] == 'yes'
  (have_macro("__clang__") ? $LDFLAGS : $CFLAGS) << " -fno-defer-pop"
  $CFLAGS << " -fno-omit-frame-pointer"
end

$INSTALLFILES = [
  ["dl.h", "$(HDRDIR)"],
]

check = true
if( have_header("dlfcn.h") )
  have_library("dl")
  check &&= have_func("dlopen")
  check &&= have_func("dlclose")
  check &&= have_func("dlsym")
  have_func("dlerror")
elsif( have_header("windows.h") )
  check &&= have_func("LoadLibrary")
  check &&= have_func("FreeLibrary")
  check &&= have_func("GetProcAddress")
else
  check = false
end

if check
  config = File.read(RbConfig.expand(File.join($arch_hdrdir, "ruby/config.h")))
  types = {"SIZE_T"=>"SSIZE_T", "PTRDIFF_T"=>nil, "INTPTR_T"=>nil}
  types.each do |type, signed|
    if /^\#define\s+SIZEOF_#{type}\s+(SIZEOF_(.+)|\d+)/ =~ config
      if size = $2 and size != 'VOIDP'
        size = types.fetch(size) {size}
        $defs << format("-DDLTYPE_%s=DLTYPE_%s", signed||type, size)
      end
      if signed
        check_signedness(type.downcase, "stddef.h")
      end
    end
  end
  $defs << %[-DRUBY_VERSION=\\"#{RUBY_VERSION}\\"]
  create_makefile("dl")
end
