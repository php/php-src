#
#  tkextlib/iwidgets/messagedialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Messagedialog < Tk::Iwidgets::Dialog
    end
  end
end

class Tk::Iwidgets::Messagedialog
  TkCommandNames = ['::iwidgets::messagedialog'.freeze].freeze
  WidgetClassName = 'Messagedialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
