#
#  tkextlib/iwidgets/timeentry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Timeentry < Tk::Iwidgets::Timefield
    end
  end
end

class Tk::Iwidgets::Timeentry
  TkCommandNames = ['::iwidgets::timeentry'.freeze].freeze
  WidgetClassName = 'Timeentry'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'closetext'
  end
  private :__strval_optkeys
end
