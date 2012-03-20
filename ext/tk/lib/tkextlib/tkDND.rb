#
#  TkDND (Tk Drag & Drop Extension) support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkDND/setup.rb'

module Tk
  module TkDND
    autoload :DND,   'tkextlib/tkDND/tkdnd'
    autoload :Shape, 'tkextlib/tkDND/shape'
  end
end
