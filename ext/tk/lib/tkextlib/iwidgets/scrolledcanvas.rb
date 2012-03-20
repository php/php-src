#
#  tkextlib/iwidgets/scrolledcanvas.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/canvas'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scrolledcanvas < Tk::Iwidgets::Scrolledwidget
    end
  end
end

class Tk::Iwidgets::Scrolledcanvas
  TkCommandNames = ['::iwidgets::scrolledcanvas'.freeze].freeze
  WidgetClassName = 'Scrolledcanvas'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ################################

  def __boolval_optkeys
    super() << 'autoresize'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'textbackground'
  end
  private :__strval_optkeys

  def initialize(*args)
    super(*args)
    @canvas = component_widget('canvas')
  end

  def method_missing(id, *args)
    if @canvas.respond_to?(id)
      @canvas.__send__(id, *args)
    else
      super(id, *args)
    end
  end

  ################################

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def justify(dir)
    tk_call(@path, 'justify', dir)
    self
  end

  ##########################
  include TkCanvasItemConfig

  def tagid(tag)
    if tag.kind_of?(TkcItem) || tag.kind_of?(TkcTag)
      tag.id
    elsif tag.kind_of?(Tk::Itk::Component)
      tag.name
    else
      tag  # maybe an Array of configure paramters
    end
  end
  private :tagid

  # create a canvas item without creating a TkcItem object
  def create(type, *args)
    type.create(self, *args)
  end

  #######################

  def addtag(tag, mode, *args)
    tk_send_without_enc('addtag', tagid(tag), mode, *args)
    self
  end
  def addtag_above(tagOrId, target)
    addtag(tagOrId, 'above', tagid(target))
  end
  def addtag_all(tagOrId)
    addtag(tagOrId, 'all')
  end
  def addtag_below(tagOrId, target)
    addtag(tagOrId, 'below', tagid(target))
  end
  def addtag_closest(tagOrId, x, y, halo=None, start=None)
    addtag(tagOrId, 'closest', x, y, halo, start)
  end
  def addtag_enclosed(tagOrId, x1, y1, x2, y2)
    addtag(tagOrId, 'enclosed', x1, y1, x2, y2)
  end
  def addtag_overlapping(tagOrId, x1, y1, x2, y2)
    addtag(tagOrId, 'overlapping', x1, y1, x2, y2)
  end
  def addtag_withtag(tagOrId, tag)
    addtag(tagOrId, 'withtag', tagid(tag))
  end

  def bbox(tagOrId, *tags)
    list(tk_send_without_enc('bbox', tagid(tagOrId),
                             *tags.collect{|t| tagid(t)}))
  end

  #def itembind(tag, context, cmd=Proc.new, *args)
  #  _bind([path, "bind", tagid(tag)], context, cmd, *args)
  #  self
  #end
  def itembind(tag, context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([path, "bind", tagid(tag)], context, cmd, *args)
    self
  end

  #def itembind_append(tag, context, cmd=Proc.new, *args)
  #  _bind_append([path, "bind", tagid(tag)], context, cmd, *args)
  #  self
  #end
  def itembind_append(tag, context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([path, "bind", tagid(tag)], context, cmd, *args)
    self
  end

  def itembind_remove(tag, context)
    _bind_remove([path, "bind", tagid(tag)], context)
    self
  end

  def itembindinfo(tag, context=nil)
    _bindinfo([path, "bind", tagid(tag)], context)
  end

  def canvasx(screen_x, *args)
    #tk_tcl2ruby(tk_send_without_enc('canvasx', screen_x, *args))
    number(tk_send_without_enc('canvasx', screen_x, *args))
  end
  def canvasy(screen_y, *args)
    #tk_tcl2ruby(tk_send_without_enc('canvasy', screen_y, *args))
    number(tk_send_without_enc('canvasy', screen_y, *args))
  end

  def coords(tag, *args)
    if args == []
      tk_split_list(tk_send_without_enc('coords', tagid(tag)))
    else
      tk_send_without_enc('coords', tagid(tag), *(args.flatten))
      self
    end
  end

  def dchars(tag, first, last=None)
    tk_send_without_enc('dchars', tagid(tag),
                        _get_eval_enc_str(first), _get_eval_enc_str(last))
    self
  end

  def delete(*args)
    tbl = nil
    TkcItem::CItemID_TBL.mutex.synchronize{
      tbl = TkcItem::CItemID_TBL[self.path]
    }
    if tbl
      find('withtag', *args).each{|item|
        if item.kind_of?(TkcItem)
          TkcItem::CItemID_TBL.mutex.synchronize{
            tbl.delete(item.id)
          }
        end
      }
    end
    tk_send_without_enc('delete', *args.collect{|t| tagid(t)})
    self
  end
  alias remove delete

  def dtag(tag, tag_to_del=None)
    tk_send_without_enc('dtag', tagid(tag), tag_to_del)
    self
  end

  def find(mode, *args)
    list(tk_send_without_enc('find', mode, *args)).collect!{|id|
      TkcItem.id2obj(self, id)
    }
  end
  def find_above(target)
    find('above', tagid(target))
  end
  def find_all
    find('all')
  end
  def find_below(target)
    find('below', tagid(target))
  end
  def find_closest(x, y, halo=None, start=None)
    find('closest', x, y, halo, start)
  end
  def find_enclosed(x1, y1, x2, y2)
    find('enclosed', x1, y1, x2, y2)
  end
  def find_overlapping(x1, y1, x2, y2)
    find('overlapping', x1, y1, x2, y2)
  end
  def find_withtag(tag)
    find('withtag', tag)
  end

  def itemfocus(tagOrId=nil)
    if tagOrId
      tk_send_without_enc('focus', tagid(tagOrId))
      self
    else
      ret = tk_send_without_enc('focus')
      if ret == ""
        nil
      else
        TkcItem.id2obj(self, ret)
      end
    end
  end

  def gettags(tagOrId)
    list(tk_send_without_enc('gettags', tagid(tagOrId))).collect{|tag|
      TkcTag.id2obj(self, tag)
    }
  end

  def icursor(tagOrId, index)
    tk_send_without_enc('icursor', tagid(tagOrId), index)
    self
  end

  def index(tagOrId, idx)
    number(tk_send_without_enc('index', tagid(tagOrId), idx))
  end

  def insert(tagOrId, index, string)
    tk_send_without_enc('insert', tagid(tagOrId), index,
                        _get_eval_enc_str(string))
    self
  end

  def lower(tag, below=nil)
    if below
      tk_send_without_enc('lower', tagid(tag), tagid(below))
    else
      tk_send_without_enc('lower', tagid(tag))
    end
    self
  end

  def move(tag, x, y)
    tk_send_without_enc('move', tagid(tag), x, y)
    self
  end

  def postscript(keys)
    tk_send("postscript", *hash_kv(keys))
  end

  def raise(tag, above=nil)
    if above
      tk_send_without_enc('raise', tagid(tag), tagid(above))
    else
      tk_send_without_enc('raise', tagid(tag))
    end
    self
  end

  def scale(tag, x, y, xs, ys)
    tk_send_without_enc('scale', tagid(tag), x, y, xs, ys)
    self
  end

  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send_without_enc('scan', 'dragto', x, y)
    self
  end

  def select(mode, *args)
    r = tk_send_without_enc('select', mode, *args)
    (mode == 'item')? TkcItem.id2obj(self, r): self
  end
  def select_adjust(tagOrId, index)
    select('adjust', tagid(tagOrId), index)
  end
  def select_clear
    select('clear')
  end
  def select_from(tagOrId, index)
    select('from', tagid(tagOrId), index)
  end
  def select_item
    select('item')
  end
  def select_to(tagOrId, index)
    select('to', tagid(tagOrId), index)
  end

  def itemtype(tag)
    TkcItem.type2class(tk_send('type', tagid(tag)))
  end

  def xview(*index)
    if index.size == 0
      list(tk_send_without_enc('xview'))
    else
      tk_send_without_enc('xview', *index)
      self
    end
  end
  def xview_moveto(*index)
    xview('moveto', *index)
  end
  def xview_scroll(*index)
    xview('scroll', *index)
  end

  def yview(*index)
    if index.size == 0
      list(tk_send_without_enc('yview'))
    else
      tk_send_without_enc('yview', *index)
      self
    end
  end
  def yview_moveto(*index)
    yview('moveto', *index)
  end
  def yview_scroll(*index)
    yview('scroll', *index)
  end
end
