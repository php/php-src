#
#  tkextlib/iwidgets/selectiondialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Selectiondialog < Tk::Iwidgets::Dialog
    end
  end
end

class Tk::Iwidgets::Selectiondialog
  TkCommandNames = ['::iwidgets::selectiondialog'.freeze].freeze
  WidgetClassName = 'Selectiondialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def clear_items
    tk_call(@path, 'clear', 'items')
    self
  end

  def clear_selection
    tk_call(@path, 'clear', 'selection')
    self
  end

  def get
    tk_call(@path, 'get')
  end

  def insert_items(idx, *args)
    tk_call(@path, 'insert', 'items', idx, *args)
  end

  def insert_selection(pos, text)
    tk_call(@path, 'insert', 'selection', pos, text)
  end

  def select_item
    tk_call(@path, 'selectitem')
    self
  end

  # based on Tk::Listbox ( and TkTextWin )
  def curselection
    list(tk_send_without_enc('curselection'))
  end
  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end
  def index(idx)
    tk_send_without_enc('index', idx).to_i
  end
  def nearest(y)
    tk_send_without_enc('nearest', y).to_i
  end
  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send_without_enc('scan', 'dragto', x, y)
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
  def size
    tk_send_without_enc('size').to_i
  end
end
