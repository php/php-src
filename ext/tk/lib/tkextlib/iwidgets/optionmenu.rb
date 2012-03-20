#
#  tkextlib/iwidgets/optionmenu.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Optionmenu < Tk::Iwidgets::Labeledwidget
    end
  end
end

class Tk::Iwidgets::Optionmenu
  TkCommandNames = ['::iwidgets::optionmenu'.freeze].freeze
  WidgetClassName = 'Optionmenu'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'cyclicon'
  end
  private :__boolval_optkeys

  def delete(first, last=nil)
    if last
      tk_call(@path, 'delete', first, last)
    else
      tk_call(@path, 'delete', first)
    end
    self
  end

  def disable(idx)
    tk_call(@path, 'disable', idx)
    self
  end

  def enable(idx)
    tk_call(@path, 'enable', idx)
    self
  end

  def get(first=nil, last=nil)
    if last
      simplelist(tk_call(@path, 'get', first, last))
    elsif first
      tk_call(@path, 'get', first)
    else
      tk_call(@path, 'get')
    end
  end
  def get_range(first, last)
    get(first, last)
  end
  def get_selected
    get()
  end

  def index(idx)
    number(tk_call(@path, 'index', idx))
  end

  def insert(idx, *args)
    tk_call(@path, 'insert', idx, *args)
    self
  end

  def select(idx)
    tk_call(@path, 'select', idx)
    self
  end

  def sort(*params, &b)
    # see 'lsort' man page about params
    if b
      tk_call(@path, 'sort', '-command', proc(&b), *params)
    else
      tk_call(@path, 'sort', *params)
    end
    self
  end
  def sort_ascending
    tk_call(@path, 'sort', 'ascending')
    self
  end
  def sort_descending
    tk_call(@path, 'sort', 'descending')
    self
  end
end
