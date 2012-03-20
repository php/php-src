#
#               tk/text.rb - Tk text classes
#                       by Yukihiro Matsumoto <matz@caelum.co.jp>
require 'tk'
require 'tk/itemfont'
require 'tk/itemconfig'
require 'tk/scrollable'
require 'tk/txtwin_abst'

module TkTextTagConfig
  include TkTreatItemFont
  include TkItemConfigMethod

  def __item_cget_cmd(id)  # id := [ type, tagOrId ]
    [self.path, id[0], 'cget', id[1]]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)  # id := [ type, tagOrId ]
    [self.path, id[0], 'configure', id[1]]
  end
  private :__item_config_cmd

  def __item_pathname(id)
    if id.kind_of?(Array)
      id = tagid(id[1])
    end
    [self.path, id].join(';')
  end
  private :__item_pathname

  def tag_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['tag', tagOrId], option)
  end
  def tag_cget(tagOrId, option)
    itemcget(['tag', tagOrId], option)
  end
  def tag_cget_strict(tagOrId, option)
    itemcget_strict(['tag', tagOrId], option)
  end
  def tag_configure(tagOrId, slot, value=None)
    itemconfigure(['tag', tagOrId], slot, value)
  end
  def tag_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['tag', tagOrId], slot)
  end
  def current_tag_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['tag', tagOrId], slot)
  end

  def window_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['window', tagOrId], option)
  end
  def window_cget(tagOrId, option)
    itemcget(['window', tagOrId], option)
  end
  def window_cget_strict(tagOrId, option)
    itemcget_strict(['window', tagOrId], option)
  end
  def window_configure(tagOrId, slot, value=None)
    itemconfigure(['window', tagOrId], slot, value)
  end
  def window_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['window', tagOrId], slot)
  end
  def current_window_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['window', tagOrId], slot)
  end

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo
end

