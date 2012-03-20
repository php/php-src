#
# tk/grid.rb : control grid geometry manager
#
require 'tk'

module TkGrid
  include Tk
  extend Tk

  TkCommandNames = ['grid'.freeze].freeze

  def anchor(master, anchor=None)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    tk_call_without_enc('grid', 'anchor', master, anchor)
  end

  def bbox(master, *args)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    args.unshift(master)
    list(tk_call_without_enc('grid', 'bbox', *args))
  end

=begin
  def configure(win, *args)
    if args[-1].kind_of?(Hash)
      opts = args.pop
    else
      opts = {}
    end
    params = []
    params.push(_epath(win))
    args.each{|win|
      case win
      when '-', 'x', '^'  # RELATIVE PLACEMENT
        params.push(win)
      else
        params.push(_epath(win))
      end
    }
    opts.each{|k, v|
      params.push("-#{k}")
      params.push((v.kind_of?(TkObject))? v.epath: v)
    }
    if Tk::TCL_MAJOR_VERSION < 8 ||
        (Tk::TCL_MAJOR_VERSION == 8 && Tk::TCL_MINOR_VERSION <= 3)
      if params[0] == '-' || params[0] == 'x' || params[0] == '^'
        tk_call_without_enc('grid', *params)
      else
        tk_call_without_enc('grid', 'configure', *params)
      end
    else
      tk_call_without_enc('grid', 'configure', *params)
    end
  end
