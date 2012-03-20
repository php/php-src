#
#               tk/canvas.rb - Tk canvas classes
#                       by Yukihiro Matsumoto <matz@caelum.co.jp>
#
require 'tk'
require 'tk/canvastag'
require 'tk/itemconfig'
require 'tk/scrollable'

module TkCanvasItemConfig
  include TkItemConfigMethod

  def __item_strval_optkeys(id)
    # maybe need to override
    super(id) + [
      'fill', 'activefill', 'disabledfill',
      'outline', 'activeoutline', 'disabledoutline'
    ]
  end
  private :__item_strval_optkeys

  def __item_methodcall_optkeys(id)
    {'coords'=>'coords'}
  end
  private :__item_methodcall_optkeys

  def __item_val2ruby_optkeys(id)  # { key=>proc, ... }
    super(id).update('window'=>proc{|i, v| window(v)},
                     'tags'=>proc{|i, v|
                       simplelist(v).collect{|tag| TkcTag.id2obj(self, tag)}
                     })
  end
  private :__item_val2ruby_optkeys

  def __item_pathname(tagOrId)
    if tagOrId.kind_of?(TkcItem) || tagOrId.kind_of?(TkcTag)
      self.path + ';' + tagOrId.id.to_s
    else
      self.path + ';' + tagOrId.to_s
    end
  end
  private :__item_pathname
end

class Tk::Canvas<TkWindow
  include TkCanvasItemConfig
  include Tk::Scrollable

  TkCommandNames = ['canvas'.freeze].freeze
  WidgetClassName = 'Canvas'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __destroy_hook__
    TkcItem::CItemID_TBL.delete(@path)
  end

  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('canvas', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('canvas', @path)
  #  end
  #end
  #private :create_self

  def __numval_optkeys
    super() + ['closeenough']
  end
  private :__numval_optkeys

  def __boolval_optkeys
    super() + ['confine']
  end
  private :__boolval_optkeys

  def tagid(tag)
    if tag.kind_of?(TkcItem) || tag.kind_of?(TkcTag)
      tag.id
    else
      tag  # maybe an Array of configure paramters
    end
  end
  private :tagid


  # create a canvas item without creating a TkcItem object
  def create(type, *args)
    type = TkcItem.type2class(type.to_s) unless type.kind_of?(TkcItem)
    type.create(self, *args)
  end


  def addtag(tag, mode, *args)
    mode = mode.to_s
    if args[0] && mode =~ /^(above|below|with(tag)?)$/
      args[0] = tagid(args[0])
    end
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
  alias canvas_x canvasx
  alias canvas_y canvasy

  def coords(tag, *args)
    if args.empty?
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
      args.each{|tag|
        find('withtag', tag).each{|item|
          if item.kind_of?(TkcItem)
            TkcItem::CItemID_TBL.mutex.synchronize{
              tbl.delete(item.id)
            }
          end
        }
      }
    end
    tk_send_without_enc('delete', *args.collect{|t| tagid(t)})
    self
  end
  alias remove delete

  def dtag(tag, tag_to_del=None)
    tk_send_without_enc('dtag', tagid(tag), tagid(tag_to_del))
    self
  end
  alias deltag dtag

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

  def imove(tagOrId, idx, x, y)
    tk_send_without_enc('imove', tagid(tagOrId), idx, x, y)
    self
  end
  alias i_move imove

  def index(tagOrId, idx)
    number(tk_send_without_enc('index', tagid(tagOrId), idx))
  end

  def insert(tagOrId, index, string)
    tk_send_without_enc('insert', tagid(tagOrId), index,
                        _get_eval_enc_str(string))
    self
  end

=begin
  def itemcget(tagOrId, option)
    case option.to_s
    when 'dash', 'activedash', 'disableddash'
      conf = tk_send_without_enc('itemcget', tagid(tagOrId), "-#{option}")
      if conf =~ /^[0-9]/
        list(conf)
      else
        conf
      end
    when 'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile'
      _fromUTF8(tk_send_without_enc('itemcget', tagid(tagOrId), "-#{option}"))
    when 'font', 'kanjifont'
      #fnt = tk_tcl2ruby(tk_send('itemcget', tagid(tagOrId), "-#{option}"))
      fnt = tk_tcl2ruby(_fromUTF8(tk_send_with_enc('itemcget', tagid(tagOrId), '-font')))
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(tagid(tagOrId), fnt)
      end
      if option.to_s == 'kanjifont' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(_fromUTF8(tk_send_without_enc('itemcget', tagid(tagOrId),
                                                "-#{option}")))
    end
  end

  def itemconfigure(tagOrId, key, value=None)
    if key.kind_of? Hash
      key = _symbolkey2str(key)
      coords = key.delete('coords')
      self.coords(tagOrId, coords) if coords

      if ( key['font'] || key['kanjifont'] \
          || key['latinfont'] || key['asciifont'] )
        tagfont_configure(tagid(tagOrId), key.dup)
      else
        _fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId),
                                      *hash_kv(key, true)))
      end

    else
      if ( key == 'coords' || key == :coords )
        self.coords(tagOrId, value)
      elsif ( key == 'font' || key == :font ||
              key == 'kanjifont' || key == :kanjifont ||
              key == 'latinfont' || key == :latinfont ||
              key == 'asciifont' || key == :asciifont )
        if value == None
          tagfontobj(tagid(tagOrId))
        else
          tagfont_configure(tagid(tagOrId), {key=>value})
        end
      else
        _fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId),
                                      "-#{key}", _get_eval_enc_str(value)))
      end
    end
    self
  end
