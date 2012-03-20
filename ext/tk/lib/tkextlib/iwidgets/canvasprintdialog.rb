#
#  tkextlib/iwidgets/canvasprintdialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Canvasprintdialog < Tk::Iwidgets::Dialog
    end
  end
end

class Tk::Iwidgets::Canvasprintdialog
  TkCommandNames = ['::iwidgets::canvasprintdialog'.freeze].freeze
  WidgetClassName = 'Canvasprintdialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def get_output
    tk_call(@path, 'getoutput')
  end

  def print
    bool(tk_call(@path, 'print'))
  end

  def refresh
    tk_call(@path, 'refresh')
    self
  end

  def set_canvas(win)
    tk_call(@path, 'setcanvas', win)
    self
  end
end
