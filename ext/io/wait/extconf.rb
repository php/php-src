require 'mkmf'
target = "io/wait"

unless macro_defined?("DOSISH", "#include <ruby.h>")
  have_header(ioctl_h = "sys/ioctl.h") or ioctl_h = nil
  fionread = %w[sys/ioctl.h sys/filio.h sys/socket.h].find do |h|
    have_macro("FIONREAD", [h, ioctl_h].compact)
  end
  if fionread
    $defs << "-DFIONREAD_HEADER=\"<#{fionread}>\""
    create_makefile(target)
  end
else
  if have_func("rb_w32_ioctlsocket", "ruby.h")
    have_func("rb_w32_is_socket", "ruby.h")
    create_makefile(target)
  end
end
