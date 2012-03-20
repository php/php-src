#
#  tkextlib/iwidgets/shell.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Shell < Tk::Itk::Toplevel
    end
  end
end

class Tk::Iwidgets::Shell
  TkCommandNames = ['::iwidgets::shell'.freeze].freeze
  WidgetClassName = 'Shell'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def activate
    tk_call(@path, 'activate')  # may return val of deactibate method
  end

  def center(win=None)
    tk_call(@path, 'center', win)
    self
  end

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def deactivate(val=None)
    tk_call(@path, 'deactivate', val)
    self
  end
end
