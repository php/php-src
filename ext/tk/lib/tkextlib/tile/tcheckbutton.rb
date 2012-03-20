#
#  tcheckbutton widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TCheckButton < Tk::CheckButton
    end
    TCheckbutton = TCheckButton
    CheckButton  = TCheckButton
    Checkbutton  = TCheckButton
  end
end

class Tk::Tile::TCheckButton < Tk::CheckButton
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::checkbutton'.freeze].freeze
  else
    TkCommandNames = ['::tcheckbutton'.freeze].freeze
  end
  WidgetClassName = 'TCheckbutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Checkbutton,
#                            :TkCheckbutton, :TkCheckButton)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tcheckbutton.rb',
                                   :Ttk, Tk::Tile::Checkbutton,
                                   :TkCheckbutton, :TkCheckButton)
