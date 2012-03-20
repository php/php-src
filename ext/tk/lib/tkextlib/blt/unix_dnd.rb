#
#  tkextlib/blt/unix_dnd.rb
#
#    *** This is alpha version, because there is no document on BLT. ***
#
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module DnD
    extend TkCore

    TkCommandNames = ['::blt::dnd'.freeze].freeze

    ##############################

    extend TkItemConfigMethod

    class << self
      def __item_cget_cmd(id)
        ['::blt::dnd', *id]
      end
      private :__item_cget_cmd

      def __item_config_cmd(id)
        ['::blt::dnd', *id]
      end
      private :__item_config_cmd

      private :itemcget_tkstring, :itemcget, :itemcget_strict
      private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

      def cget_tkstring(win, option)
        itemcget_tkstring(['cget', win], option)
      end
      def cget(win, option)
        itemcget(['cget', win], option)
      end
      def cget_strict(win, option)
        itemcget_strict(['cget', win], option)
      end
      def configure(win, slot, value=None)
        itemconfigure(['configure', win], slot, value)
      end
      def configinfo(win, slot=nil)
        itemconfiginfo(['configure', win], slot)
      end
      def current_configinfo(win, slot=nil)
        current_itemconfiginfo(['configure', win], slot)
      end

      def token_cget_tkstring(win, option)
        itemcget_tkstring(['token', 'cget', win], option)
      end
      def token_cget(win, option)
        itemcget(['token', 'cget', win], option)
      end
      def token_cget_strict(win, option)
        itemcget_strict(['token', 'cget', win], option)
      end
      def token_configure(win, slot, value=None)
        itemconfigure(['token', 'configure', win], slot, value)
      end
      def token_configinfo(win, slot=nil)
        itemconfiginfo(['token', 'configure', win], slot)
      end
      def current_token_configinfo(win, slot=nil)
        current_itemconfiginfo(['token', 'configure', win], slot)
      end

      def token_windowconfigure(win, slot, value=None)
        itemconfigure(['token', 'window', win], slot, value)
      end
      def token_windowconfiginfo(win, slot=nil)
        itemconfiginfo(['token', 'window', win], slot)
      end
      def current_token_windowconfiginfo(win, slot=nil)
        current_itemconfiginfo(['token', 'window', win], slot)
      end
    end

    ##############################

    def self.cancel(win)
      tk_call('::blt::dnd', 'cancel', *wins)
    end
    def self.delete(*wins)
      tk_call('::blt::dnd', 'delete', *wins)
    end
    def self.delete_source(*wins)
      tk_call('::blt::dnd', 'delete', '-source', *wins)
    end
    def self.delete_target(*wins)
      tk_call('::blt::dnd', 'delete', '-target', *wins)
    end
    def self.drag(win, x, y, token=None)
      tk_call('::blt::dnd', 'drag', win, x, y, token)
    end
    def self.drop(win, x, y, token=None)
      tk_call('::blt::dnd', 'drop', win, x, y, token)
    end
    def self.get_data(win, fmt=nil, cmd=nil)
      if fmt
        tk_call('::blt::dnd', 'getdata', win, fmt, cmd)
      else
        list(tk_call('::blt::dnd', 'getdata', win))
      end
    end
    def self.names(pat=None)
      list(tk_call('::blt::dnd', 'names', pat))
    end
    def self.source_names(pat=None)
      list(tk_call('::blt::dnd', 'names', '-source', pat))
    end
    def self.target_names(pat=None)
      list(tk_call('::blt::dnd', 'names', '-target', pat))
    end
    def self.pull(win, fmt)
      tk_call('::blt::dnd', 'pull', win, fmt)
    end
    def self.register(win, keys={})
      tk_call('::blt::dnd', 'register', win, keys)
    end
    def self.select(win, x, y, timestamp)
      tk_call('::blt::dnd', 'select', win, x, y, timestamp)
    end
    def self.set_data(win, fmt=nil, cmd=nil)
      if fmt
        tk_call('::blt::dnd', 'setdata', win, fmt, cmd)
      else
        list(tk_call('::blt::dnd', 'setdata', win))
      end
    end
    def self.token(*args)
      tk_call('::blt::dnd', 'token', *args)
    end
  end
end
