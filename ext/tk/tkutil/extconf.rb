begin
  has_tk = compiled?('tk')
rescue NoMethodError
  # Probably, called manually (NOT from 'extmk.rb'). Force to make Makefile.
  has_tk = true
end

if has_tk
  require 'mkmf'

  have_func("rb_obj_instance_exec", "ruby.h")
  have_func("rb_obj_untrust", "ruby.h")
  have_func("rb_obj_taint", "ruby.h")
  have_func("rb_sym_to_s", "ruby.h")
  have_func("strndup", "string.h")

  create_makefile('tkutil')
end
