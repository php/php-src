#
#  tkextlib/iwidgets/dialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Dialog < Tk::Iwidgets::Dialogshell
    end
  end
end

class Tk::Iwidgets::Dialog
  TkCommandNames = ['::iwidgets::dialog'.freeze].freeze
  WidgetClassName = 'Dialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
