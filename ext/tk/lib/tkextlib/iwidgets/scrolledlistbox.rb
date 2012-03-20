#
#  tkextlib/iwidgets/scrolledlistbox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/listbox'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scrolledlistbox < Tk::Iwidgets::Scrolledwidget
    end
  end
end

class Tk::Iwidgets::Scrolledlistbox
  TkCommandNames = ['::iwidgets::scrolledlistbox'.freeze].freeze
  WidgetClassName = 'Scrolledlistbox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'textbackground'
  end
  private :__strval_optkeys

  def __tkvariable_optkeys
    super() << 'listvariable'
  end
  private :__tkvariable_optkeys

  def __font_optkeys
    super() << 'textfont'
  end
  private :__font_optkeys

  ################################

  def initialize(*args)
    super(*args)
    @listbox = component_widget('listbox')
  end

  def method_missing(id, *args)
    if @listbox.respond_to?(id)
      @listbox.__send__(id, *args)
    else
      super(id, *args)
    end
  end

  ################################

  def clear
    tk_call(@path, 'clear')
    self
  end

  def get_curselection
    tk_call(@path, 'getcurselection')
  end

  def justify(dir)
    tk_call(@path, 'justify', dir)
    self
  end

  def selected_item_count
    number(tk_call(@path, 'selecteditemcount'))
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

  #####################################

  def bbox(index)
    list(tk_send_without_enc('bbox', index))
  end
  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end
  def get(*index)
    _fromUTF8(tk_send_without_enc('get', *index))
  end
  def insert(index, *args)
    tk_send('insert', index, *args)
    self
  end
  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send_without_enc('scan', 'dragto', x, y)
    self
  end
  def see(index)
    tk_send_without_enc('see', index)
    self
  end

  #####################################

  include TkListItemConfig

  def tagid(tag)
    if tag.kind_of?(Tk::Itk::Component)
      tag.name
    else
      super(tag)
    end
  end
  private :tagid

  #####################################

  def activate(y)
    tk_send_without_enc('activate', y)
    self
  end
  def curselection
    list(tk_send_without_enc('curselection'))
  end
  def get(first, last=nil)
    if last
      # tk_split_simplelist(_fromUTF8(tk_send_without_enc('get', first, last)))
      tk_split_simplelist(tk_send_without_enc('get', first, last),
                          false, true)
    else
      _fromUTF8(tk_send_without_enc('get', first))
    end
  end
  def nearest(y)
    tk_send_without_enc('nearest', y).to_i
  end
  def size
    tk_send_without_enc('size').to_i
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

  def index(idx)
    tk_send_without_enc('index', idx).to_i
  end

  #####################################

  def xview(*index)
    if index.size == 0
      list(tk_send_without_enc('xview'))
    else
      tk_send_without_enc('xview', *index)
      self
    end
  end
  def xview_moveto(*index)
    xview('moveto', *index)
  end
  def xview_scroll(*index)
    xview('scroll', *index)
  end

  def yview(*index)
    if index.size == 0
      list(tk_send_without_enc('yview'))
    else
      tk_send_without_enc('yview', *index)
      self
    end
  end
  def yview_moveto(*index)
    yview('moveto', *index)
  end
  def yview_scroll(*index)
    yview('scroll', *index)
  end
end
