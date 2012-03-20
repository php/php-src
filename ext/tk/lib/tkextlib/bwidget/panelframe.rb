#
#  tkextlib/bwidget/panelframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/frame'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class PanelFrame < TkWindow
    end
  end
end

class Tk::BWidget::PanelFrame
  TkCommandNames = ['PanelFrame'.freeze].freeze
  WidgetClassName = 'PanelFrame'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() + ['panelforeground', 'panelbackground']
  end
  private :__strval_optkeys

  def add(win, keys={})
    tk_send('add', win, keys)
    self
  end

  def delete(*wins)
    tk_send('delete', *wins)
    self
  end

  def get_frame(&b)
    win = window(tk_send_without_enc('getframe'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def items
    simplelist(tk_send('items')).map{|w| window(w)}
  end

  def remove(*wins)
    tk_send('remove', *wins)
    self
  end

  def remove_with_destroy(*wins)
    tk_send('remove', '-destroy', *wins)
    self
  end

  def delete(*wins) # same to 'remove_with_destroy'
    tk_send('delete', *wins)
    self
  end
end
