require 'mkmf'

if compiled?("dl")
  callbacks = (0..8).map{|i| "callback-#{i}"}.unshift("callback")
  callback_srcs = callbacks.map{|basename| "#{basename}.c"}
  callback_objs = callbacks.map{|basename| "#{basename}.o"}

  $distcleanfiles << '$(SRCS)'
  $srcs = callback_srcs
  $objs = callback_objs
  $INCFLAGS << " -I$(srcdir)/.."

  create_makefile("dl/callback")
end
