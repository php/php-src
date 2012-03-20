#
#  tkextlib/tcllib/calendar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * calendar widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::calendar', '0.9')
TkPackage.require('widget::calendar')

module Tk::Tcllib
  module Widget
    class Calendar < TkCanvas
      PACKAGE_NAME = 'widget::calendar'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::calendar')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::Widget::Calendar
  TkCommandNames = ['::widget::calendar'.freeze].freeze

  def __boolval_optkeys
    super() << 'showpast'
  end
  private :__boolval_optkeys

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def get(what = 'all')
    tk_send('get', what)
  end
end
