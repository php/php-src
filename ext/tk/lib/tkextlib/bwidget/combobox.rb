#
#  tkextlib/bwidget/combobox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/entry'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/listbox'
require 'tkextlib/bwidget/spinbox'

module Tk
  module BWidget
    class ComboBox < Tk::BWidget::SpinBox
    end
  end
end

class Tk::BWidget::ComboBox
  include Scrollable

  TkCommandNames = ['ComboBox'.freeze].freeze
  WidgetClassName = 'ComboBox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'autocomplete' << 'autopost'
  end
  private :__boolval_optkeys

  def get_listbox(&b)
    win = window(tk_send_without_enc('getlistbox'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def clear_value
    tk_send_without_enc('clearvalue')
    self
  end
  alias clearvalue clear_value

  def icursor(idx)
    tk_send_without_enc('icursor', idx)
  end

  def post
    tk_send_without_enc('post')
    self
  end

  def unpost
    tk_send_without_enc('unpost')
    self
  end
end
