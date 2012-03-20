#
#  tkextlib/blt/htext.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/itemconfig.rb'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Htext<TkWindow
    Htext_Var = TkVarAccess.new_hash('htext')
    Htext_Widget = TkVarAccess.new('htext(widget)', :window)
    Htext_File = TkVarAccess.new('htext(file)')
    Htext_Line = TkVarAccess.new('htext(line)')

    include TkItemConfigMethod
    include Scrollable

    TkCommandNames = ['::blt::htext'.freeze].freeze
    WidgetClassName = 'Htext'.freeze
    WidgetClassNames[WidgetClassName] ||= self

    alias window_cget_tkstring itemcget_tkstring
    alias window_cget itemcget
    alias window_cget_strict itemcget_strict
    alias window_configure itemconfigure
    alias window_configuinfo itemconfiginfo
    alias current_window_configuinfo current_itemconfiginfo

    def __strval_optkeys
      super() << 'filename'
    end
  private :__strval_optkeys

    def append(win, keys={})
      tk_send('append', _epath(win), keys)
      self
    end

    def goto_line(idx)
      tk_send_without_enc('gotoline', idx)
      self
    end
    def current_line
      number(tk_send_without_enc('gotoline'))
    end

    def index(str)
      number(tk_send('index', str))
    end

    def line_pos(str)
      tk_send('linepos', str)
    end

    def range(from=None, to=None)
      tk_send_without_enc('range', from, to)
    end

    def scan_mark(pos)
      tk_send_without_enc('scan', 'mark', pos)
      self
    end

    def scan_dragto(pos)
      tk_send_without_enc('scan', 'dragto', pos)
      self
    end

    def search(pat, from=None, to=None)
      num = number(tk_send('search', pat, from, to))
      (num < 0)? nil: num
    end

    def selection_adjust(index)
      tk_send_without_enc('selection', 'adjust', index)
      self
    end
    def selection_clear()
      tk_send_without_enc('selection', 'clear')
      self
    end
    def selection_from(index)
      tk_send_without_enc('selection', 'from', index)
      self
    end
    def selection_line(index)
      tk_send_without_enc('selection', 'line', index)
      self
    end
    def selection_present()
      bool(tk_send_without_enc('selection', 'present'))
    end
    def selection_range(first, last)
      tk_send_without_enc('selection', 'range', first, last)
      self
    end
    def selection_to(index)
      tk_send_without_enc('selection', 'to', index)
      self
    end
    def selection_word(index)
      tk_send_without_enc('selection', 'word', index)
      self
    end

    def windows(pat=None)
      list(tk_send('windows', pat))
    end
  end
end
