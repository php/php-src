#
#  tkextlib/blt/busy.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/itemconfig.rb'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Busy
    extend TkCore
    extend TkItemConfigMethod

    TkCommandNames = ['::blt::busy'.freeze].freeze

    ###########################

    class Shield < TkWindow
      def self.shield_path(win)
        win = window(win) unless win.kind_of?(TkWindow)
        if win.kind_of?(Tk::Toplevel)
          win.path + '._Busy'
        else
          win.path + '_Busy'
        end
      end

      def initialize(win)
        @path = self.class.shield_path(win)
      end
    end

    def self.shield_path(win)
      Tk::BLT::Busy::Shield.shield_path(win)
    end
  end
end

class << Tk::BLT::Busy
  def __item_config_cmd(win)
    ['::blt::busy', 'configure', win]
  end
  private :__item_config_cmd

  undef itemcget
  undef itemcget_tkstring
  alias configure itemconfigure
  alias configinfo itemconfiginfo
  alias current_configinfo current_itemconfiginfo
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  ##################################

  def hold(win, keys={})
    tk_call('::blt::busy', 'hold', win, *hash_kv(keys))
  end

  def release(*wins)
    tk_call('::blt::busy', 'release', *wins)
  end

  def forget(*wins)
    tk_call('::blt::busy', 'forget', *wins)
  end

  def is_busy(pat=None)
    tk_split_list(tk_call('::blt::busy', 'isbusy', pat))
  end

  def names(pat=None)
    tk_split_list(tk_call('::blt::busy', 'names', pat))
  end
  alias windows names

  def check(win)
    bool(tk_call('::blt::busy', 'check', win))
  end

  def status(win)
    bool(tk_call('::blt::busy', 'status', win))
  end
end
