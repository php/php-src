#
#  tkextlib/tcllib/scrolledwindow.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * scrolledwindow widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::scrolledwindow', '1.2')
TkPackage.require('widget::scrolledwindow')

module Tk::Tcllib
  module Widget
    class Scrolledwindow < Tk::Tile::TFrame
      PACKAGE_NAME = 'widget::scrolledwindow'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::scrolledwindow')
        rescue
          ''
        end
      end
    end
    ScrolledWindow = Scrolledwindow
  end
end

class Tk::Tcllib::Widget::ScrolledWindow
  TkCommandNames = ['::widget::scrolledwindow'.freeze].freeze

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def getframe
    window(tk_send_without_enc('getframe'))
  end
  alias get_frame getframe

  def setwidget(w)
    window(tk_send_without_enc('setwidget', w))
  end
  alias set_widget setwidget
end
