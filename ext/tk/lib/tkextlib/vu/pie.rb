#
#  ::vu::pie widget
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

# create module/class
module Tk
  module Vu
    module PieSliceConfigMethod
    end
    class Pie < TkWindow
    end
    class PieSlice < TkObject
    end
    class NamedPieSlice < PieSlice
    end
  end
end

# call setup script  --  <libdir>/tkextlib/vu.rb
require 'tkextlib/vu.rb'

module Tk::Vu::PieSliceConfigMethod
  include TkItemConfigMethod

  def __item_pathname(tagOrId)
    if tagOrId.kind_of?(Tk::Vu::PieSlice)
      self.path + ';' + tagOrId.id.to_s
    else
      self.path + ';' + tagOrId.to_s
    end
  end
  private :__item_pathname
end

class Tk::Vu::Pie < TkWindow
  TkCommandNames = ['::vu::pie'.freeze].freeze
  WidgetClassName = 'Pie'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __destroy_hook__
    Tk::Vu::PieSlice::SliceID_TBL.delete(@path)
  end

  ###############################

  include Tk::Vu::PieSliceConfigMethod

  def tagid(tag)
    if tag.kind_of?(Tk::Vu::PieSlice)
      tag.id
    else
      # tag
      _get_eval_string(tag)
    end
  end

  ###############################

  def delete(*glob_pats)
    tk_call(@path, 'delete', *glob_pats)
    self
  end

  def explode(slice, *args)
    tk_call(@path, 'explode', slice, *args)
    self
  end

  def explode_value(slice)
    num_or_str(tk_call(@path, 'explode', slice))
  end

  def lower(slice, below=None)
    tk_call(@path, 'lower', slice, below)
    self
  end

  def names(*glob_pats)
    simplelist(tk_call(@path, 'names', *glob_pats))
  end
  alias slices names

  def order(*args)
    tk_call(@path, 'order', *args)
    self
  end

  def raise(slice, above=None)
    tk_call(@path, 'raise', slice, above)
    self
  end

  def swap(slice1, slice2)
    tk_call(@path, 'swap', slice1, slice2)
    self
  end

  def set(slice, *args)
    num_or_str(tk_call(@path, 'set', slice, *args))
  end
  alias set_value  set
  alias set_values set
  alias create     set

  def slice_value(slice)
    num_or_str(tk_call(@path, 'set', slice))
  end

  def value(val = None)
    num_or_str(tk_call_without_enc(@path, 'value'))
  end
  alias sum_value value
end

class Tk::Vu::PieSlice
  SliceID_TBL = TkCore::INTERP.create_table

  (Pie_Slice_ID = ['vu:pie'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    SliceID_TBL.mutex.synchronize{ SliceID_TBL.clear }
  }

  def self.id2obj(pie, id)
    pie_path = pie.path
    SliceID_TBL.mutex.synchronize{
      if SliceID_TBL[pie_path]
        SliceID_TBL[pie_path][id]? SliceID_TBL[pie_path][id]: id
      else
        id
      end
    }
  end

  def initialize(parent, *args)
    unless parent.kind_of?(Tk::Vu::Pie)
      fail ArgumentError, "expect a Tk::Vu::Pie instance for 1st argument"
    end
    @parent = @pie = parent
    @ppath = parent.path
    Pie_Slice_ID.mutex.synchronize{
      @path = @id = Pie_Slice_ID.join(TkCore::INTERP._ip_id_)
      Pie_Slice_ID[1].succ!
    }
    SliceID_TBL.mutex.synchronize{
      SliceID_TBL[@ppath] = {} unless SliceID_TBL[@ppath]
      SliceID_TBL[@ppath][@id] = self
    }

    if args[-1].kind_of?(Hash)
      keys = args.unshift
    end
    @pie.set(@id, *args)
    configure(keys)
  end

  def id
    @id
  end

  def [](key)
    cget key
  end

  def []=(key,val)
    configure key, val
    val
  end

  def cget_tkstring(slot)
    @pie.itemcget_tkstring(@id, slot)
  end

  def cget(slot)
    @pie.itemcget(@id, slot)
  end

  def cget_strict(slot)
    @pie.itemcget_strict(@id, slot)
  end

  def configure(*args)
    @pie.itemconfigure(@id, *args)
    self
  end

  def configinfo(*args)
    @pie.itemconfiginfo(@id, *args)
  end

  def current_configinfo(*args)
    @pie.current_itemconfiginfo(@id, *args)
  end

  def delete
    @pie.delete(@id)
  end

  def explode(value)
    @pie.explode(@id, value)
    self
  end

  def explode_value
    @pie.explode_value(@id)
  end

  def lower(other=None)
    @pie.lower(@id, other)
    self
  end

  def raise(other=None)
    @pie.raise(@id, other)
    self
  end

  def set(value)
    @pie.set(@id, value)
    self
  end
  alias set_value set

  def value
    @pie.set(@id)
  end
end

class Tk::Vu::NamedPieSlice
  def self.new(parent, name, *args)
    obj = nil
    SliceID_TBL.mutex.synchronize{
      if SliceID_TBL[parent.path] && SliceID_TBL[parent.path][name]
        obj = SliceID_TBL[parent.path][name]
      else
        #super(parent, name, *args)
        unless parent.kind_of?(Tk::Vu::Pie)
          fail ArgumentError, "expect a Tk::Vu::Pie instance for 1st argument"
        end
        obj = self.allocate
        obj.instance_eval{
          @parent = @pie = parent
          @ppath = parent.path
          @path = @id = name.to_s
          SliceID_TBL[@ppath] = {} unless SliceID_TBL[@ppath]
          SliceID_TBL[@ppath][@id] = self
        }
      end
    }
    obj.instance_eval{
      if args[-1].kind_of?(Hash)
        keys = args.unshift
      end
      @pie.set(@id, *args)
      configure(keys)
    }

    obj
  end

  def initialize(parent, name, *args)
    # dummy:: not called by 'new' method
    unless parent.kind_of?(Tk::Vu::Pie)
      fail ArgumentError, "expect a Tk::Vu::Pie instance for 1st argument"
    end
    @parent = @pie = parent
    @ppath = parent.path
    @path = @id = name.to_s
    SliceID_TBL.mutex.synchronize{
      SliceID_TBL[@ppath] = {} unless SliceID_TBL[@ppath]
      SliceID_TBL[@ppath][@id] = self
    }

    if args[-1].kind_of?(Hash)
      keys = args.unshift
    end
    @pie.set(@id, *args)
    configure(keys)
  end
end
