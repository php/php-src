#
#  tkextlib/iwidgets/fileselectiondialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Fileselectiondialog < Tk::Iwidgets::Dialog
    end
  end
end

class Tk::Iwidgets::Fileselectiondialog
  TkCommandNames = ['::iwidgets::fileselectiondialog'.freeze].freeze
  WidgetClassName = 'Fileselectiondialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def filter
    tk_call(@path, 'filter')
    self
  end

  def get
    tk_call(@path, 'get')
  end
end