#  def itemconfigure(tagOrId, key, value=None)
#    if key.kind_of? Hash
#      tk_send 'itemconfigure', tagid(tagOrId), *hash_kv(key)
#    else
#      tk_send 'itemconfigure', tagid(tagOrId), "-#{key}", value
#    end
#  end
#  def itemconfigure(tagOrId, keys)
#    tk_send 'itemconfigure', tagid(tagOrId), *hash_kv(keys)
#  end

  def itemconfiginfo(tagOrId, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'coords'
          return ['coords', '', '', '', self.coords(tagOrId)]
        when 'dash', 'activedash', 'disableddash'
          conf = tk_split_simplelist(tk_send_without_enc('itemconfigure', tagid(tagOrId), "-#{key}"))
          if conf[3] && conf[3] =~ /^[0-9]/
            conf[3] = list(conf[3])
          end
          if conf[4] && conf[4] =~ /^[0-9]/
            conf[4] = list(conf[4])
          end
        when 'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId), "-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId),"-#{key}")))
          conf[4] = tagfont_configinfo(tagid(tagOrId), conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId), "-#{key}")))
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        ret = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId)))).collect{|conflist|
          conf = tk_split_simplelist(conflist)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile'
          when 'dash', 'activedash', 'disableddash'
            if conf[3] && conf[3] =~ /^[0-9]/
              conf[3] = list(conf[3])
            end
            if conf[4] && conf[4] =~ /^[0-9]/
              conf[4] = list(conf[4])
            end
          else
            if conf[3]
              if conf[3].index('{')
                conf[3] = tk_split_list(conf[3])
              else
                conf[3] = tk_tcl2ruby(conf[3])
              end
            end
            if conf[4]
              if conf[4].index('{')
                conf[4] = tk_split_list(conf[4])
              else
                conf[4] = tk_tcl2ruby(conf[4])
              end
            end
          end
          conf[1] = conf[1][1..-1] if conf.size == 2 # alias info
          conf
        }

        fontconf = ret.assoc('font')
        if fontconf
          ret.delete_if{|item| item[0] == 'font' || item[0] == 'kanjifont'}
          fontconf[4] = tagfont_configinfo(tagid(tagOrId), fontconf[4])
          ret.push(fontconf)
        end

        ret << ['coords', '', '', '', self.coords(tagOrId)]
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'coords'
          {'coords' => ['', '', '', self.coords(tagOrId)]}
        when 'dash', 'activedash', 'disableddash'
          conf = tk_split_simplelist(tk_send_without_enc('itemconfigure',
                                                         tagid(tagOrId),
                                                         "-#{key}"))
          if conf[3] && conf[3] =~ /^[0-9]/
            conf[3] = list(conf[3])
          end
          if conf[4] && conf[4] =~ /^[0-9]/
            conf[4] = list(conf[4])
          end
        when 'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId), "-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId),"-#{key}")))
          conf[4] = tagfont_configinfo(tagid(tagOrId), conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId), "-#{key}")))
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', tagid(tagOrId)))).each{|conflist|
          conf = tk_split_simplelist(conflist)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile'
          when 'dash', 'activedash', 'disableddash'
            if conf[2] && conf[2] =~ /^[0-9]/
              conf[2] = list(conf[2])
            end
            if conf[3] && conf[3] =~ /^[0-9]/
              conf[3] = list(conf[3])
            end
          else
            if conf[2]
              if conf[2].index('{')
                conf[2] = tk_split_list(conf[2])
              else
                conf[2] = tk_tcl2ruby(conf[2])
              end
            end
            if conf[3]
              if conf[3].index('{')
                conf[3] = tk_split_list(conf[3])
              else
                conf[3] = tk_tcl2ruby(conf[3])
              end
            end
          end
          if conf.size == 1
            ret[key] = conf[0][1..-1]  # alias info
          else
            ret[key] = conf
          end
        }

        fontconf = ret['font']
        if fontconf
          ret.delete('font')
          ret.delete('kanjifont')
          fontconf[3] = tagfont_configinfo(tagid(tagOrId), fontconf[3])
          ret['font'] = fontconf
        end

        ret['coords'] = ['', '', '', self.coords(tagOrId)]

        ret
      end
    end
  end

  def current_itemconfiginfo(tagOrId, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        conf = itemconfiginfo(tagOrId, key)
        {conf[0] => conf[4]}
      else
        ret = {}
        itemconfiginfo(tagOrId).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      itemconfiginfo(tagOrId, key).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end
=end

  def lower(tag, below=nil)
    if below
      tk_send_without_enc('lower', tagid(tag), tagid(below))
    else
      tk_send_without_enc('lower', tagid(tag))
    end
    self
  end

  def move(tag, dx, dy)
    tk_send_without_enc('move', tagid(tag), dx, dy)
    self
  end

  def moveto(tag, x, y)
    # Tcl/Tk 8.6 or later
    tk_send_without_enc('moveto', tagid(tag), x, y)
    self
  end
  alias move_to moveto

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

  def rchars(tag, first, last, str_or_coords)
    # Tcl/Tk 8.6 or later
    str_or_coords = str_or_coords.flatten if str_or_coords.kinad_of? Array
    tk_send_without_enc('rchars', tagid(tag), first, last, str_or_coords)
    self
  end
  alias replace_chars rchars
  alias replace_coords rchars

  def scale(tag, x, y, xs, ys)
    tk_send_without_enc('scale', tagid(tag), x, y, xs, ys)
    self
  end

  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y, gain=None)
    tk_send_without_enc('scan', 'dragto', x, y, gain)
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

  def create_itemobj_from_id(idnum)
    id = TkcItem.id2obj(self, idnum.to_i)
    return id if id.kind_of?(TkcItem)

    typename = tk_send('type', id)
    unless type = TkcItem.type2class(typename)
      (itemclass = typename.dup)[0,1] = typename[0,1].upcase
      type = TkcItem.const_set(itemclass, Class.new(TkcItem))
      type.const_set("CItemTypeName", typename.freeze)
      TkcItem::CItemTypeToClass[typename] = type
    end

    canvas = self
    (obj = type.allocate).instance_eval{
      @parent = @c = canvas
      @path = canvas.path
      @id = id
      TkcItem::CItemID_TBL.mutex.synchronize{
        TkcItem::CItemID_TBL[@path] = {} unless TkcItem::CItemID_TBL[@path]
        TkcItem::CItemID_TBL[@path][@id] = self
      }
    }
  end
