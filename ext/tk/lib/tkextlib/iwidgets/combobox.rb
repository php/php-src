#
#  tkextlib/iwidgets/combobox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Combobox < Tk::Iwidgets::Entryfield
    end
  end
end

class Tk::Iwidgets::Combobox
  TkCommandNames = ['::iwidgets::combobox'.freeze].freeze
  WidgetClassName = 'Combobox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __boolval_optkeys
    super() << 'completion' << 'dropdown' << 'editable' << 'unique'
  end
  private :__boolval_optkeys

  def clear(component=None)
    tk_call(@path, 'clear', component)
    self
  end

  def delete_list(first, last=None)
    tk_call(@path, 'delete', 'list', first, last)
    self
  end

  def delete_entry(first, last=None)
    tk_call(@path, 'delete', 'entry', first, last)
    self
  end

  def get_list_contents(index)
    tk_call(@path, 'get', index)
  end

  def insert_list(idx, *elems)
    tk_call(@path, 'insert', 'list', idx, *elems)
    self
  end

  def insert_entry(idx, *elems)
    tk_call(@path, 'insert', 'entry', idx, *elems)
    self
  end

  # listbox methods
  def size
    tk_send_without_enc('size').to_i
  end
  def see(index)
    tk_send_without_enc('see', index)
    self
  end
  def selection_anchor(index)
    tk_send_without_enc('selection', 'anchor', index)
    self
  end
  def selection_clear(first, last=None)
    tk_send_without_enc('selection', 'clear', first, last)
    self
  end
  def selection_includes(index)
    bool(tk_send_without_enc('selection', 'includes', index))
  end
  def selection_set(first, last=None)
    tk_send_without_enc('selection', 'set', first, last)
    self
  end

  # scrolledlistbox methods
  def get_curselection
    tk_call(@path, 'getcurselection')
  end
  def justify(dir)
    tk_call(@path, 'justify', dir)
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
