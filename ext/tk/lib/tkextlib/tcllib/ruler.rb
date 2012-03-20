#
#  tkextlib/tcllib/ruler.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * ruler widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::ruler', '1.0')
TkPackage.require('widget::ruler')

module Tk::Tcllib
  module Widget
    class Ruler < TkWindow
      PACKAGE_NAME = 'widget::ruler'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::ruler')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::Widget::Ruler
  TkCommandNames = ['::widget::ruler'.freeze].freeze

  def __boolval_optkeys
    ['showvalues', 'outline', 'grid']
  end
  private :__boolval_optkeys

  def __numlistval_optkeys
    ['interval', 'sizes']
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

  def redraw
    tk_send('redraw')
    self
  end

  def shade(org, dest, frac)
    tk_send('shade', org, dest, frac)
  end
end
