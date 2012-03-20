#
#  tkextlib/iwidgets/disjointlistbox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Disjointlistbox < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Disjointlistbox
  TkCommandNames = ['::iwidgets::disjointlistbox'.freeze].freeze
  WidgetClassName = 'Disjointlistbox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'lhslabeltext' << 'rhslabeltext' << 'lhsbuttonlabel' << 'rhsbuttonlabel'
  end
  private :__strval_optkeys

  def set_lhs(*items)
    tk_call(@path, 'setlhs', items)
    self
  end
  def set_rhs(*items)
    tk_call(@path, 'setrhs', items)
    self
  end

  def get_lhs
    simplelist(tk_call(@path, 'getlhs'))
  end
  def get_rhs
    simplelist(tk_call(@path, 'getrhs'))
  end

  def insert_lhs(*items)
    tk_call(@path, 'insertlhs', items)
    self
  end
  def insert_rhs(*items)
    tk_call(@path, 'insertrhs', items)
    self
  end
end
