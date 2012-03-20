#
#  tkextlib/blt/ted.rb
#
#    *** This is alpha version, because there is no document on BLT. ***
#
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Ted
    extend TkCore

    TkCommandNames = ['::blt::ted'.freeze].freeze

    ##############################

    extend TkItemConfigMethod

    class << self
      def __item_cget_cmd(id)
        ['::blt::ted', 'cget', id]
      end
      private :__item_cget_cmd

      def __item_config_cmd(id)
        ['::blt::ted', 'configure', id]
      end
      private :__item_config_cmd

      private :itemcget_tkstring, :itemcget, :itemcget_strict
      private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

      def cget_tkstring(master, option)
        itemcget_tkstring(master, option)
      end
      def cget(master, option)
        itemcget(master, option)
      end
      def cget_strict(master, option)
        itemcget_strict(master, option)
      end
      def configure(master, slot, value=None)
        itemconfigure(master, slot, value)
      end
      def configinfo(master, slot=nil)
        itemconfiginfo(master, slot)
      end
      def current_configinfo(master, slot=nil)
        current_itemconfiginfo(master, slot)
      end
    end

    ##############################

    def self.edit(master, *args)
      tk_call('::blt::ted', 'edit', master, *args)
    end
    def self.rep(master, *args)
      tk_call('::blt::ted', 'rep', master, *args)
    end
    def self.select(master, *args)
      tk_call('::blt::ted', 'select', master, *args)
    end
  end
end
