#
#  tkextlib/bwidget/arrowbutton.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/button'

module Tk
  module BWidget
    class ArrowButton < Tk::BWidget::Button
    end
  end
end

class Tk::BWidget::ArrowButton
  TkCommandNames = ['ArrowButton'.freeze].freeze
  WidgetClassName = 'ArrowButton'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
