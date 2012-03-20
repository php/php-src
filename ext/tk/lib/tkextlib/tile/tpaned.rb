#
#  tpaned widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    class TPaned < TkWindow
    end
    PanedWindow = Panedwindow = Paned = TPaned
  end
end

class Tk::Tile::TPaned < TkWindow
  include Tk::Tile::TileWidget

  if Tk::Tile::USE_TTK_NAMESPACE
    if Tk::Tile::TILE_SPEC_VERSION_ID < 8
      TkCommandNames = ['::ttk::paned'.freeze].freeze
    else
      TkCommandNames = ['::ttk::panedwindow'.freeze].freeze
    end
  else
    TkCommandNames = ['::tpaned'.freeze].freeze
  end
  WidgetClassName = 'TPaned'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.style(*args)
    [self::WidgetClassName, *(args.map!{|a| _get_eval_string(a)})].join('.')
  end

  def add(*args)
    keys = args.pop
    fail ArgumentError, "no window in arguments" unless keys

    if keys && keys.kind_of?(Hash)
      fail ArgumentError, "no window in arguments" if args == []
      opts = hash_kv(keys)
    else
      args.push(keys) if keys
      opts = []
    end

    args.each{|win|
      tk_send_without_enc('add', _epath(win), *opts)
    }
    self
  end

  def forget(pane)
    pane = _epath(pane)
    tk_send_without_enc('forget', pane)
    self
  end

  def insert(pos, win, keys)
    win = _epath(win)
    tk_send_without_enc('insert', pos, win, *hash_kv(keys))
    self
  end

  def panecget_tkstring(pane, slot)
    pane = _epath(pane)
    tk_send_without_enc('pane', pane, "-#{slot}")
  end
  alias pane_cget_tkstring panecget_tkstring

  def panecget_strict(pane, slot)
    pane = _epath(pane)
    tk_tcl2ruby(tk_send_without_enc('pane', pane, "-#{slot}"))
  end
  alias pane_cget_strict panecget_strict

  def panecget(pane, slot)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      panecget_strict(pane, slot)
    else
      begin
        panecget_strict(pane, slot)
      rescue => e
        begin
          if current_paneconfiginfo(pane).has_key?(slot.to_s)
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
  alias pane_cget panecget

  def paneconfigure(pane, key, value=nil)
    pane = _epath(pane)
    if key.kind_of? Hash
      params = []
      key.each{|k, v|
        params.push("-#{k}")
        # params.push((v.kind_of?(TkObject))? v.epath: v)
        params.push(_epath(v))
      }
      tk_send_without_enc('pane', pane, *params)
    else
      # value = value.epath if value.kind_of?(TkObject)
      value = _epath(value)
      tk_send_without_enc('pane', pane, "-#{key}", value)
    end
    self
  end
  alias pane_config paneconfigure
  alias pane_configure paneconfigure

  def paneconfiginfo(win)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      win = _epath(win)
      if key
        conf = tk_split_list(tk_send_without_enc('pane', win, "-#{key}"))
        conf[0] = conf[0][1..-1]
        if conf[0] == 'hide'
          conf[3] = bool(conf[3]) unless conf[3].empty?
          conf[4] = bool(conf[4]) unless conf[4].empty?
        end
        conf
      else
        tk_split_simplelist(tk_send_without_enc('pane',
                                                win)).collect{|conflist|
          conf = tk_split_simplelist(conflist)
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
      win = _epath(win)
      if key
        conf = tk_split_list(tk_send_without_enc('pane', win, "-#{key}"))
        key = conf.shift[1..-1]
        if key == 'hide'
          conf[2] = bool(conf[2]) unless conf[2].empty?
          conf[3] = bool(conf[3]) unless conf[3].empty?
        end
        { key => conf }
      else
        ret = {}
        tk_split_simplelist(tk_send_without_enc('pane',
                                                win)).each{|conflist|
          conf = tk_split_simplelist(conflist)
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
    tk_split_simplelist(tk_send_without_enc('panes')).map{|w|
      (obj = window(w))? obj: w
    }
  end

  def identify(x, y)
    num_or_nil(tk_send_without_enc('identify', x, y))
  end

  def sashpos(idx, newpos=None)
    num_or_str(tk_send_without_enc('sashpos', idx, newpos))
  end
end

#Tk.__set_toplevel_aliases__(:Ttk, Tk::Tile::Panedwindow,
#                            :TkPanedwindow, :TkPanedWindow)
Tk.__set_loaded_toplevel_aliases__('tkextlib/tile/tpaned.rb',
                                   :Ttk, Tk::Tile::Panedwindow,
                                   :TkPanedwindow, :TkPanedWindow)
