#
# tk/xim.rb : control imput_method
#
require 'tk'

module TkXIM
  include Tk
  extend Tk

  TkCommandNames = ['imconfigure'.freeze].freeze

  def TkXIM.useinputmethods(value = None, win = nil)
    if value == None
      if win
        bool(tk_call_without_enc('tk', 'useinputmethods',
                                 '-displayof', win))
      else
        bool(tk_call_without_enc('tk', 'useinputmethods'))
      end
    else
      if win
        bool(tk_call_without_enc('tk', 'useinputmethods',
                                 '-displayof', win, value))
      else
        bool(tk_call_without_enc('tk', 'useinputmethods', value))
      end
    end
  end

  def TkXIM.useinputmethods_displayof(win, value = None)
    TkXIM.useinputmethods(value, win)
  end

  def TkXIM.caret(win, keys=nil)
    if keys
      tk_call_without_enc('tk', 'caret', win, *hash_kv(keys))
      self
    else
      lst = tk_split_list(tk_call_without_enc('tk', 'caret', win))
      info = {}
      while key = lst.shift
        info[key[1..-1]] = lst.shift
      end
      info
    end
  end

  def TkXIM.configure(win, slot, value=None)
    begin
      if /^8\.*/ === Tk::TK_VERSION  && JAPANIZED_TK
        if slot.kind_of? Hash
          tk_call('imconfigure', win, *hash_kv(slot))
        else
          tk_call('imconfigure', win, "-#{slot}", value)
        end
      end
    rescue
    end
  end

  def TkXIM.configinfo(win, slot=nil)
    if TkComm::GET_CONFIGINFOwoRES_AS_ARRAY
      begin
        if /^8\.*/ === Tk::TK_VERSION  && JAPANIZED_TK
          if slot
            conf = tk_split_list(tk_call('imconfigure', win, "-#{slot}"))
            conf[0] = conf[0][1..-1]
            conf
          else
            tk_split_list(tk_call('imconfigure', win)).collect{|conf|
              conf[0] = conf[0][1..-1]
              conf
            }
          end
        else
          []
        end
      rescue
        []
      end
    else # ! TkComm::GET_CONFIGINFOwoRES_AS_ARRAY
      TkXIM.current_configinfo(win, slot)
    end
  end

  def TkXIM.current_configinfo(win, slot=nil)
    begin
      if /^8\.*/ === Tk::TK_VERSION  && JAPANIZED_TK
        if slot
          conf = tk_split_list(tk_call('imconfigure', win, "-#{slot}"))
          { conf[0][1..-1] => conf[1] }
        else
          ret = {}
          tk_split_list(tk_call('imconfigure', win)).each{|conf|
            ret[conf[0][1..-1]] = conf[1]
          }
          ret
        end
      else
        {}
      end
    rescue
      {}
    end
  end

  def useinputmethods(value=None)
    TkXIM.useinputmethods(value, self)
  end

  def caret(keys=nil)
    TkXIM.caret(self, keys=nil)
  end

  def imconfigure(slot, value=None)
    TkXIM.configure(self, slot, value)
  end

  def imconfiginfo(slot=nil)
    TkXIM.configinfo(self, slot)
  end
end
