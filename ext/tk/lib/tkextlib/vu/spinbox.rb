#
#  ::vu::spinbox widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#  a standard spinbox (<= 8.3)
#  This is the same as the 8.4 core spinbox widget.
#
require 'tk'

if (Tk::TK_MAJOR_VERSION < 8 ||
    (Tk::TK_MAJOR_VERSION == 8 && Tk::TK_MINOR_VERSION < 4))
  # call setup script  --  <libdir>/tkextlib/vu.rb
  require 'tkextlib/vu.rb'

  Tk.tk_call('namespace', 'import', '::vu::spinbox')
end

module Tk
  module Vu
    Spinbox = Tk::Spinbox
  end
end
