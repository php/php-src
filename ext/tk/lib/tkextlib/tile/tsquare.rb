#
#  tsquare widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TSquare < TkWindow
    end
    Square = TSquare
  end
end

class Tk::Tile::TSquare < TkWindow
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::square'.freeze].freeze
  else
    TkCommandNames = ['::tsquare'.freeze].freeze
  end
  WidgetClassName = 'TSquare'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end
