#
# tk/clipboard.rb : methods to treat clipboard
#
require 'tk'

module TkClipboard
  include Tk
  extend Tk

  TkCommandNames = ['clipboard'.freeze].freeze

  def self.clear(win=nil)
    if win
      tk_call_without_enc('clipboard', 'clear', '-displayof', win)
    else
      tk_call_without_enc('clipboard', 'clear')
    end
  end
  def self.clear_on_display(win)
    tk_call_without_enc('clipboard', 'clear', '-displayof', win)
  end

  def self.get(type=nil)
    if type
      tk_call_without_enc('clipboard', 'get', '-type', type)
    else
      tk_call_without_enc('clipboard', 'get')
    end
  end
  def self.get_on_display(win, type=nil)
    if type
      tk_call_without_enc('clipboard', 'get', '-displayof', win, '-type', type)
    else
      tk_call_without_enc('clipboard', 'get', '-displayof', win)
    end
  end

  def self.set(data, keys=nil)
    clear
    append(data, keys)
  end
  def self.set_on_display(win, data, keys=nil)
    clear(win)
    append_on_display(win, data, keys)
  end

  def self.append(data, keys=nil)
    args = ['clipboard', 'append']
    args.concat(hash_kv(keys))
    args.concat(['--', data])
    tk_call(*args)
  end
  def self.append_on_display(win, data, keys=nil)
    args = ['clipboard', 'append', '-displayof', win]
    args.concat(hash_kv(keys))
    args.concat(['--', data])
    tk_call(*args)
  end

  def clear
    TkClipboard.clear_on_display(self)
    self
  end
  def get(type=nil)
    TkClipboard.get_on_display(self, type)
  end
  def set(data, keys=nil)
    TkClipboard.set_on_display(self, data, keys)
    self
  end
  def append(data, keys=nil)
    TkClipboard.append_on_display(self, data, keys)
    self
  end
end
