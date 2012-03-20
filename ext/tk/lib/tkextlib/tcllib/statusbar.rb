#
#  tkextlib/tcllib/statusbar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * statusbar widget
#

require 'tk'
require 'tkextlib/tcllib.rb'

# TkPackage.require('widget::statusbar', '1.2')
TkPackage.require('widget::statusbar')

module Tk::Tcllib
  module Widget
    class Statusbar < Tk::Tile::TFrame
      PACKAGE_NAME = 'widget::statusbar'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('widget::statusbar')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::Widget::Statusbar
  TkCommandNames = ['::widget::statusbar'.freeze].freeze

  def __boolval_optkeys
    super() << 'separator' << 'resize' << 'resizeseparator'
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

  def getframe
    window(tk_send_without_enc('getframe'))
  end
  alias get_frame getframe

  def add(w, keys={})
    window(tk_send_without_enc('setwidget', *(hash_kv(keys))))
  end

  def remove(*wins)
    tk_send_without_enc('remove', *wins)
    self
  end

  def remove_with_destroy(*wins)
    tk_send_without_enc('remove', '-destroy', *wins)
    self
  end

  def delete(*wins)
    tk_send_without_enc('delete', *wins)
    self
  end

  def items(pat=None)
    tk_split_list(tk_send('items', pat))
  end
end
