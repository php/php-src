#
#  tkextlib/iwidgets/extbutton.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Extbutton < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Extbutton
  TkCommandNames = ['::iwidgets::extbutton'.freeze].freeze
  WidgetClassName = 'Extbutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'bitmapforeground' << 'ringbackground'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'defaultring'
  end
  private :__boolval_optkeys

  def invoke
    tk_call(@path, 'invoke')
    self
  end

  def flash
    tk_call(@path, 'flash')
    self
  end
end
