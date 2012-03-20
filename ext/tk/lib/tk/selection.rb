#
# tk/selection.rb : control selection
#
require 'tk'

module TkSelection
  include Tk
  extend Tk

  TkCommandNames = ['selection'.freeze].freeze

  def self.clear(sel=nil)
    if sel
      tk_call_without_enc('selection', 'clear', '-selection', sel)
    else
      tk_call_without_enc('selection', 'clear')
    end
  end
  def self.clear_on_display(win, sel=nil)
    if sel
      tk_call_without_enc('selection', 'clear',
                          '-displayof', win, '-selection', sel)
    else
      tk_call_without_enc('selection', 'clear', '-displayof', win)
    end
  end
  def clear(sel=nil)
    TkSelection.clear_on_display(self, sel)
    self
  end

  def self.get(keys=nil)
    #tk_call('selection', 'get', *hash_kv(keys))
    _fromUTF8(tk_call_without_enc('selection', 'get', *hash_kv(keys)))
  end
  def self.get_on_display(win, keys=nil)
    #tk_call('selection', 'get', '-displayof', win, *hash_kv(keys))
    _fromUTF8(tk_call_without_enc('selection', 'get', '-displayof',
                                  win, *hash_kv(keys)))
  end
  def get(keys=nil)
    TkSelection.get_on_display(self, sel)
  end

  def self.handle(win, func=Proc.new, keys=nil, &b)
    if func.kind_of?(Hash) && keys == nil
      keys = func
      func = Proc.new(&b)
    end
    args = ['selection', 'handle']
    args.concat(hash_kv(keys))
    args.concat([win, func])
    tk_call_without_enc(*args)
  end
  def handle(func=Proc.new, keys=nil, &b)
    TkSelection.handle(self, func, keys, &b)
  end

  def self.get_owner(sel=nil)
    if sel
      window(tk_call_without_enc('selection', 'own', '-selection', sel))
    else
      window(tk_call_without_enc('selection', 'own'))
    end
  end
  def self.get_owner_on_display(win, sel=nil)
    if sel
      window(tk_call_without_enc('selection', 'own',
                                 '-displayof', win, '-selection', sel))
    else
      window(tk_call_without_enc('selection', 'own', '-displayof', win))
    end
  end
  def get_owner(sel=nil)
    TkSelection.get_owner_on_display(self, sel)
    self
  end

  def self.set_owner(win, keys=nil)
    tk_call_without_enc('selection', 'own', *(hash_kv(keys) << win))
  end
  def set_owner(keys=nil)
    TkSelection.set_owner(self, keys)
    self
  end
end
