#
#  tscale & tprogress widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TScale < Tk::Scale
    end
    Scale = TScale

    class TProgress < TScale
    end
    Progress = TProgress
  end
end

class Tk::Tile::TScale < Tk::Scale
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::scale'.freeze].freeze
  else
    TkCommandNames = ['::tscale'.freeze].freeze
  end
  WidgetClassName = 'TScale'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end

  alias identify ttk_identify
end

class Tk::Tile::TProgress < Tk::Tile::TScale
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::progress'.freeze].freeze
  else
    TkCommandNames = ['::tprogress'.freeze].freeze
  end
  WidgetClassName = 'TProgress'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Scale, :TkScale)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tscale.rb',
                                   :Ttk, Tk::Tile::Scale, :TkScale)
