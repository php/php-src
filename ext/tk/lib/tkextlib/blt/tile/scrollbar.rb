#
#  tkextlib/blt/tile/scrollbar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/scrollbar'
require 'tkextlib/blt/tile.rb'

module Tk::BLT
  module Tile
    class Scrollbar < Tk::Scrollbar
      TkCommandNames = ['::blt::tile::scrollbar'.freeze].freeze
    end
  end
end
