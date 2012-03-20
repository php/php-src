#
#  tkextlib/iwidgets/feedback.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Feedback < Tk::Iwidgets::Labeledwidget
    end
  end
end

class Tk::Iwidgets::Feedback
  TkCommandNames = ['::iwidgets::feedback'.freeze].freeze
  WidgetClassName = 'Feedback'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'barcolor'
  end
  private :__strval_optkeys

  def reset
    tk_call(@path, 'reset')
    self
  end

  def step(inc=1)
    tk_call(@path, 'step', inc)
    self
  end
end
