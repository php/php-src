#
#  tkextlib/iwidgets/labeledwidget.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Labeledwidget < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Labeledwidget
  extend TkCore

  TkCommandNames = ['::iwidgets::labeledwidget'.freeze].freeze
  WidgetClassName = 'Labeledwidget'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'labeltext'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'labelvariable'
  end
  private :__tkvariable_optkeys

  def __font_optkeys
    super() << 'labelfont'
  end
  private :__font_optkeys

  def self.alignlabels(*wins)
    tk_call('::iwidgets::Labeledwidget::alignlabels', *wins)
  end

  def child_site
    window(tk_call(@path, 'childsite'))
  end
end
