#
# tk/place.rb : control place geometry manager
#
require 'tk'

module TkPlace
  include Tk
  extend Tk

  TkCommandNames = ['place'.freeze].freeze

  def configure(win, slot, value=None)
    # for >= Tk8.4a2 ?
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    if slot.kind_of? Hash
      params = []
      slot.each{|k, v|
        params.push("-#{k}")
        # params.push((v.kind_of?(TkObject))? v.epath: v)
        params.push(_epath(v))
      }
      tk_call_without_enc('place', 'configure', win, *params)
    else
      # value = value.epath if value.kind_of?(TkObject)
      value = _epath(value)
      tk_call_without_enc('place', 'configure', win, "-#{slot}", value)
    end
  end
  alias place configure

  def configinfo(win, slot = nil)
    # for >= Tk8.4a2 ?
    if TkComm::GET_CONFIGINFOwoRES_AS_ARRAY
      # win = win.epath if win.kind_of?(TkObject)
      win = _epath(win)
      if slot
        #conf = tk_split_list(tk_call_without_enc('place', 'configure',
        #                                        win, "-#{slot}") )
        conf = tk_split_simplelist(tk_call_without_enc('place', 'configure',
                                                       win, "-#{slot}") )
        conf[0] = conf[0][1..-1]
        conf[1] = tk_tcl2ruby(conf[1])
        conf[2] = tk_tcl2ruby(conf[1])
        conf[3] = tk_tcl2ruby(conf[1])
        conf[4] = tk_tcl2ruby(conf[1])
        conf
      else
        tk_split_simplelist(tk_call_without_enc('place', 'configure',
                                                win)).collect{|conflist|
          #conf = list(conflist)
          conf = simplelist(conflist).collect!{|inf| tk_tcl2ruby(inf)}
          conf[0] = conf[0][1..-1]
          conf
        }
      end
    else # ! TkComm::GET_CONFIGINFOwoRES_AS_ARRAY
      current_configinfo(win, slot)
    end
  end

  def current_configinfo(win, slot = nil)
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    if slot
      #conf = tk_split_list(tk_call_without_enc('place', 'configure',
      #                                         win, "-#{slot}") )
      conf = tk_split_simplelist(tk_call_without_enc('place', 'configure',
                                                     win, "-#{slot}") )
      # { conf[0][1..-1] => conf[1] }
      { conf[0][1..-1] => tk_tcl2ruby(conf[4]) }
    else
      ret = {}
      #tk_split_list(tk_call_without_enc('place','configure',win)).each{|conf|
      tk_split_simplelist(tk_call_without_enc('place', 'configure',
                                              win)).each{|conf_list|
        #ret[conf[0][1..-1]] = conf[1]
        conf = simplelist(conf_list)
        ret[conf[0][1..-1]] = tk_tcl2ruby(conf[4])
      }
      ret
    end
  end

  def forget(win)
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    tk_call_without_enc('place', 'forget', win)
  end

  def info(win)
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    #ilist = list(tk_call_without_enc('place', 'info', win))
    ilist = simplelist(tk_call_without_enc('place', 'info', win))
    info = {}
    while key = ilist.shift
      #info[key[1..-1]] = ilist.shift
      info[key[1..-1]] = tk_tcl2ruby(ilist.shift)
    end
    return info
  end

  def slaves(master)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    list(tk_call('place', 'slaves', master))
  end

  module_function :place, :configure, :configinfo, :current_configinfo
  module_function :forget, :info, :slaves
end
=begin
def TkPlace(win, slot, value=None)
  win = win.epath if win.kind_of?(TkObject)
  if slot.kind_of? Hash
    params = []
    slot.each{|k, v|
      params.push("-#{k}")
      params.push((v.kind_of?(TkObject))? v.epath: v)
    }
    tk_call_without_enc('place', win, *params)
  else
    value = value.epath if value.kind_of?(TkObject)
    tk_call_without_enc('place', win, "-#{slot}", value)
  end
end
=end
