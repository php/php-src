#
#  tkextlib/bwidget/separator.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Separator < TkWindow
    end
  end
end

class Tk::BWidget::Separator
  TkCommandNames = ['Separator'.freeze].freeze
  WidgetClassName = 'Separator'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
