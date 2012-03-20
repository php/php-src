#
#  tkextlib/iwidgets/dateentry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Dateentry < Tk::Iwidgets::Datefield
    end
  end
end

class Tk::Iwidgets::Dateentry
  TkCommandNames = ['::iwidgets::dateentry'.freeze].freeze
  WidgetClassName = 'Dateentry'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
