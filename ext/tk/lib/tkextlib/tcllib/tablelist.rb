#
#  tkextlib/tcllib/tablelist.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * A multi-column listbox

require 'tk'
require 'tkextlib/tcllib.rb'

# check Tile extension :: If already loaded, use tablelist_tile.
unless defined? Tk::Tcllib::Tablelist_usingTile
  Tk::Tcllib::Tablelist_usingTile =
    TkPackage.provide('tile') || TkPackage.provide('Ttk')
end

if Tk::Tcllib::Tablelist_usingTile
  # with Tile
  require 'tkextlib/tcllib/tablelist_tile'

else
  # without Tile

  # TkPackage.require('tablelist', '4.2')
  TkPackage.require('tablelist')

  require 'tkextlib/tcllib/tablelist_core'
end
