#
#  tkextlib/iwidgets/promptdialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Promptdialog < Tk::Iwidgets::Dialog
    end
  end
end

class Tk::Iwidgets::Promptdialog
  TkCommandNames = ['::iwidgets::promptdialog'.freeze].freeze
  WidgetClassName = 'Promptdialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  # index method is not available, because it shows index of the entry field
  def default(name)
    tk_call(@path, 'default', tagid(name))
    self
  end

  def hide(name)
    tk_call(@path, 'hide', tagid(name))
    self
  end

  def invoke(name=nil)
    if name
      tk_call(@path, 'invoke', tagid(name))
    else
      tk_call(@path, 'invoke')
    end
    self
  end

  def show(name)
    tk_call(@path, 'show', tagid(name))
    self
  end


  # based on Tk::Iwidgets::Entryfield
  def clear
    tk_call(@path, 'clear')
    self
  end

  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end

  def value
    _fromUTF8(tk_send_without_enc('get'))
  end
  def value= (val)
    tk_send_without_enc('delete', 0, 'end')
    tk_send_without_enc('insert', 0, _get_eval_enc_str(val))
    val
  end
  alias get value
  alias set value=

  def cursor=(index)
    tk_send_without_enc('icursor', index)
    #self
    index
  end
  alias icursor cursor=

  def index(idx)
    number(tk_send_without_enc('index', idx))
  end

  def insert(pos,text)
    tk_send_without_enc('insert', pos, _get_eval_enc_str(text))
    self
  end

  def mark(pos)
    tk_send_without_enc('scan', 'mark', pos)
    self
  end
  def dragto(pos)
    tk_send_without_enc('scan', 'dragto', pos)
    self
  end
  def selection_adjust(index)
    tk_send_without_enc('selection', 'adjust', index)
    self
  end
  def selection_clear
    tk_send_without_enc('selection', 'clear')
    self
  end
  def selection_from(index)
    tk_send_without_enc('selection', 'from', index)
    self
  end
  def selection_present()
    bool(tk_send_without_enc('selection', 'present'))
  end
  def selection_range(s, e)
    tk_send_without_enc('selection', 'range', s, e)
    self
  end
  def selection_to(index)
    tk_send_without_enc('selection', 'to', index)
    self
  end

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
end
