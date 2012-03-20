#
#  tentry widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TEntry < Tk::Entry
    end
    Entry = TEntry
  end
end

class Tk::Tile::TEntry < Tk::Entry
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::entry'.freeze].freeze
  else
    TkCommandNames = ['::tentry'.freeze].freeze
  end
  WidgetClassName = 'TEntry'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __optkey_aliases
    {:vcmd=>:validatecommand, :invcmd=>:invalidcommand}
  end
  private :__optkey_aliases

  def __boolval_optkeys
    super() << 'exportselection'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'show'
  end
  private :__strval_optkeys

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Entry, :TkEntry)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tentry.rb',
                                   :Ttk, Tk::Tile::Entry, :TkEntry)
