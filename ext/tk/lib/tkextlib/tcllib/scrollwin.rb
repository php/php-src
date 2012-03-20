#
#  tkextlib/tcllib/scrollwin.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Scrolled widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::scrolledwindow', '1.0')
TkPackage.require('widget::scrolledwindow')

module Tk::Tcllib
  module Widget
    class ScrolledWindow < TkWindow
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
    Scrolledwindow = ScrolledWindow
  end
end

class Tk::Tcllib::Widget::ScrolledWindow
  TkCommandNames = ['::widget::scrolledwindow'.freeze].freeze

  def __numlistval_optkeys
    ['ipad']
  end
  private :__numlistval_optkeys

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def get_frame
    window(tk_send('getframe'))
  end

  def set_widget(widget)
    tk_send('setwidget', widget)
    self
  end
end
