#
# tk/txtwin_abst.rb : TkTextWin abstruct class
#
require 'tk'

class TkTextWin<TkWindow
  TkCommandNames = [].freeze
  #def create_self
  #  fail RuntimeError, "TkTextWin is an abstract class"
  #end
  #private :create_self

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
end
