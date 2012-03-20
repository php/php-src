#
# tk/panedwindow.rb : treat panedwindow
#
require 'tk'

class Tk::PanedWindow<TkWindow
  TkCommandNames = ['panedwindow'.freeze].freeze
  WidgetClassName = 'Panedwindow'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('panedwindow', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('panedwindow', @path)
  #  end
  #end
  #private :create_self

  def add(*args)
    keys = args.pop
    fail ArgumentError, "no window in arguments" unless keys
    if keys && keys.kind_of?(Hash)
      fail ArgumentError, "no window in arguments" if args == []
      # args = args.collect{|w| (w.kind_of?(TkObject))? w.epath: w }
      args = args.collect{|w| _epath(w) }
      #args.push(hash_kv(keys))
      args.concat(hash_kv(keys))
    else
      args.push(keys) if keys
      # args = args.collect{|w| (w.kind_of?(TkObject))? w.epath: w }
      args = args.collect{|w| _epath(w) }
    end
    tk_send_without_enc('add', *args)
    self
  end

  def forget(win, *wins)
    wins.unshift(win)
    # tk_send_without_enc('forget', *((w.kind_of?(TkObject))? w.epath: w))
    tk_send_without_enc('forget', *(wins.collect{|w| _epath(w)}))
    self
  end
  alias del forget
  alias delete forget
  alias remove forget

  def identify(x, y)
    list(tk_send_without_enc('identify', x, y))
  end

  def proxy_coord
    list(tk_send_without_enc('proxy', 'coord'))
  end
  def proxy_forget
    tk_send_without_enc('proxy', 'forget')
    self
  end
  def proxy_place(x, y)
    tk_send_without_enc('proxy', 'place', x, y)
    self
  end

  def sash_coord(index)
    list(tk_send('sash', 'coord', index))
  end
  def sash_dragto(index, x, y)
    tk_send('sash', 'dragto', index, x, y)
    self
  end
  def sash_mark(index, x, y)
    tk_send('sash', 'mark', index, x, y)
    self
  end
  def sash_place(index, x, y)
    tk_send('sash', 'place', index, x, y)
    self
  end

  def panecget_strict(win, key)
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    tk_tcl2ruby(tk_send_without_enc('panecget', win, "-#{key}"))
  end
  def panecget(win, key)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      panecget_strict(win, key)
    else
      begin
        panecget_strict(win, key)
      rescue => e
        begin
          if current_paneconfiginfo(win).has_key?(option.to_s)
            # not tag error & option is known -> error on known option
            fail e
          else
            # not tag error & option is unknown
            nil
          end
        rescue
          fail e  # tag error
        end
      end
    end
  end

  def paneconfigure(win, key, value=nil)
    # win = win.epath if win.kind_of?(TkObject)
    win = _epath(win)
    if key.kind_of? Hash
      params = []
      key.each{|k, v|
        params.push("-#{k}")
        # params.push((v.kind_of?(TkObject))? v.epath: v)
        params.push(_epath(v))
      }
      tk_send_without_enc('paneconfigure', win, *params)
    else
      # value = value.epath if value.kind_of?(TkObject)
      value = _epath(value)
      tk_send_without_enc('paneconfigure', win, "-#{key}", value)
    end
    self
  end
  alias pane_config paneconfigure

  def paneconfiginfo(win, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      # win = win.epath if win.kind_of?(TkObject)
      win = _epath(win)
      if key
        #conf = tk_split_list(tk_send_without_enc('paneconfigure',
        #                                         win, "-#{key}"))
        conf = tk_split_list(tk_send_without_enc('paneconfigure',
                                                 win, "-#{key}"),
                             false, true)
        conf[0] = conf[0][1..-1]
        if conf[0] == 'hide'
          conf[3] = bool(conf[3]) unless conf[3].empty?
          conf[4] = bool(conf[4]) unless conf[4].empty?
        end
        conf
      else
        #tk_split_simplelist(tk_send_without_enc('paneconfigure',
        #                                        win)).collect{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('paneconfigure', win),
                            false, false).collect{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          conf[0] = conf[0][1..-1]
          if conf[3]
            if conf[0] == 'hide'
              conf[3] = bool(conf[3]) unless conf[3].empty?
            elsif conf[3].index('{')
              conf[3] = tk_split_list(conf[3])
            else
              conf[3] = tk_tcl2ruby(conf[3])
            end
          end
          if conf[4]
            if conf[0] == 'hide'
              conf[4] = bool(conf[4]) unless conf[4].empty?
            elsif conf[4].index('{')
              conf[4] = tk_split_list(conf[4])
            else
              conf[4] = tk_tcl2ruby(conf[4])
            end
          end
          conf[1] = conf[1][1..-1] if conf.size == 2 # alias info
          conf
        }
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      # win = win.epath if win.kind_of?(TkObject)
      win = _epath(win)
      if key
        #conf = tk_split_list(tk_send_without_enc('paneconfigure',
        #                                         win, "-#{key}"))
        conf = tk_split_list(tk_send_without_enc('paneconfigure',
                                                 win, "-#{key}"),
                             false, true)
        key = conf.shift[1..-1]
        if key == 'hide'
          conf[2] = bool(conf[2]) unless conf[2].empty?
          conf[3] = bool(conf[3]) unless conf[3].empty?
        end
        { key => conf }
      else
        ret = {}
        #tk_split_simplelist(tk_send_without_enc('paneconfigure',
        #                                        win)).each{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('paneconfigure', win),
                            false, false).each{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          key = conf.shift[1..-1]
          if key
            if key == 'hide'
              conf[2] = bool(conf[2]) unless conf[2].empty?
            elsif conf[2].index('{')
              conf[2] = tk_split_list(conf[2])
            else
              conf[2] = tk_tcl2ruby(conf[2])
            end
          end
          if conf[3]
            if key == 'hide'
              conf[3] = bool(conf[3]) unless conf[3].empty?
            elsif conf[3].index('{')
              conf[3] = tk_split_list(conf[3])
            else
              conf[3] = tk_tcl2ruby(conf[3])
            end
          end
          if conf.size == 1
            ret[key] = conf[0][1..-1]  # alias info
          else
            ret[key] = conf
          end
        }
        ret
      end
    end
  end
  alias pane_configinfo paneconfiginfo

  def current_paneconfiginfo(win, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        conf = paneconfiginfo(win, key)
        {conf[0] => conf[4]}
      else
        ret = {}
        paneconfiginfo(win).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      paneconfiginfo(win, key).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end

  alias current_pane_configinfo current_paneconfiginfo

  def panes
    list(tk_send_without_enc('panes'))
  end
end

Tk::Panedwindow = Tk::PanedWindow
#TkPanedWindow = Tk::PanedWindow unless Object.const_defined? :TkPanedWindow
#TkPanedwindow = Tk::Panedwindow unless Object.const_defined? :TkPanedwindow
#Tk.__set_toplevel_aliases__(:Tk, Tk::PanedWindow,
#                            :TkPanedWindow, :TkPanedwindow)
Tk.__set_loaded_toplevel_aliases__('tk/panedwindow.rb', :Tk, Tk::PanedWindow,
                                   :TkPanedWindow, :TkPanedwindow)
