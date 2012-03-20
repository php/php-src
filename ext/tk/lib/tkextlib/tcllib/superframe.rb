#
#  tkextlib/tcllib/superframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Superframe widget - enhanced labelframe widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::superframe', '1.0')
TkPackage.require('widget::superframe')

module Tk::Tcllib
  module Widget
    class SuperFrame < TkWindow
      PACKAGE_NAME = 'widget::superframe'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::superframe')
        rescue
          ''
        end
      end
    end
    Superframe = SuperlFrame
  end
end

class Tk::Tcllib::Widget::SuperFrame
  TkCommandNames = ['::widget::superframe'.freeze].freeze

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def labelwidget
    window(tk_send('labelwidget'))
  end
end
