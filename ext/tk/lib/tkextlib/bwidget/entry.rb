#
#  tkextlib/bwidget/entry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/entry'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Entry < Tk::Entry
    end
  end
end

class Tk::BWidget::Entry
  include Scrollable

  TkCommandNames = ['Entry'.freeze].freeze
  WidgetClassName = 'Entry'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helptext' << 'insertbackground'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'dragenabled' << 'dropenabled' << 'editable'
  end
  private :__boolval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys

  def invoke
    tk_send_without_enc('invoke')
    self
  end
end
