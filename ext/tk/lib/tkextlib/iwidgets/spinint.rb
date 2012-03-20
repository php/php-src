#
#  tkextlib/iwidgets/spinint.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Spinint < Tk::Iwidgets::Spinner
    end
  end
end

class Tk::Iwidgets::Spinint
  TkCommandNames = ['::iwidgets::spinint'.freeze].freeze
  WidgetClassName = 'Spinint'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'wrap'
  end
  private :__boolval_optkeys

  def __numlistval_optkeys
    super() << 'range'
  end
  private :__numlistval_optkeys
end