end

#TkCanvas = Tk::Canvas unless Object.const_defined? :TkCanvas
#Tk.__set_toplevel_aliases__(:Tk, Tk::Canvas, :TkCanvas)
Tk.__set_loaded_toplevel_aliases__('tk/canvas.rb', :Tk, Tk::Canvas, :TkCanvas)


class TkcItem<TkObject
  extend Tk
  include TkcTagAccess
  extend TkItemFontOptkeys
  extend TkItemConfigOptkeys

  CItemTypeName = nil
  CItemTypeToClass = {}

  CItemID_TBL = TkCore::INTERP.create_table

  TkCore::INTERP.init_ip_env{
    CItemID_TBL.mutex.synchronize{ CItemID_TBL.clear }
  }

  def TkcItem.type2class(type)
    CItemTypeToClass[type]
  end

  def TkcItem.id2obj(canvas, id)
    cpath = canvas.path
    CItemID_TBL.mutex.synchronize{
      if CItemID_TBL[cpath]
        CItemID_TBL[cpath][id]? CItemID_TBL[cpath][id]: id
      else
        id
      end
    }
  end

  ########################################
  def self._parse_create_args(args)
    fontkeys = {}
    methodkeys = {}
    if args[-1].kind_of? Hash
      keys = _symbolkey2str(args.pop)
      if args.size == 0
        args = keys.delete('coords')
        unless args.kind_of?(Array)
          fail "coords parameter must be given by an Array"
        end
      end

      #['font', 'kanjifont', 'latinfont', 'asciifont'].each{|key|
      #  fontkeys[key] = keys.delete(key) if keys.key?(key)
      #}
      __item_font_optkeys(nil).each{|key|
        fkey = key.to_s
        fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

        fkey = "kanji#{key}"
        fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

        fkey = "latin#{key}"
        fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

        fkey = "ascii#{key}"
        fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)
      }

      __item_optkey_aliases(nil).each{|alias_name, real_name|
        alias_name = alias_name.to_s
        if keys.has_key?(alias_name)
          keys[real_name.to_s] = keys.delete(alias_name)
        end
      }

      __item_methodcall_optkeys(nil).each{|key|
        key = key.to_s
        methodkeys[key] = keys.delete(key) if keys.key?(key)
      }

      __item_ruby2val_optkeys(nil).each{|key, method|
        key = key.to_s
        keys[key] = method.call(keys[key]) if keys.has_key?(key)
      }

      #args = args.flatten.concat(hash_kv(keys))
      args = args.flatten.concat(itemconfig_hash_kv(nil, keys))
    else
      args = args.flatten
    end

    [args, fontkeys, methodkeys]
  end
  private_class_method :_parse_create_args

  def self.create(canvas, *args)
    unless self::CItemTypeName
      fail RuntimeError, "#{self} is an abstract class"
    end
    args, fontkeys, methodkeys = _parse_create_args(args)
    idnum = tk_call_without_enc(canvas.path, 'create',
                                self::CItemTypeName, *args)
    canvas.itemconfigure(idnum, fontkeys) unless fontkeys.empty?
    canvas.itemconfigure(idnum, methodkeys) unless methodkeys.empty?
    idnum.to_i  # 'canvas item id' is an integer number
  end
  ########################################

  def initialize(parent, *args)
    #unless parent.kind_of?(Tk::Canvas)
    #  fail ArgumentError, "expect Tk::Canvas for 1st argument"
    #end
    @parent = @c = parent
    @path = parent.path

    @id = create_self(*args) # an integer number as 'canvas item id'
    CItemID_TBL.mutex.synchronize{
      CItemID_TBL[@path] = {} unless CItemID_TBL[@path]
      CItemID_TBL[@path][@id] = self
    }
  end
  def create_self(*args)
    self.class.create(@c, *args) # return an integer number as 'canvas item id'
  end
  private :create_self

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
    CItemID_TBL.mutex.synchronize{
      CItemID_TBL[@path].delete(@id) if CItemID_TBL[@path]
    }
    self
  end
  alias remove  delete
  alias destroy delete
end

class TkcArc<TkcItem
  CItemTypeName = 'arc'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcBitmap<TkcItem
  CItemTypeName = 'bitmap'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcImage<TkcItem
  CItemTypeName = 'image'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcLine<TkcItem
  CItemTypeName = 'line'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcOval<TkcItem
  CItemTypeName = 'oval'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcPolygon<TkcItem
  CItemTypeName = 'polygon'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcRectangle<TkcItem
  CItemTypeName = 'rectangle'.freeze
  CItemTypeToClass[CItemTypeName] = self
end

class TkcText<TkcItem
  CItemTypeName = 'text'.freeze
  CItemTypeToClass[CItemTypeName] = self
  def self.create(canvas, *args)
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
      txt = keys['text']
      keys['text'] = _get_eval_enc_str(txt) if txt
      args.push(keys)
    end
    super(canvas, *args)
  end
end

class TkcWindow<TkcItem
  CItemTypeName = 'window'.freeze
  CItemTypeToClass[CItemTypeName] = self
  def self.create(canvas, *args)
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
      win = keys['window']
      # keys['window'] = win.epath if win.kind_of?(TkWindow)
      keys['window'] = _epath(win) if win
      args.push(keys)
    end
    super(canvas, *args)
  end
end
