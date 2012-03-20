#
#  tframe widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TFrame < Tk::Frame
    end
    Frame = TFrame
  end
end

class Tk::Tile::TFrame < Tk::Frame
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::frame'.freeze].freeze
  else
    TkCommandNames = ['::tframe'.freeze].freeze
  end
  WidgetClassName = 'TFrame'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Frame, :TkFrame)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tframe.rb',
                                   :Ttk, Tk::Tile::Frame, :TkFrame)