class Tk::Text<TkTextWin
  ItemConfCMD = ['tag'.freeze, 'configure'.freeze].freeze
  #include TkTreatTextTagFont
  include TkTextTagConfig
  include Scrollable

  #######################################

  module IndexModMethods
    def +(mod)
      return chars(mod) if mod.kind_of?(Numeric)

      mod = mod.to_s
      if mod =~ /^\s*[+-]?\d/
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod)
      else
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod)
      end
    end

    def -(mod)
      return chars(-mod) if mod.kind_of?(Numeric)

      mod = mod.to_s
      if mod =~ /^\s*[+-]?\d/
        Tk::Text::IndexString.new(String.new(id) << ' - ' << mod)
      elsif mod =~ /^\s*[-]\s+(\d.*)$/
        Tk::Text::IndexString.new(String.new(id) << ' - -' << $1)
      else
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod)
      end
    end

    def chars(mod)
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' chars')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' chars')
      end
    end
    alias char chars

    def display_chars(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' display chars')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' display chars')
      end
    end
    alias display_char display_chars

    def any_chars(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' any chars')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' any chars')
      end
    end
    alias any_char any_chars

    def indices(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' indices')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' indices')
      end
    end

    def display_indices(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' display indices')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' display indices')
      end
    end

    def any_indices(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' any indices')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' any indices')
      end
    end

    def lines(mod)
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' lines')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' lines')
      end
    end
    alias line lines

    def display_lines(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' display_lines')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' display lines')
      end
    end
    alias display_line display_lines

    def any_lines(mod)
      # Tk8.5 feature
      fail ArgumentError, 'expect Integer'  unless mod.kind_of?(Integer)
      if mod < 0
        Tk::Text::IndexString.new(String.new(id) << ' ' << mod.to_s << ' any_lines')
      else
        Tk::Text::IndexString.new(String.new(id) << ' + ' << mod.to_s << ' any lines')
      end
    end
    alias any_line any_lines

    def linestart
      Tk::Text::IndexString.new(String.new(id) << ' linestart')
    end
    def lineend
      Tk::Text::IndexString.new(String.new(id) << ' lineend')
    end

    def display_linestart
      # Tk8.5 feature
      Tk::Text::IndexString.new(String.new(id) << ' display linestart')
    end
    def display_lineend
      # Tk8.5 feature
      Tk::Text::IndexString.new(String.new(id) << ' display lineend')
    end

    def wordstart
      Tk::Text::IndexString.new(String.new(id) << ' wordstart')
    end
    def wordend
      Tk::Text::IndexString.new(String.new(id) << ' wordend')
    end

    def display_wordstart
      # Tk8.5 feature
      Tk::Text::IndexString.new(String.new(id) << ' display wordstart')
    end
    def display_wordend
      # Tk8.5 feature
      Tk::Text::IndexString.new(String.new(id) << ' display wordend')
    end
  end

  class IndexString < String
    include IndexModMethods

    def self.at(x,y)
      self.new("@#{x},#{y}")
    end

    def self.new(str)
      if str.kind_of?(String)
        super(str)
      elsif str.kind_of?(Symbol)
        super(str.to_s)
      else
        str
      end
    end

    def id
      self
    end
  end

  #######################################

  TkCommandNames = ['text'.freeze].freeze
  WidgetClassName = 'Text'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def self.new(*args, &block)
    obj = super(*args){}
    obj.init_instance_variable
    if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
      obj.instance_exec(obj, &block) if defined? yield
    else
      obj.instance_eval(&block) if defined? yield
    end
    obj
  end

  def init_instance_variable
    @cmdtbl = []
    @tags = {}
  end

  def __destroy_hook__
    TkTextTag::TTagID_TBL.mutex.synchronize{
      TkTextTag::TTagID_TBL.delete(@path)
    }
    TkTextTag::TMarkID_TBL.mutex.synchronize{
      TkTextMark::TMarkID_TBL.delete(@path)
    }
  end

  def create_self(keys)
    #if keys and keys != None
    #  #tk_call_without_enc('text', @path, *hash_kv(keys, true))
    #  tk_call_without_enc(self.class::TkCommandNames[0], @path,
    #                     *hash_kv(keys, true))
    #else
    #  #tk_call_without_enc('text', @path)
    #  tk_call_without_enc(self.class::TkCommandNames[0], @path)
    #end
    super(keys)
    init_instance_variable
  end
  private :create_self

  def __strval_optkeys
    super() << 'inactiveseletcionbackground'
  end
  private :__strval_optkeys

  def self.at(x, y)
    Tk::Text::IndexString.at(x, y)
  end

  def at(x, y)
    Tk::Text::IndexString.at(x, y)
  end

  def index(idx)
    Tk::Text::IndexString.new(tk_send_without_enc('index',
                                                  _get_eval_enc_str(idx)))
  end

  def get_displaychars(*index)
    # Tk8.5 feature
    get('-displaychars', *index)
  end

  def value
    _fromUTF8(tk_send_without_enc('get', "1.0", "end - 1 char"))
  end

  def value= (val)
    tk_send_without_enc('delete', "1.0", 'end')
    tk_send_without_enc('insert', "1.0", _get_eval_enc_str(val))
    val
  end

  def clear
    tk_send_without_enc('delete', "1.0", 'end')
    self
  end
  alias erase clear

  def _addcmd(cmd)
    @cmdtbl.push cmd
  end

  def _addtag(name, obj)
    @tags[name] = obj
  end

  def tagid(tag)
    if tag.kind_of?(TkTextTag) \
      || tag.kind_of?(TkTextMark) \
      || tag.kind_of?(TkTextImage) \
      || tag.kind_of?(TkTextWindow)
      tag.id
    else
      tag  # maybe an Array of configure paramters
    end
  end
  private :tagid

  def tagid2obj(tagid)
    if @tags[tagid]
      @tags[tagid]
    else
      tagid
    end
  end

  def tag_names(index=None)
    #tk_split_simplelist(_fromUTF8(tk_send_without_enc('tag', 'names', _get_eval_enc_str(index)))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('tag', 'names', _get_eval_enc_str(index)), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def mark_names
    #tk_split_simplelist(_fromUTF8(tk_send_without_enc('mark', 'names'))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('mark', 'names'), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def mark_gravity(mark, direction=nil)
    if direction
      tk_send_without_enc('mark', 'gravity',
                          _get_eval_enc_str(mark), direction)
      self
    else
      tk_send_without_enc('mark', 'gravity', _get_eval_enc_str(mark))
    end
  end

  def mark_set(mark, index)
    tk_send_without_enc('mark', 'set', _get_eval_enc_str(mark),
                        _get_eval_enc_str(index))
    self
  end
  alias set_mark mark_set

  def mark_unset(*marks)
    tk_send_without_enc('mark', 'unset',
                        *(marks.collect{|mark| _get_eval_enc_str(mark)}))
    self
  end
  alias unset_mark mark_unset

  def mark_next(index)
    tagid2obj(_fromUTF8(tk_send_without_enc('mark', 'next',
                                            _get_eval_enc_str(index))))
  end
  alias next_mark mark_next

  def mark_previous(index)
    tagid2obj(_fromUTF8(tk_send_without_enc('mark', 'previous',
                                            _get_eval_enc_str(index))))
  end
  alias previous_mark mark_previous

  def image_cget_strict(index, slot)
    case slot.to_s
    when 'text', 'label', 'show', 'data', 'file'
      _fromUTF8(tk_send_without_enc('image', 'cget',
                                    _get_eval_enc_str(index), "-#{slot}"))
    else
      tk_tcl2ruby(_fromUTF8(tk_send_without_enc('image', 'cget',
                                                _get_eval_enc_str(index),
                                                "-#{slot}")))
    end
  end

  def image_cget(index, slot)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      image_cget_strict(index, slot)
    else
      begin
        image_cget_strict(index, slot)
      rescue => e
        begin
          if current_image_configinfo(index).has_key?(slot.to_s)
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

  def image_configure(index, slot, value=None)
    if slot.kind_of?(Hash)
      _fromUTF8(tk_send_without_enc('image', 'configure',
                                    _get_eval_enc_str(index),
                                    *hash_kv(slot, true)))
    else
      _fromUTF8(tk_send_without_enc('image', 'configure',
                                    _get_eval_enc_str(index),
                                    "-#{slot}",
                                    _get_eval_enc_str(value)))
    end
    self
  end

  def image_configinfo(index, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          #conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), false, true)
        else
          #conf = tk_split_list(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_list(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), 0, false, true)
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        # tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)))).collect{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)), false, false).collect{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show', 'data', 'file'
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
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          #conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), false, true)
        else
          #conf = tk_split_list(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_list(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), 0, false, true)
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        #tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)))).each{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)), false, false).each{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show', 'data', 'file'
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
        ret
      end
    end
  end

  def current_image_configinfo(index, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        conf = image_configinfo(index, slot)
        {conf[0] => conf[4]}
      else
        ret = {}
        image_configinfo(index).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      image_configinfo(index, slot).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end

  def image_names
    #tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'names'))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('image', 'names'), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def set_insert(index)
    tk_send_without_enc('mark','set','insert', _get_eval_enc_str(index))
    self
  end

  def set_current(index)
    tk_send_without_enc('mark','set','current', _get_eval_enc_str(index))
    self
  end

  def insert(index, chars, *tags)
    if tags[0].kind_of?(Array)
      # multiple chars-taglist argument :: str, [tag,...], str, [tag,...], ...
      args = [chars]
      while tags.size > 0
        args << tags.shift.collect{|x|_get_eval_string(x)}.join(' ')  # taglist
        args << tags.shift if tags.size > 0                           # chars
      end
      super(index, *args)
    else
      # single chars-taglist argument :: str, tag, tag, ...
      if tags.size == 0
        super(index, chars)
      else
        super(index, chars, tags.collect{|x|_get_eval_string(x)}.join(' '))
      end
    end
  end

  def destroy
    @tags = {} unless @tags
    @tags.each_value do |t|
      t.destroy
    end
    super()
  end

  def backspace
    self.delete 'insert'
  end

  def bbox(index)
    list(tk_send_without_enc('bbox', _get_eval_enc_str(index)))
  end

  def compare(idx1, op, idx2)
    bool(tk_send_without_enc('compare', _get_eval_enc_str(idx1),
                             op, _get_eval_enc_str(idx2)))
  end

  def count(idx1, idx2, *opts)
    # opts are Tk8.5 feature
    cnt = 0
    args = opts.collect{|opt|
      str = opt.to_s
      cnt += 1 if str != 'update'
      '-' + str
    }
    args << _get_eval_enc_str(idx1) << _get_eval_enc_str(idx2)
    if cnt <= 1
      number(tk_send_without_enc('count', *opts))
    else
      list(tk_send_without_enc('count', *opts))
    end
  end

  def count_info(idx1, idx2, update=true)
    # Tk8.5 feature
    opts = [
      :chars, :displaychars, :displayindices, :displaylines,
      :indices, :lines, :xpixels, :ypixels
    ]
    if update
      lst = count(idx1, idx2, :update, *opts)
    else
      lst = count(idx1, idx2, *opts)
    end
    info = {}
    opts.each_with_index{|key, idx| info[key] = lst[idx]}
    info
  end

  def peer_names()
    # Tk8.5 feature
    list(tk_send_without_enc('peer', 'names'))
  end

  def replace(idx1, idx2, *opts)
    tk_send('replace', idx1, idx2, *opts)
    self
  end

  def debug
    bool(tk_send_without_enc('debug'))
  end
  def debug=(boolean)
    tk_send_without_enc('debug', boolean)
    #self
    boolean
  end

  def dlineinfo(index)
    list(tk_send_without_enc('dlineinfo', _get_eval_enc_str(index)))
  end

  def modified?
    bool(tk_send_without_enc('edit', 'modified'))
  end
  def modified(mode)
    tk_send_without_enc('edit', 'modified', mode)
    self
  end
  def modified=(mode)
    modified(mode)
    mode
  end

  def edit_redo
    tk_send_without_enc('edit', 'redo')
    self
  end
  def edit_reset
    tk_send_without_enc('edit', 'reset')
    self
  end
  def edit_separator
    tk_send_without_enc('edit', 'separator')
    self
  end
  def edit_undo
    tk_send_without_enc('edit', 'undo')
    self
  end

  def xview_pickplace(index)
    tk_send_without_enc('xview', '-pickplace', _get_eval_enc_str(index))
    self
  end

  def yview_pickplace(index)
    tk_send_without_enc('yview', '-pickplace', _get_eval_enc_str(index))
    self
  end

  def text_copy
    # Tk8.4 feature
    tk_call_without_enc('tk_textCopy', @path)
    self
  end

  def text_cut
    # Tk8.4 feature
    tk_call_without_enc('tk_textCut', @path)
    self
  end

  def text_paste
    # Tk8.4 feature
    tk_call_without_enc('tk_textPaste', @path)
    self
  end

  def tag_add(tag, index1, index2=None)
    tk_send_without_enc('tag', 'add', _get_eval_enc_str(tag),
                        _get_eval_enc_str(index1),
                        _get_eval_enc_str(index2))
    self
  end
  alias addtag tag_add
  alias add_tag tag_add

  def tag_delete(*tags)
    tk_send_without_enc('tag', 'delete',
                        *(tags.collect{|tag| _get_eval_enc_str(tag)}))
    TkTextTag::TTagID_TBL.mutex.synchronize{
      if TkTextTag::TTagID_TBL[@path]
        tags.each{|tag|
          if tag.kind_of?(TkTextTag)
            TkTextTag::TTagID_TBL[@path].delete(tag.id)
          else
            TkTextTag::TTagID_TBL[@path].delete(tag)
          end
        }
      end
    }
    self
  end
  alias deltag tag_delete
  alias delete_tag tag_delete

  #def tag_bind(tag, seq, cmd=Proc.new, *args)
  #  _bind([@path, 'tag', 'bind', tag], seq, cmd, *args)
  #  self
  #end
  def tag_bind(tag, seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([@path, 'tag', 'bind', tag], seq, cmd, *args)
    self
  end

  #def tag_bind_append(tag, seq, cmd=Proc.new, *args)
  #  _bind_append([@path, 'tag', 'bind', tag], seq, cmd, *args)
  #  self
  #end
  def tag_bind_append(tag, seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([@path, 'tag', 'bind', tag], seq, cmd, *args)
    self
  end

  def tag_bind_remove(tag, seq)
    _bind_remove([@path, 'tag', 'bind', tag], seq)
    self
  end

  def tag_bindinfo(tag, context=nil)
    _bindinfo([@path, 'tag', 'bind', tag], context)
  end

=begin
  def tag_cget(tag, key)
    case key.to_s
    when 'text', 'label', 'show', 'data', 'file'
      tk_call_without_enc(@path, 'tag', 'cget',
                          _get_eval_enc_str(tag), "-#{key}")
    when 'font', 'kanjifont'
      #fnt = tk_tcl2ruby(tk_send('tag', 'cget', tag, "-#{key}"))
      fnt = tk_tcl2ruby(_fromUTF8(tk_send_without_enc('tag','cget',_get_eval_enc_str(tag),'-font')))
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(tag, fnt)
      end
      if key.to_s == 'kanjifont' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(_fromUTF8(tk_call_without_enc(@path,'tag','cget',_get_eval_enc_str(tag),"-#{key}")))
    end
  end

  def tag_configure(tag, key, val=None)
    if key.kind_of?(Hash)
      key = _symbolkey2str(key)
      if ( key['font'] || key['kanjifont'] \
          || key['latinfont'] || key['asciifont'] )
        tagfont_configure(tag, key)
      else
        tk_send_without_enc('tag', 'configure', _get_eval_enc_str(tag),
                            *hash_kv(key, true))
      end

    else
      if  key == 'font' || key == :font ||
          key == 'kanjifont' || key == :kanjifont ||
          key == 'latinfont' || key == :latinfont ||
          key == 'asciifont' || key == :asciifont
        if val == None
          tagfontobj(tag)
        else
          tagfont_configure(tag, {key=>val})
        end
      else
        tk_send_without_enc('tag', 'configure', _get_eval_enc_str(tag),
                            "-#{key}", _get_eval_enc_str(val))
      end
    end
    self
  end

  def tag_configinfo(tag, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'text', 'label', 'show', 'data', 'file'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
          conf[4] = tagfont_configinfo(tag, conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        ret = tk_split_simplelist(_fromUTF8(tk_send('tag','configure',_get_eval_enc_str(tag)))).collect{|conflist|
          conf = tk_split_simplelist(conflist)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show', 'data', 'file'
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
          fontconf[4] = tagfont_configinfo(tag, fontconf[4])
          ret.push(fontconf)
        else
          ret
        end
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'text', 'label', 'show', 'data', 'file'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
          conf[4] = tagfont_configinfo(tag, conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('tag','configure',_get_eval_enc_str(tag),"-#{key}")))
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        tk_split_simplelist(_fromUTF8(tk_send('tag','configure',_get_eval_enc_str(tag)))).each{|conflist|
          conf = tk_split_simplelist(conflist)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show', 'data', 'file'
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
          fontconf[3] = tagfont_configinfo(tag, fontconf[3])
          ret['font'] = fontconf
        end
        ret
      end
    end
  end

  def current_tag_configinfo(tag, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        conf = tag_configinfo(tag, key)
        {conf[0] => conf[4]}
      else
        ret = {}
        tag_configinfo(tag).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      tag_configinfo(tag, key).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end
=end

  def tag_raise(tag, above=None)
    tk_send_without_enc('tag', 'raise', _get_eval_enc_str(tag),
                        _get_eval_enc_str(above))
    self
  end

  def tag_lower(tag, below=None)
    tk_send_without_enc('tag', 'lower', _get_eval_enc_str(tag),
                        _get_eval_enc_str(below))
    self
  end

  def tag_remove(tag, *indices)
    tk_send_without_enc('tag', 'remove', _get_eval_enc_str(tag),
                        *(indices.collect{|idx| _get_eval_enc_str(idx)}))
    self
  end

  def tag_ranges(tag)
    #l = tk_split_simplelist(tk_send_without_enc('tag', 'ranges',
    #                                            _get_eval_enc_str(tag)))
    l = tk_split_simplelist(tk_send_without_enc('tag', 'ranges',
                                                _get_eval_enc_str(tag)),
                            false, true)
    r = []
    while key=l.shift
      r.push [Tk::Text::IndexString.new(key), Tk::Text::IndexString.new(l.shift)]
    end
    r
  end

  def tag_nextrange(tag, first, last=None)
    simplelist(tk_send_without_enc('tag', 'nextrange',
                                   _get_eval_enc_str(tag),
                                   _get_eval_enc_str(first),
                                   _get_eval_enc_str(last))).collect{|idx|
      Tk::Text::IndexString.new(idx)
    }
  end

  def tag_prevrange(tag, first, last=None)
    simplelist(tk_send_without_enc('tag', 'prevrange',
                                   _get_eval_enc_str(tag),
                                   _get_eval_enc_str(first),
                                   _get_eval_enc_str(last))).collect{|idx|
      Tk::Text::IndexString.new(idx)
    }
  end

=begin
  def window_cget(index, slot)
    case slot.to_s
    when 'text', 'label', 'show', 'data', 'file'
      _fromUTF8(tk_send_without_enc('window', 'cget',
                                    _get_eval_enc_str(index), "-#{slot}"))
    when 'font', 'kanjifont'
      #fnt = tk_tcl2ruby(tk_send('window', 'cget', index, "-#{slot}"))
      fnt = tk_tcl2ruby(_fromUTF8(tk_send_without_enc('window', 'cget', _get_eval_enc_str(index), '-font')))
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(index, fnt)
      end
      if slot.to_s == 'kanjifont' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(_fromUTF8(tk_send_without_enc('window', 'cget', _get_eval_enc_str(index), "-#{slot}")))
    end
  end

  def window_configure(index, slot, value=None)
    if index.kind_of?(TkTextWindow)
      index.configure(slot, value)
    else
      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        win = slot['window']
        # slot['window'] = win.epath if win.kind_of?(TkWindow)
        slot['window'] = _epath(win) if win
        if slot['create']
          p_create = slot['create']
          if p_create.kind_of?(Proc)
#=begin
            slot['create'] = install_cmd(proc{
                                           id = p_create.call
                                           if id.kind_of?(TkWindow)
                                             id.epath
                                           else
                                             id
                                           end
                                         })
#=end
            slot['create'] = install_cmd(proc{_epath(p_create.call)})
          end
        end
        tk_send_without_enc('window', 'configure',
                            _get_eval_enc_str(index),
                            *hash_kv(slot, true))
      else
        if slot == 'window' || slot == :window
          # id = value
          # value = id.epath if id.kind_of?(TkWindow)
          value = _epath(value)
        end
        if slot == 'create' || slot == :create
          p_create = value
          if p_create.kind_of?(Proc)
#=begin
            value = install_cmd(proc{
                                  id = p_create.call
                                  if id.kind_of?(TkWindow)
                                    id.epath
                                  else
                                    id
                                  end
                                })
#=end
            value = install_cmd(proc{_epath(p_create.call)})
          end
        end
        tk_send_without_enc('window', 'configure',
                            _get_eval_enc_str(index),
                            "-#{slot}", _get_eval_enc_str(value))
      end
    end
    self
  end

  def window_configinfo(win, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          conf = tk_split_simplelist(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win), "-#{slot}")))
        else
          conf = tk_split_list(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win), "-#{slot}")))
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        tk_split_simplelist(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win)))).collect{|conflist|
          conf = tk_split_simplelist(conflist)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show', 'data', 'file'
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
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          conf = tk_split_simplelist(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win), "-#{slot}")))
        else
          conf = tk_split_list(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win), "-#{slot}")))
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        tk_split_simplelist(_fromUTF8(tk_send('window', 'configure', _get_eval_enc_str(win)))).each{|conflist|
          conf = tk_split_simplelist(conflist)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show', 'data', 'file'
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
        ret
      end
    end
  end

  def current_window_configinfo(win, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        conf = window_configinfo(win, slot)
        {conf[0] => conf[4]}
      else
        ret = {}
        window_configinfo(win).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      window_configinfo(win, slot).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end
=end

  def window_names
    # tk_split_simplelist(_fromUTF8(tk_send_without_enc('window', 'names'))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('window', 'names'), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def _ktext_length(txt)
    if TkCore::WITH_ENCODING ### Ruby 1.9 !!!!!!!!!!!!!
      return txt.length
    end
    ###########################

    if $KCODE !~ /n/i
      return txt.gsub(/[^\Wa-zA-Z_\d]/, ' ').length
    end

    # $KCODE == 'NONE'
    if JAPANIZED_TK
      tk_call_without_enc('kstring', 'length',
                          _get_eval_enc_str(txt)).to_i
    else
      begin
        tk_call_without_enc('encoding', 'convertto', 'ascii',
                            _get_eval_enc_str(txt)).length
      rescue StandardError, NameError
        # sorry, I have no plan
        txt.length
      end
    end
  end
  private :_ktext_length

  def tksearch(*args)
    # call 'search' subcommand of text widget
    #   args ::= [<array_of_opts>] <pattern> <start_index> [<stop_index>]
    # If <pattern> is regexp, then it must be a regular expression of Tcl
    nocase = false
    if args[0].kind_of?(Array)
      opts = args.shift.collect{|opt|
        s_opt = opt.to_s
        nocase = true if s_opt == 'nocase'
        '-' + s_opt
      }
    else
      opts = []
    end

    if args[0].kind_of?(Regexp)
      regexp = args.shift
      if !nocase && (regexp.options & Regexp::IGNORECASE) != 0
        opts << '-nocase'
      end
      args.unshift(regexp.source)
    end

    opts << '--'

    ret = tk_send('search', *(opts + args))
    if ret == ""
      nil
    else
      Tk::Text::IndexString.new(ret)
    end
  end

  def tksearch_with_count(*args)
    # call 'search' subcommand of text widget
    #   args ::= [<array_of_opts>] <var> <pattern> <start_index> [<stop_index>]
    # If <pattern> is regexp, then it must be a regular expression of Tcl
    nocase = false
    if args[0].kind_of?(Array)
      opts = args.shift.collect{|opt|
        s_opt = opt.to_s
        nocase = true if s_opt == 'nocase'
        '-' + s_opt
      }
    else
      opts = []
    end

    opts << '-count' << args.shift

    if args[0].kind_of?(Regexp)
      regexp = args.shift
      if !nocase && (regexp.options & Regexp::IGNORECASE) != 0
        opts << '-nocase'
      end
      args.unshift(regexp.source)
    end

    opts << '--'

    ret = tk_send('search', *(opts + args))
    if ret == ""
      nil
    else
      Tk::Text::IndexString.new(ret)
    end
  end

  def search_with_length(pat,start,stop=None)
    pat = pat.chr if pat.kind_of?(Integer)
    if stop != None
      return ["", 0] if compare(start,'>=',stop)
      txt = get(start,stop)
      if (pos = txt.index(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of?(String)
          #return [index(start + " + #{pos} chars"), pat.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(pat), pat.dup]
        else
          #return [index(start + " + #{pos} chars"), $&.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(match), match]
        end
      else
        return ["", 0]
      end
    else
      txt = get(start,'end - 1 char')
      if (pos = txt.index(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of?(String)
          #return [index(start + " + #{pos} chars"), pat.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(pat), pat.dup]
        else
          #return [index(start + " + #{pos} chars"), $&.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(match), match]
        end
      else
        txt = get('1.0','end - 1 char')
        if (pos = txt.index(pat))
          match = $&
          #pos = txt[0..(pos-1)].split('').length if pos > 0
          pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
          if pat.kind_of?(String)
            #return [index("1.0 + #{pos} chars"), pat.split('').length]
            return [index("1.0 + #{pos} chars"),
                    _ktext_length(pat), pat.dup]
          else
            #return [index("1.0 + #{pos} chars"), $&.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
          end
        else
          return ["", 0]
        end
      end
    end
  end

  def search(pat,start,stop=None)
    search_with_length(pat,start,stop)[0]
  end

  def rsearch_with_length(pat,start,stop=None)
    pat = pat.chr if pat.kind_of?(Integer)
    if stop != None
      return ["", 0] if compare(start,'<=',stop)
      txt = get(stop,start)
      if (pos = txt.rindex(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of?(String)
          #return [index(stop + " + #{pos} chars"), pat.split('').length]
          return [index(stop + " + #{pos} chars"), _ktext_length(pat), pat.dup]
        else
          #return [index(stop + " + #{pos} chars"), $&.split('').length]
          return [index(stop + " + #{pos} chars"), _ktext_length(match), match]
        end
      else
        return ["", 0]
      end
    else
      txt = get('1.0',start)
      if (pos = txt.rindex(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of?(String)
          #return [index("1.0 + #{pos} chars"), pat.split('').length]
          return [index("1.0 + #{pos} chars"), _ktext_length(pat), pat.dup]
        else
          #return [index("1.0 + #{pos} chars"), $&.split('').length]
          return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
        end
      else
        txt = get('1.0','end - 1 char')
        if (pos = txt.rindex(pat))
          match = $&
          #pos = txt[0..(pos-1)].split('').length if pos > 0
          pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
          if pat.kind_of?(String)
            #return [index("1.0 + #{pos} chars"), pat.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(pat), pat.dup]
          else
            #return [index("1.0 + #{pos} chars"), $&.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
          end
        else
          return ["", 0]
        end
      end
    end
  end

  def rsearch(pat,start,stop=None)
    rsearch_with_length(pat,start,stop)[0]
  end

  def dump(type_info, *index, &block)
    if type_info.kind_of?(Symbol)
      type_info = [ type_info.to_s ]
    elsif type_info.kind_of?(String)
      type_info = [ type_info ]
    end
    args = type_info.collect{|inf| '-' + inf}
    args << '-command' << block if block
    str = tk_send('dump', *(args + index))
    result = []
    sel = nil
    i = 0
    while i < str.size
      # retrieve key
      idx = str.index(/ /, i)
      result.push str[i..(idx-1)]
      i = idx + 1

      # retrieve value
      case result[-1]
      when 'text'
        if str[i] == ?{
          # text formed as {...}
          val, i = _retrieve_braced_text(str, i)
          result.push val
        else
          # text which may contain backslahes
          val, i = _retrieve_backslashed_text(str, i)
          result.push val
        end
      else
        idx = str.index(/ /, i)
        val = str[i..(idx-1)]
        case result[-1]
        when 'mark'
          case val
          when 'insert'
            result.push TkTextMarkInsert.new(self)
          when 'current'
            result.push TkTextMarkCurrent.new(self)
          when 'anchor'
            result.push TkTextMarkAnchor.new(self)
          else
            result.push tk_tcl2ruby(val)
          end
        when 'tagon'
          if val == 'sel'
            if sel
              result.push sel
            else
              result.push TkTextTagSel.new(self)
            end
          else
            result.push tk_tcl2ruby(val)
          end
        when 'tagoff'
            result.push tk_tcl2ruby(val)
        when 'window'
          result.push tk_tcl2ruby(val)
        when 'image'
          result.push tk_tcl2ruby(val)
        end
        i = idx + 1
      end

      # retrieve index
      idx = str.index(/ /, i)
      if idx
        result.push(Tk::Text::IndexString.new(str[i..(idx-1)]))
        i = idx + 1
      else
        result.push(Tk::Text::IndexString.new(str[i..-1]))
        break
      end
    end

    kvis = []
    until result.empty?
      kvis.push [result.shift, result.shift, result.shift]
    end
    kvis  # result is [[key1, value1, index1], [key2, value2, index2], ...]
  end

  def _retrieve_braced_text(str, i)
    cnt = 0
    idx = i
    while idx < str.size
      case str[idx]
      when ?{
        cnt += 1
      when ?}
        cnt -= 1
        if cnt == 0
          break
        end
      end
      idx += 1
    end
    return str[i+1..idx-1], idx + 2
  end
  private :_retrieve_braced_text

  def _retrieve_backslashed_text(str, i)
    j = i
    idx = nil
    loop {
      idx = str.index(/ /, j)
      if str[idx-1] == ?\\
        j += 1
      else
        break
      end
    }
    val = str[i..(idx-1)]
    val.gsub!(/\\( |\{|\})/, '\1')
    return val, idx + 1
  end
  private :_retrieve_backslashed_text

  def dump_all(*index, &block)
    dump(['all'], *index, &block)
  end
  def dump_mark(*index, &block)
    dump(['mark'], *index, &block)
  end
  def dump_tag(*index, &block)
    dump(['tag'], *index, &block)
  end
  def dump_text(*index, &block)
    dump(['text'], *index, &block)
  end
  def dump_window(*index, &block)
    dump(['window'], *index, &block)
  end
  def dump_image(*index, &block)
    dump(['image'], *index, &block)
  end
end

#TkText = Tk::Text unless Object.const_defined? :TkText
#Tk.__set_toplevel_aliases__(:Tk, Tk::Text, :TkText)
Tk.__set_loaded_toplevel_aliases__('tk/text.rb', :Tk, Tk::Text, :TkText)


#######################################

class Tk::Text::Peer < Tk::Text
  # Tk8.5 feature
  def initialize(text, parent=nil, keys={})
    unless text.kind_of?(Tk::Text)
      fail ArgumentError, "Tk::Text is expected for 1st argument"
    end
    @src_text = text
    super(parent, keys)
  end

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(@src_text.path, 'peer', 'create',
                          @path, *hash_kv(keys, true))
    else
      tk_call_without_enc(@src_text.path, 'peer', 'create', @path)
    end
  end
  private :create_self
end
