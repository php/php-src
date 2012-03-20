#
#  tkextlib/tcllib/screenruler.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * screenruler dialog
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::screenruler', '1.1')
TkPackage.require('widget::screenruler')

module Tk::Tcllib
  module Widget
    class ScreenRuler < TkWindow
      PACKAGE_NAME = 'widget::ruler'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::screenruler')
        rescue
          ''
        end
      end
    end
    Screenruler = ScreenRuler
  end
end

class Tk::Tcllib::Widget::ScreenRuler
  TkCommandNames = ['::widget::screenruler'.freeze].freeze

  def __boolval_optkeys
    ['topmost', 'reflect']
  end
  private :__boolval_optkeys

  def __numlistval_optkeys
    ['alpha']
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

  def display
    tk_send('display')
    self
  end
  alias show display

  def hide
    tk_send('hide')
    self
  end
end
