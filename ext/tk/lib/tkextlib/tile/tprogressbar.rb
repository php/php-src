#
#  tprogressbar widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TProgressbar < TkWindow
    end
    Progressbar = TProgressbar
  end
end

class Tk::Tile::TProgressbar
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    TkCommandNames = ['::ttk::progressbar'.freeze].freeze
  else
    TkCommandNames = ['::tprogressbar'.freeze].freeze
  end
  WidgetClassName = 'TProgressbar'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end

  def step(amount=None)
    tk_send_without_enc('step', amount).to_f
  end
  #def step=(amount)
  #  tk_send_without_enc('step', amount)
  #end

  def start(interval=None)
    if Tk::Tile::TILE_SPEC_VERSION_ID < 5
      tk_call_without_enc('::tile::progressbar::start', @path, interval)
    else
      tk_send_without_enc('start', interval)
    end
  end

  def stop(amount=None)
    if Tk::Tile::TILE_SPEC_VERSION_ID < 5
      tk_call_without_enc('::tile::progressbar::stop', @path)
    else
      tk_send_without_enc('stop', amount)
    end
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Progressbar, :TkProgressbar)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tprogressbar.rb',
                                   :Ttk, Tk::Tile::Progressbar, :TkProgressbar)
