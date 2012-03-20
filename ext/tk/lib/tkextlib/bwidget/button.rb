#
#  tkextlib/bwidget/button.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/button'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Button < Tk::Button
    end
  end
end

class Tk::BWidget::Button
  TkCommandNames = ['Button'.freeze].freeze
  WidgetClassName = 'Button'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helptext'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys
end
