#
#  tkextlib/blt/tile.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Tile
    TkComm::TkExtlibAutoloadModule.unshift(self)
    # Require autoload-symbols which is a same name as widget classname.
    # Those are used at  TkComm._genobj_for_tkwidget method.

    autoload :Button,      'tkextlib/blt/tile/button.rb'
    autoload :CheckButton, 'tkextlib/blt/tile/checkbutton.rb'
    autoload :Checkbutton, 'tkextlib/blt/tile/checkbutton.rb'
    autoload :Radiobutton, 'tkextlib/blt/tile/radiobutton.rb'
    autoload :RadioButton, 'tkextlib/blt/tile/radiobutton.rb'
    autoload :Frame,       'tkextlib/blt/tile/frame.rb'
    autoload :Label,       'tkextlib/blt/tile/label.rb'
    autoload :Scrollbar,   'tkextlib/blt/tile/scrollbar.rb'
    autoload :Toplevel,    'tkextlib/blt/tile/toplevel.rb'
  end
end
