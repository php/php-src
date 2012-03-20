#
#  tkextlib/blt/tile/toplevel.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/toplevel'
require 'tkextlib/blt/tile.rb'

module Tk::BLT
  module Tile
    class Toplevel < Tk::Toplevel
      TkCommandNames = ['::blt::tile::toplevel'.freeze].freeze
    end
  end
end
