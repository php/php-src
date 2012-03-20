#
#  tkextlib/tcllib/dialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Generic dialog widget (themed)
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::dialog', '1.2')
TkPackage.require('widget::dialog')

module Tk::Tcllib
  module Widget
    class Dialog < TkWindow
      PACKAGE_NAME = 'widget::dialog'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::dialog')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::Widget::Dialog
  TkCommandNames = ['::widget::dialog'.freeze].freeze

  def __boolval_optkeys
    ['separator', 'synchronous', 'transient']
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

  def add(what, *args)
    window(tk_send('add', *args))
  end

  def get_frame
    window(tk_send('getframe'))
  end

  def set_widget(widget)
    tk_send('setwidget', widget)
    self
  end

  def display
    tk_send('display')
    self
  end
  alias show display

  def cancel
    tk_send('cancel')
    self
  end

  def close(reason = None)
    tk_send('close', reason)
  end

  def withdraw
    tk_send('withdraw')
    self
  end
end
