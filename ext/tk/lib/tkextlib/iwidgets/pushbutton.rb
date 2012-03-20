#
#  tkextlib/iwidgets/pushbutton.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Pushbutton < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Pushbutton
  TkCommandNames = ['::iwidgets::pushbutton'.freeze].freeze
  WidgetClassName = 'Pushbutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'defaultring'
  end
  private :__boolval_optkeys

  def invoke
    tk_call_without_enc(@path, 'invoke')
    self
  end

  def flash
    tk_call_without_enc(@path, 'flash')
    self
  end
end
