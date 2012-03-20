#
# tk/canvastag.rb - methods for treating canvas tags
#
require 'tk'
require 'tk/tagfont'

module TkcTagAccess
  include TkComm
  include TkTreatTagFont
end

require 'tk/canvas'

module TkcTagAccess
  def addtag(tag)
    @c.addtag(tag, 'withtag', @id)
    self
  end

  def bbox
    @c.bbox(@id)
  end

  #def bind(seq, cmd=Proc.new, *args)
  #  @c.itembind(@id, seq, cmd, *args)
  #  self
  #end
  def bind(seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    @c.itembind(@id, seq, cmd, *args)
    self
  end

  #def bind_append(seq, cmd=Proc.new, *args)
  #  @c.itembind_append(@id, seq, cmd, *args)
  #  self
  #end
  def bind_append(seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    @c.itembind_append(@id, seq, cmd, *args)
    self
  end

  def bind_remove(seq)
    @c.itembind_remove(@id, seq)
    self
  end

  def bindinfo(seq=nil)
    @c.itembindinfo(@id, seq)
  end

  def cget_tkstring(option)
    @c.itemcget_tkstring(@id, option)
  end
  def cget(option)
    @c.itemcget(@id, option)
  end
  def cget_strict(option)
    @c.itemcget_strict(@id, option)
  end

  def configure(key, value=None)
    @c.itemconfigure(@id, key, value)
    self
  end
#  def configure(keys)
#    @c.itemconfigure @id, keys
#  end

  def configinfo(key=nil)
    @c.itemconfiginfo(@id, key)
  end

  def current_configinfo(key=nil)
    @c.current_itemconfiginfo(@id, key)
  end

  def coords(*args)
    @c.coords(@id, *args)
  end

  def dchars(first, last=None)
    @c.dchars(@id, first, last)
    self
  end

  def dtag(tag_to_del=None)
    @c.dtag(@id, tag_to_del)
    self
  end
  alias deltag dtag

  def find
    @c.find('withtag', @id)
  end
  alias list find

  def focus
    @c.itemfocus(@id)
  end

  def gettags
    @c.gettags(@id)
  end

  def icursor(index)
    @c.icursor(@id, index)
    self
  end

  def imove(idx, x, y)
    # Tcl/Tk 8.6 or later
    @c.imove(@id, idx, x, y)
    self
  end
  alias i_move imove

  def index(idx)
    @c.index(@id, idx)
  end

  def insert(beforethis, string)
    @c.insert(@id, beforethis, string)
    self
  end

  def lower(belowthis=None)
    @c.lower(@id, belowthis)
    self
  end

  def move(xamount, yamount)
    @c.move(@id, xamount, yamount)
    self
  end

  def moveto(x, y)
    # Tcl/Tk 8.6 or later
    @c.moveto(@id, x, y)
    self
  end
  alias move_to moveto

  def raise(abovethis=None)
    @c.raise(@id, abovethis)
    self
  end

  def scale(xorigin, yorigin, xscale, yscale)
    @c.scale(@id, xorigin, yorigin, xscale, yscale)
    self
  end

  def rchars(first, last, str_or_coords)
    # Tcl/Tk 8.6 or later
    @c.rchars(@id, first, last, str_or_coords)
    self
  end
  alias replace_chars rchars
  alias replace_coords rchars

  def select_adjust(index)
    @c.select('adjust', @id, index)
    self
  end
  def select_from(index)
    @c.select('from', @id, index)
    self
  end
  def select_to(index)
    @c.select('to', @id, index)
    self
  end

  def itemtype
    @c.itemtype(@id)
  end

  # Following operators support logical expressions of canvas tags
  # (for Tk8.3+).
  # If tag1.path is 't1' and tag2.path is 't2', then
  #      ltag = tag1 & tag2; ltag.path => "(t1)&&(t2)"
  #      ltag = tag1 | tag2; ltag.path => "(t1)||(t2)"
  #      ltag = tag1 ^ tag2; ltag.path => "(t1)^(t2)"
  #      ltag = - tag1;      ltag.path => "!(t1)"
  def & (tag)
    if tag.kind_of? TkObject
      TkcTagString.new(@c, '(' + @id + ')&&(' + tag.path + ')')
    else
      TkcTagString.new(@c, '(' + @id + ')&&(' + tag.to_s + ')')
    end
  end

  def | (tag)
    if tag.kind_of? TkObject
      TkcTagString.new(@c, '(' + @id + ')||(' + tag.path + ')')
    else
      TkcTagString.new(@c, '(' + @id + ')||(' + tag.to_s + ')')
    end
  end

  def ^ (tag)
    if tag.kind_of? TkObject
      TkcTagString.new(@c, '(' + @id + ')^(' + tag.path + ')')
    else
      TkcTagString.new(@c, '(' + @id + ')^(' + tag.to_s + ')')
    end
  end

  def -@
    TkcTagString.new(@c, '!(' + @id + ')')
  end
end

class TkcTag<TkObject
  include TkcTagAccess

  CTagID_TBL = TkCore::INTERP.create_table

  (Tk_CanvasTag_ID = ['ctag'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    CTagID_TBL.mutex.synchronize{ CTagID_TBL.clear }
  }

  def TkcTag.id2obj(canvas, id)
    cpath = canvas.path
    CTagID_TBL.mutex.synchronize{
      if CTagID_TBL[cpath]
        CTagID_TBL[cpath][id]? CTagID_TBL[cpath][id]: id
      else
        id
      end
    }
  end

  def initialize(parent, mode=nil, *args)
    #unless parent.kind_of?(TkCanvas)
    #  fail ArgumentError, "expect TkCanvas for 1st argument"
    #end
    @c = parent
    @cpath = parent.path
    Tk_CanvasTag_ID.mutex.synchronize{
      # @path = @id = Tk_CanvasTag_ID.join('')
      @path = @id = Tk_CanvasTag_ID.join(TkCore::INTERP._ip_id_)
      Tk_CanvasTag_ID[1].succ!
    }
    CTagID_TBL.mutex.synchronize{
      CTagID_TBL[@cpath] = {} unless CTagID_TBL[@cpath]
      CTagID_TBL[@cpath][@id] = self
    }
    if mode
      tk_call_without_enc(@c.path, "addtag", @id, mode, *args)
    end
  end
  def id
    @id
  end

  def exist?
    if @c.find_withtag(@id)
      true
    else
      false
    end
  end

  def delete
    @c.delete @id
    CTagID_TBL.mutex.synchronize{
      CTagID_TBL[@cpath].delete(@id) if CTagID_TBL[@cpath]
    }
    self
  end
  alias remove  delete
  alias destroy delete

  def set_to_above(target)
    @c.addtag_above(@id, target)
    self
  end
  alias above set_to_above

  def set_to_all
    @c.addtag_all(@id)
    self
  end
  alias all set_to_all

  def set_to_below(target)
    @c.addtag_below(@id, target)
    self
  end
  alias below set_to_below

  def set_to_closest(x, y, halo=None, start=None)
    @c.addtag_closest(@id, x, y, halo, start)
    self
  end
  alias closest set_to_closest

  def set_to_enclosed(x1, y1, x2, y2)
    @c.addtag_enclosed(@id, x1, y1, x2, y2)
    self
  end
  alias enclosed set_to_enclosed

  def set_to_overlapping(x1, y1, x2, y2)
    @c.addtag_overlapping(@id, x1, y1, x2, y2)
    self
  end
  alias overlapping set_to_overlapping

  def set_to_withtag(target)
    @c.addtag_withtag(@id, target)
    self
  end
  alias withtag set_to_withtag
end

class TkcTagString<TkcTag
  def self.new(parent, name, mode=nil, *args)
    obj = nil
    CTagID_TBL.mutex.synchronize{
      if CTagID_TBL[parent.path] && CTagID_TBL[parent.path][name]
        obj = CTagID_TBL[parent.path][name]
      else
        # super(parent, name, *args)
        (obj = self.allocate).instance_eval{
          @c = parent
          @cpath = parent.path
          @path = @id = name
          CTagID_TBL[@cpath] = {} unless CTagID_TBL[@cpath]
          CTagID_TBL[@cpath][@id] = self
        }
      end
    }
    if obj && mode
      tk_call_without_enc(@c.path, "addtag", @id, mode, *args)
    end
    obj
  end

  def initialize(parent, name, mode=nil, *args)
    # dummy:: not called by 'new' method

    #unless parent.kind_of?(TkCanvas)
    #  fail ArgumentError, "expect TkCanvas for 1st argument"
    #end
    @c = parent
    @cpath = parent.path
    @path = @id = name

    if mode
      tk_call_without_enc(@c.path, "addtag", @id, mode, *args)
    end
  end
end
TkcNamedTag = TkcTagString

class TkcTagAll<TkcTagString
  def self.new(parent)
    super(parent, 'all')
  end
=begin
  def initialize(parent)
    #unless parent.kind_of?(TkCanvas)
    #  fail ArgumentError, "expect TkCanvas for 1st argument"
    #end
    @c = parent
    @cpath = parent.path
    @path = @id = 'all'
    CTagID_TBL.mutex.synchronize{
      CTagID_TBL[@cpath] = {} unless CTagID_TBL[@cpath]
      CTagID_TBL[@cpath][@id] = self
    }
  end
=end
end

class TkcTagCurrent<TkcTagString
  def self.new(parent)
    super(parent, 'current')
  end
=begin
  def initialize(parent)
    #unless parent.kind_of?(TkCanvas)
    #  fail ArgumentError, "expect TkCanvas for 1st argument"
    #end
    @c = parent
    @cpath = parent.path
    @path = @id = 'current'
    CTagID_TBL.mutex.synchronize{
      CTagID_TBL[@cpath] = {} unless CTagID_TBL[@cpath]
      CTagID_TBL[@cpath][@id] = self
    }
  end
=end
end

class TkcGroup<TkcTag
  (Tk_cGroup_ID = ['tkcg'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  #def create_self(parent, *args)
  def initialize(parent, *args)
    #unless parent.kind_of?(TkCanvas)
    #  fail ArgumentError, "expect TkCanvas for 1st argument"
    #end
    @c = parent
    @cpath = parent.path
    Tk_cGroup_ID.mutex.synchronize{
      # @path = @id = Tk_cGroup_ID.join('')
      @path = @id = Tk_cGroup_ID.join(TkCore::INTERP._ip_id_)
      Tk_cGroup_ID[1].succ!
    }
    CTagID_TBL.mutex.synchronize{
      CTagID_TBL[@cpath] = {} unless CTagID_TBL[@cpath]
      CTagID_TBL[@cpath][@id] = self
    }
    include(*args) if args != []
  end
  #private :create_self

  def include(*tags)
    for i in tags
      #i.addtag(@id)
      @c.addtag_withtag(@id, i)
    end
    self
  end
  alias add include

  def exclude(*tags)
    for i in tags
      #i.dtag(@id)
      @c.dtag(i, @id)
    end
    self
  end
end