=end
  def configure(*args)
    if args[-1].kind_of?(Hash)
      opts = args.pop
    else
      opts = {}
    end
    fail ArgumentError, 'no widget is given' if args.empty?
    params = []
    args.flatten(1).each{|win|
      case win
      when '-', ?-              # RELATIVE PLACEMENT (increase columnspan)
        params.push('-')
      when /^-+$/             # RELATIVE PLACEMENT (increase columnspan)
        params.concat(win.to_s.split(//))
      when '^', ?^              # RELATIVE PLACEMENT (increase rowspan)
        params.push('^')
      when /^\^+$/             # RELATIVE PLACEMENT (increase rowspan)
        params.concat(win.to_s.split(//))
      when 'x', :x, ?x, nil, '' # RELATIVE PLACEMENT (empty column)
        params.push('x')
      when /^x+$/             # RELATIVE PLACEMENT (empty column)
        params.concat(win.to_s.split(//))
      else
        params.push(_epath(win))
      end
    }
    opts.each{|k, v|
      params.push("-#{k}")
      params.push(_epath(v))  # have to use 'epath' (hash_kv() is unavailable)
    }
    if Tk::TCL_MAJOR_VERSION < 8 ||
        (Tk::TCL_MAJOR_VERSION == 8 && Tk::TCL_MINOR_VERSION <= 3)
      if params[0] == '-' || params[0] == 'x' || params[0] == '^'
        tk_call_without_enc('grid', *params)
      else
        tk_call_without_enc('grid', 'configure', *params)
      end
    else
      tk_call_without_enc('grid', 'configure', *params)
    end
  end
  alias grid configure

  def columnconfigure(master, index, args)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    tk_call_without_enc("grid", 'columnconfigure',
                        master, index, *hash_kv(args))
  end

  def rowconfigure(master, index, args)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    tk_call_without_enc("grid", 'rowconfigure', master, index, *hash_kv(args))
  end

  def columnconfiginfo(master, index, slot=nil)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    if slot
      case slot
      when 'uniform', :uniform
        tk_call_without_enc('grid', 'columnconfigure',
                            master, index, "-#{slot}")
      else
        num_or_str(tk_call_without_enc('grid', 'columnconfigure',
                                       master, index, "-#{slot}"))
      end
    else
      #ilist = list(tk_call_without_enc('grid','columnconfigure',master,index))
      ilist = simplelist(tk_call_without_enc('grid', 'columnconfigure',
                                             master, index))
      info = {}
      while key = ilist.shift
        case key
        when 'uniform'
          info[key[1..-1]] = ilist.shift
        else
          info[key[1..-1]] = tk_tcl2ruby(ilist.shift)
        end
      end
      info
    end
  end

  def rowconfiginfo(master, index, slot=nil)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    if slot
      case slot
      when 'uniform', :uniform
        tk_call_without_enc('grid', 'rowconfigure',
                            master, index, "-#{slot}")
      else
        num_or_str(tk_call_without_enc('grid', 'rowconfigure',
                                       master, index, "-#{slot}"))
      end
    else
      #ilist = list(tk_call_without_enc('grid', 'rowconfigure', master, index))
      ilist = simplelist(tk_call_without_enc('grid', 'rowconfigure',
                                             master, index))
      info = {}
      while key = ilist.shift
        case key
        when 'uniform'
          info[key[1..-1]] = ilist.shift
        else
          info[key[1..-1]] = tk_tcl2ruby(ilist.shift)
        end
      end
      info
    end
  end

  def column(master, index, keys=nil)
    if keys.kind_of?(Hash)
      columnconfigure(master, index, keys)
    else
      columnconfiginfo(master, index, keys)
    end
  end

  def row(master, index, keys=nil)
    if keys.kind_of?(Hash)
      rowconfigure(master, index, keys)
    else
      rowconfiginfo(master, index, keys)
    end
  end

  def add(widget, *args)
    configure(widget, *args)
  end

  def forget(*args)
    return '' if args.size == 0
    wins = args.collect{|win|
      # (win.kind_of?(TkObject))? win.epath: win
      _epath(win)
    }
    tk_call_without_enc('grid', 'forget', *wins)
  end

  def info(slave)
    # slave = slave.epath if slave.kind_of?(TkObject)
    slave = _epath(slave)
    #ilist = list(tk_call_without_enc('grid', 'info', slave))
    ilist = simplelist(tk_call_without_enc('grid', 'info', slave))
    info = {}
    while key = ilist.shift
      #info[key[1..-1]] = ilist.shift
      info[key[1..-1]] = tk_tcl2ruby(ilist.shift)
    end
    return info
  end

  def location(master, x, y)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    list(tk_call_without_enc('grid', 'location', master, x, y))
  end

  def propagate(master, mode=None)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    if mode == None
      bool(tk_call_without_enc('grid', 'propagate', master))
    else
      tk_call_without_enc('grid', 'propagate', master, mode)
    end
  end

  def remove(*args)
    return '' if args.size == 0
    wins = args.collect{|win|
      # (win.kind_of?(TkObject))? win.epath: win
      _epath(win)
    }
    tk_call_without_enc('grid', 'remove', *wins)
  end

  def size(master)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    list(tk_call_without_enc('grid', 'size', master))
  end

  def slaves(master, keys=nil)
    # master = master.epath if master.kind_of?(TkObject)
    master = _epath(master)
    list(tk_call_without_enc('grid', 'slaves', master, *hash_kv(args)))
  end

  module_function :anchor, :bbox, :add, :forget, :propagate, :info
  module_function :remove, :size, :slaves, :location
  module_function :grid, :configure, :columnconfigure, :rowconfigure
  module_function :column, :row, :columnconfiginfo, :rowconfiginfo
end
=begin
def TkGrid(win, *args)
  if args[-1].kind_of?(Hash)
    opts = args.pop
  else
    opts = {}
  end
  params = []
  params.push((win.kind_of?(TkObject))? win.epath: win)
  args.each{|win|
    case win
    when '-', 'x', '^'  # RELATIVE PLACEMENT
      params.push(win)
    else
      params.push((win.kind_of?(TkObject))? win.epath: win)
    end
  }
  opts.each{|k, v|
    params.push("-#{k}")
    params.push((v.kind_of?(TkObject))? v.epath: v)
  }
  tk_call_without_enc("grid", *params)
end
=end
