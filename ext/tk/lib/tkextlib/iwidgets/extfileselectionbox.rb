#
#  tkextlib/iwidgets/extfileselectionbox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Extfileselectionbox < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Extfileselectionbox
  TkCommandNames = ['::iwidgets::extfileselectionbox'.freeze].freeze
  WidgetClassName = 'Extfileselectionbox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() + [
      'dirslabel', 'fileslabel', 'filterlabel', 'mask', 'nomatchstring',
      'selectionlabel'
    ]
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() + ['dirson', 'fileson', 'filteron', 'selectionon']
  end
  private :__boolval_optkeys

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
