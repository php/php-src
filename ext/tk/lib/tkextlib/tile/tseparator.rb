#
#  tseparator widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TSeparator < TkWindow
    end
    Separator = TSeparator
  end
end

class Tk::Tile::TSeparator < TkWindow
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::separator'.freeze].freeze
  else
    TkCommandNames = ['::tseparator'.freeze].freeze
  end
  WidgetClassName = 'TSeparator'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Separator, :TkSeparator)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tseparator.rb',
                                   :Ttk, Tk::Tile::Separator, :TkSeparator)
