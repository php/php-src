#
# tk/wm.rb : methods for wm command
#
require 'tk'

module Tk
  module Wm
    #include TkComm
    extend TkCore

    TkCommandNames = ['wm'.freeze].freeze

    TOPLEVEL_METHODCALL_OPTKEYS = {}

    def Wm.aspect(win, *args)
      if args.length == 0
        list(tk_call_without_enc('wm', 'aspect', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call('wm', 'aspect', win.epath, *args)
        win
      end
    end
    def aspect(*args)
      Wm.aspect(self, *args)
    end
    alias wm_aspect aspect
    TOPLEVEL_METHODCALL_OPTKEYS['aspect'] = 'aspect'

    def Wm.attributes(win, slot=nil,value=TkComm::None)
      if slot == nil
        lst = tk_split_list(tk_call('wm', 'attributes', win.epath))
        info = {}
        while key = lst.shift
          info[key[1..-1]] = lst.shift
        end
        info
      elsif slot.kind_of? Hash
        tk_call('wm', 'attributes', win.epath, *hash_kv(slot))
        win
      elsif value == TkComm::None
        tk_call('wm', 'attributes', win.epath, "-#{slot}")
      else
        tk_call('wm', 'attributes', win.epath, "-#{slot}", value)
        win
      end
    end
    def attributes(slot=nil,value=TkComm::None)
      Wm.attributes(self, slot, value)
    end
    alias wm_attributes attributes
    TOPLEVEL_METHODCALL_OPTKEYS['attributes'] = 'attributes'

    def Wm.client(win, name=TkComm::None)
      if name == TkComm::None
        tk_call('wm', 'client', win.epath)
      else
        name = '' if name == nil
        tk_call('wm', 'client', win.epath, name)
        win
      end
    end
    def client(name=TkComm::None)
      Wm.client(self, name)
    end
    alias wm_client client
    TOPLEVEL_METHODCALL_OPTKEYS['client'] = 'client'

    def Wm.colormapwindows(win, *args)
      if args.size == 0
        list(tk_call_without_enc('wm', 'colormapwindows', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'colormapwindows', win.epath, *args)
        win
      end
    end
    def colormapwindows(*args)
      Wm.colormapwindows(self, *args)
    end
    alias wm_colormapwindows colormapwindows
    TOPLEVEL_METHODCALL_OPTKEYS['colormapwindows'] = 'colormapwindows'

    def Wm.command(win, value=nil)
      if value
        tk_call('wm', 'command', win.epath, value)
        win
      else
        #procedure(tk_call('wm', 'command', win.epath))
        tk_call('wm', 'command', win.epath)
      end
    end
    def wm_command(value=nil)
      Wm.command(self, value)
    end
    TOPLEVEL_METHODCALL_OPTKEYS['wm_command'] = 'wm_command'

    def Wm.deiconify(win, ex = true)
      if ex
        tk_call_without_enc('wm', 'deiconify', win.epath)
      else
        Wm.iconify(win)
      end
      win
    end
    def deiconify(ex = true)
      Wm.deiconify(self, ex)
    end
    alias wm_deiconify deiconify

    def Wm.focusmodel(win, mode = nil)
      if mode
        tk_call_without_enc('wm', 'focusmodel', win.epath, mode)
        win
      else
        tk_call_without_enc('wm', 'focusmodel', win.epath)
      end
    end
    def focusmodel(mode = nil)
      Wm.focusmodel(self, mode)
    end
    alias wm_focusmodel focusmodel
    TOPLEVEL_METHODCALL_OPTKEYS['focusmodel'] = 'focusmodel'

    def Wm.forget(win)
      # Tcl/Tk 8.5+
      # work with dockable frames
      tk_call_without_enc('wm', 'forget', win.epath)
      win
    end
    def wm_forget
      Wm.forget(self)
    end

    def Wm.frame(win)
      tk_call_without_enc('wm', 'frame', win.epath)
    end
    def frame
      Wm.frame(self)
    end
    alias wm_frame frame

    def Wm.geometry(win, geom=nil)
      if geom
        tk_call_without_enc('wm', 'geometry', win.epath, geom)
        win
      else
        tk_call_without_enc('wm', 'geometry', win.epath)
      end
    end
    def geometry(geom=nil)
      Wm.geometry(self, geom)
    end
    alias wm_geometry geometry
    TOPLEVEL_METHODCALL_OPTKEYS['geometry'] = 'geometry'

    def Wm.grid(win, *args)
      if args.size == 0
        list(tk_call_without_enc('wm', 'grid', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'grid', win.epath, *args)
        win
      end
    end
    def wm_grid(*args)
      Wm.grid(self, *args)
    end
    TOPLEVEL_METHODCALL_OPTKEYS['wm_grid'] = 'wm_grid'

    def Wm.group(win, leader = nil)
      if leader
        tk_call('wm', 'group', win.epath, leader)
        win
      else
        window(tk_call('wm', 'group', win.epath))
      end
    end
    def group(leader = nil)
      Wm.group(self, leader)
    end
    alias wm_group group
    TOPLEVEL_METHODCALL_OPTKEYS['group'] = 'group'

    def Wm.iconbitmap(win, bmp=nil)
      if bmp
        tk_call_without_enc('wm', 'iconbitmap', win.epath, bmp)
        win
      else
        image_obj(tk_call_without_enc('wm', 'iconbitmap', win.epath))
      end
    end
    def iconbitmap(bmp=nil)
      Wm.iconbitmap(self, bmp)
    end
    alias wm_iconbitmap iconbitmap
    TOPLEVEL_METHODCALL_OPTKEYS['iconbitmap'] = 'iconbitmap'

    def Wm.iconphoto(win, *imgs)
      if imgs.empty?
        win.instance_eval{
          @wm_iconphoto = nil unless defined? @wm_iconphoto
          return @wm_iconphoto
        }
      end

      imgs = imgs[0] if imgs.length == 1 && imgs[0].kind_of?(Array)
      tk_call_without_enc('wm', 'iconphoto', win.epath, *imgs)
      win.instance_eval{ @wm_iconphoto = imgs  }
      win
    end
    def iconphoto(*imgs)
      Wm.iconphoto(self, *imgs)
    end
    alias wm_iconphoto iconphoto
    TOPLEVEL_METHODCALL_OPTKEYS['iconphoto'] = 'iconphoto'

    def Wm.iconphoto_default(win, *imgs)
      imgs = imgs[0] if imgs.length == 1 && imgs[0].kind_of?(Array)
      tk_call_without_enc('wm', 'iconphoto', win.epath, '-default', *imgs)
      win
    end
    def iconphoto_default(*imgs)
      Wm.iconphoto_default(self, *imgs)
    end
    alias wm_iconphoto_default iconphoto_default

    def Wm.iconify(win, ex = true)
      if ex
        tk_call_without_enc('wm', 'iconify', win.epath)
      else
        Wm.deiconify(win)
      end
      win
    end
    def iconify(ex = true)
      Wm.iconify(self, ex)
    end
    alias wm_iconify iconify

    def Wm.iconmask(win, bmp=nil)
      if bmp
        tk_call_without_enc('wm', 'iconmask', win.epath, bmp)
        win
      else
        image_obj(tk_call_without_enc('wm', 'iconmask', win.epath))
      end
    end
    def iconmask(bmp=nil)
      Wm.iconmask(self, bmp)
    end
    alias wm_iconmask iconmask
    TOPLEVEL_METHODCALL_OPTKEYS['iconmask'] = 'iconmask'

    def Wm.iconname(win, name=nil)
      if name
        tk_call('wm', 'iconname', win.epath, name)
        win
      else
        tk_call('wm', 'iconname', win.epath)
      end
    end
    def iconname(name=nil)
      Wm.iconname(self, name)
    end
    alias wm_iconname iconname
    TOPLEVEL_METHODCALL_OPTKEYS['iconname'] = 'iconname'

    def Wm.iconposition(win, *args)
      if args.size == 0
        list(tk_call_without_enc('wm', 'iconposition', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'iconposition', win.epath, *args)
        win
      end
    end
    def iconposition(*args)
      Wm.iconposition(self, *args)
    end
    alias wm_iconposition iconposition
    TOPLEVEL_METHODCALL_OPTKEYS['iconposition'] = 'iconposition'

    def Wm.iconwindow(win, iconwin = nil)
      if iconwin
        tk_call_without_enc('wm', 'iconwindow', win.epath, iconwin)
        win
      else
        w = tk_call_without_enc('wm', 'iconwindow', win.epath)
        (w == '')? nil: window(w)
      end
    end
    def iconwindow(iconwin = nil)
      Wm.iconwindow(self, iconwin)
    end
    alias wm_iconwindow iconwindow
    TOPLEVEL_METHODCALL_OPTKEYS['iconwindow'] = 'iconwindow'

    def Wm.manage(win)
      # Tcl/Tk 8.5+ feature
      tk_call_without_enc('wm', 'manage', win.epath)
      win
    end
    def wm_manage
      Wm.manage(self)
    end
=begin
    def Wm.manage(win, use_id = nil)
      # Tcl/Tk 8.5+ feature
      # --------------------------------------------------------------
      # In the future release, I want to support to embed the 'win'
      # into the container which has window-id 'use-id'.
      # It may give users frexibility on controlling their GUI.
      # However, it may be difficult for current Tcl/Tk (Tcl/Tk8.5.1),
      # because it seems to require to modify Tcl/Tk's source code.
      # --------------------------------------------------------------
      if use_id
        tk_call_without_enc('wm', 'manage', win.epath, '-use', use_id)
      else
        tk_call_without_enc('wm', 'manage', win.epath)
      end
      win
    end
=end

    def Wm.maxsize(win, *args)
      if args.size == 0
        list(tk_call_without_enc('wm', 'maxsize', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'maxsize', win.epath, *args)
        win
      end
    end
    def maxsize(*args)
      Wm.maxsize(self, *args)
    end
    alias wm_maxsize maxsize
    TOPLEVEL_METHODCALL_OPTKEYS['maxsize'] = 'maxsize'

    def Wm.minsize(win, *args)
      if args.size == 0
        list(tk_call_without_enc('wm', 'minsize', win.epath))
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'minsize', win.path, *args)
        win
      end
    end
    def minsize(*args)
      Wm.minsize(self, *args)
    end
    alias wm_minsize minsize
    TOPLEVEL_METHODCALL_OPTKEYS['minsize'] = 'minsize'

    def Wm.overrideredirect(win, mode=TkComm::None)
      if mode == TkComm::None
        bool(tk_call_without_enc('wm', 'overrideredirect', win.epath))
      else
        tk_call_without_enc('wm', 'overrideredirect', win.epath, mode)
        win
      end
    end
    def overrideredirect(mode=TkComm::None)
      Wm.overrideredirect(self, mode)
    end
    alias wm_overrideredirect overrideredirect
    TOPLEVEL_METHODCALL_OPTKEYS['overrideredirect'] = 'overrideredirect'

    def Wm.positionfrom(win, who=TkComm::None)
      if who == TkComm::None
        r = tk_call_without_enc('wm', 'positionfrom', win.epath)
        (r == "")? nil: r
      else
        tk_call_without_enc('wm', 'positionfrom', win.epath, who)
        win
      end
    end
    def positionfrom(who=TkComm::None)
      Wm.positionfrom(self, who)
    end
    alias wm_positionfrom positionfrom
    TOPLEVEL_METHODCALL_OPTKEYS['positionfrom'] = 'positionfrom'

    def Wm.protocol(win, name=nil, cmd=nil, &b)
      if cmd
        tk_call_without_enc('wm', 'protocol', win.epath, name, cmd)
        win
      elsif b
        tk_call_without_enc('wm', 'protocol', win.epath, name, proc(&b))
        win
      elsif name
        result = tk_call_without_enc('wm', 'protocol', win.epath, name)
        (result == "")? nil : tk_tcl2ruby(result)
      else
        tk_split_simplelist(tk_call_without_enc('wm', 'protocol', win.epath))
      end
    end
    def protocol(name=nil, cmd=nil, &b)
      Wm.protocol(self, name, cmd, &b)
    end
    alias wm_protocol protocol

    def Wm.protocols(win, kv=nil)
      unless kv
        ret = {}
        Wm.protocol(win).each{|name|
          ret[name] = Wm.protocol(win, name)
        }
        return ret
      end

      unless kv.kind_of?(Hash)
        fail ArgumentError, 'expect a hash of protocol=>command'
      end
      kv.each{|k, v| Wm.protocol(win, k, v)}
      win
    end
    def protocols(kv=nil)
      Wm.protocols(self, kv)
    end
    alias wm_protocols protocols
    TOPLEVEL_METHODCALL_OPTKEYS['protocols'] = 'protocols'

    def Wm.resizable(win, *args)
      if args.length == 0
        list(tk_call_without_enc('wm', 'resizable', win.epath)).map!{|e| bool(e)}
      else
        args = args[0] if args.length == 1 && args[0].kind_of?(Array)
        tk_call_without_enc('wm', 'resizable', win.epath, *args)
        win
      end
    end
    def resizable(*args)
      Wm.resizable(self, *args)
    end
    alias wm_resizable resizable
    TOPLEVEL_METHODCALL_OPTKEYS['resizable'] = 'resizable'

    def Wm.sizefrom(win, who=TkComm::None)
      if who == TkComm::None
        r = tk_call_without_enc('wm', 'sizefrom', win.epath)
        (r == "")? nil: r
      else
        tk_call_without_enc('wm', 'sizefrom', win.epath, who)
        win
      end
    end
    def sizefrom(who=TkComm::None)
      Wm.sizefrom(self, who)
    end
    alias wm_sizefrom sizefrom
    TOPLEVEL_METHODCALL_OPTKEYS['sizefrom'] = 'sizefrom'

    def Wm.stackorder(win)
      list(tk_call('wm', 'stackorder', win.epath))
    end
    def stackorder
      Wm.stackorder(self)
    end
    alias wm_stackorder stackorder

    def Wm.stackorder_isabove(win, target)
      bool(tk_call('wm', 'stackorder', win.epath, 'isabove', target))
    end
    def Wm.stackorder_is_above(win, target)
      Wm.stackorder_isabove(win, target)
    end
    def stackorder_isabove(target)
      Wm.stackorder_isabove(self, target)
    end
    alias stackorder_is_above stackorder_isabove
    alias wm_stackorder_isabove stackorder_isabove
    alias wm_stackorder_is_above stackorder_isabove

    def Wm.stackorder_isbelow(win, target)
      bool(tk_call('wm', 'stackorder', win.epath, 'isbelow', target))
    end
    def Wm.stackorder_is_below(win, target)
      Wm.stackorder_isbelow(win, target)
    end
    def stackorder_isbelow(target)
      Wm.stackorder_isbelow(self, target)
    end
    alias stackorder_is_below stackorder_isbelow
    alias wm_stackorder_isbelow stackorder_isbelow
    alias wm_stackorder_is_below stackorder_isbelow

    def Wm.state(win, st=nil)
      if st
        tk_call_without_enc('wm', 'state', win.epath, st)
        win
      else
        tk_call_without_enc('wm', 'state', win.epath)
      end
    end
    def state(st=nil)
      Wm.state(self, st)
    end
    alias wm_state state
    TOPLEVEL_METHODCALL_OPTKEYS['state'] = 'state'

    def Wm.title(win, str=nil)
      if str
        tk_call('wm', 'title', win.epath, str)
        win
      else
        tk_call('wm', 'title', win.epath)
      end
    end
    def title(str=nil)
      Wm.title(self, str)
    end
    alias wm_title title
    TOPLEVEL_METHODCALL_OPTKEYS['title'] = 'title'

    def Wm.transient(win, master=nil)
      if master
        tk_call_without_enc('wm', 'transient', win.epath, master)
        win
      else
        window(tk_call_without_enc('wm', 'transient', win.epath))
      end
    end
    def transient(master=nil)
      Wm.transient(self, master)
    end
    alias wm_transient transient
    TOPLEVEL_METHODCALL_OPTKEYS['transient'] = 'transient'

    def Wm.withdraw(win, ex = true)
      if ex
        tk_call_without_enc('wm', 'withdraw', win.epath)
      else
        Wm.deiconify(win)
      end
      win
    end
    def withdraw(ex = true)
      Wm.withdraw(self, ex)
    end
    alias wm_withdraw withdraw
  end

  module Wm_for_General
    Wm.instance_methods.each{|m|
      if (m = m.to_s) =~ /^wm_(.*)$/
        eval "def #{m}(*args, &b); Tk::Wm.#{$1}(self, *args, &b); end"
      end
    }
  end
end
