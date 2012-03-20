#
#  ::vu::dial widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# create module/class
module Tk
  module Vu
    class Dial < TkWindow
    end
  end
end

# call setup script  --  <libdir>/tkextlib/vu.rb
require 'tkextlib/vu.rb'

# define module/class
class Tk::Vu::Dial < TkWindow
  TkCommandNames = ['::vu::dial'.freeze].freeze
  WidgetClassName = 'Dial'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ###############################

  def __methodcall_optkeys  # { key=>method, ... }
    {'coords'=>'coords'}
  end
  private :__methodcall_optkeys

  ###############################

  def coords(val = nil)
    if val
      tk_send_without_enc('coords', val)
      self
    else
      tk_split_list(tk_send_without_enc('coords'))
    end
  end

  def constrain(val = None)
    num_or_str(tk_call(@path, 'constrain', val))
  end

  def get(*args)
    num_or_str(tk_call(@path, 'get', *args))
  end

  def identify(x, y)
    tk_call(@path, 'identify', x, y)
  end

  def get_label(val=nil)
    if val
      tk_call(@path, 'label', val)
    else
      ret = []
      lst = simplelist(tk_call(@path, 'label'))
      while lst.size > 0
        ret << ([num_or_str(lst.shift)] << lst.shift)
      end
    end
  end

  def set_label(val, str, *args)
    tk_call(@path, 'label', val, str, *args)
    self
  end

  def set_label_constrain(val, str, *args)
    tk_call(@path, 'label', '-constrain', val, str, *args)
    self
  end

  def get_tag(val=nil)
    if val
      tk_call(@path, 'tag', val)
    else
      ret = []
      lst = simplelist(tk_call(@path, 'tag'))
      while lst.size > 0
        ret << ([num_or_str(lst.shift)] << lst.shift)
      end
    end
  end

  def set_tag(val, str, *args)
    tk_call(@path, 'tag', val, str, *args)
    self
  end

  def set_tag_constrain(val, str, *args)
    tk_call(@path, 'tag', '-constrain', val, str, *args)
    self
  end

  def set(val = None)
    tk_call_without_enc(@path, 'set', val)
    self
  end
end
