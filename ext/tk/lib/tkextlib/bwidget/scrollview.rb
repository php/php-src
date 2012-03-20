#
#  tkextlib/bwidget/scrollview.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class ScrollView < TkWindow
    end
  end
end

class Tk::BWidget::ScrollView
  TkCommandNames = ['ScrollView'.freeze].freeze
  WidgetClassName = 'ScrollView'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'fill'
  end
  private :__strval_optkeys
end
