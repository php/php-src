#
#  tmenubutton widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TMenubutton < Tk::Menubutton
    end
    TMenuButton = TMenubutton
    Menubutton  = TMenubutton
    MenuButton  = TMenubutton
  end
end

class Tk::Tile::TMenubutton < Tk::Menubutton
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::menubutton'.freeze].freeze
  else
    TkCommandNames = ['::tmenubutton'.freeze].freeze
  end
  WidgetClassName = 'TMenubutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Menubutton,
#                            :TkMenubutton, :TkMenuButton)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tmenubutton.rb',
                                   :Ttk, Tk::Tile::Menubutton,
                                   :TkMenubutton, :TkMenuButton)
