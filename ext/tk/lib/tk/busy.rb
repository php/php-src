#
# tk/busy.rb: support 'tk busy' command (Tcl/Tk8.6 or later)
#
require 'tk'

module Tk::Busy
  include TkCore
  extend TkCore
  extend TkItemConfigMethod
end

class << Tk::Busy
  def __item_cget_cmd(win)
    # maybe need to override
    ['tk', 'busy', 'cget', win.path]
  end
  private :__item_cget_cmd

  def __item_config_cmd(win)
    # maybe need to override
    ['tk', 'busy', 'configure', win.path]
  end
  private :__item_config_cmd

  def __item_confinfo_cmd(win)
    # maybe need to override
    __item_config_cmd(win)
  end
  private :__item_confinfo_cmd

  alias cget_tkstring itemcget_tkstring
  alias cget itemcget
  alias cget_strict itemcget_strict
  alias configure itemconfigure
  alias configinfo itemconfiginfo
  alias current_configinfo current_itemconfiginfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

  def method_missing(id, *args)
    name = id.id2name
    case args.length
    when 1
      if name[-1] == ?=
        configure name[0..-2], args[0]
        args[0]
      else
        configure name, args[0]
        self
      end
    when 0
      begin
        cget(name)
      rescue
        super(id, *args)
      end
    else
      super(id, *args)
    end
  end

  def hold(win, keys={})
    tk_call_without_enc('tk', 'busy', 'hold', win, *hash_kv(keys))
    win
  end

  def forget(*wins)
    tk_call_without_enc('tk', 'busy', 'forget', *wins)
    self
  end

  def current(pat=None)
    list(tk_call('tk', 'busy', 'current', pat))
  end

  def status(win)
    bool(tk_call_without_enc('tk', 'busy', 'status', win))
  end
end

module Tk::Busy
  def busy_configinfo(option=nil)
    Tk::Busy.configinfo(self, option)
  end

  def busy_current_configinfo(option=nil)
    Tk::Busy.current_configinfo(self, option)
  end

  def busy_configure(option, value=None)
    Tk::Busy.configure(self, option, value)
    self
  end

  def busy_cget(option)
    Tk::Busy.configure(self, option)
  end

  def busy(keys={})
    Tk::Busy.hold(self, keys)
    self
  end
  alias busy_hold busy

  def busy_forget
    Tk::Busy.forget(self)
    self
  end

  def busy_current?
    ! Tk::Busy.current(self.path).empty?
  end

  def busy_status
    Tk::Busy.status(self)
  end
end
