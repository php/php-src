#
#  tkextlib/tcllib/menuentry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * menuentry widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::menuentry', '1.0')
TkPackage.require('widget::menuentry')

module Tk::Tcllib
  module Widget
    class Menuentry < Tk::Tile::TEntry
      PACKAGE_NAME = 'widget::menuentry'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::menuentry')
        rescue
          ''
        end
      end
    end
    MenuEntry = Menuentry
  end
end

class Tk::Tcllib::Widget::Menuentry
  TkCommandNames = ['::widget::menuentry'.freeze].freeze

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self
end
