#
#  tkextlib/iwidgets/labeledframe.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Labeledframe < Tk::Itk::Archetype
    end
  end
end

class Tk::Iwidgets::Labeledframe
  TkCommandNames = ['::iwidgets::labeledframe'.freeze].freeze
  WidgetClassName = 'Labeledframe'.freeze
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

  def child_site
    window(tk_call(@path, 'childsite'))
  end
end
