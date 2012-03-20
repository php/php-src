#
#  tkextlib/bwidget/label.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/label'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Label < Tk::Label
    end
  end
end

class Tk::BWidget::Label
  TkCommandNames = ['Label'.freeze].freeze
  WidgetClassName = 'Label'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helptext'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'dragenabled' << 'dropenabled'
  end
  private :__boolval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys

  def set_focus
    tk_send_without_enc('setfocus')
    self
  end
end
