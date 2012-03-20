#
# tk/texttag.rb - methods for treating text tags
#
require 'tk'
require 'tk/text'
require 'tk/tagfont'

class TkTextTag<TkObject
  include TkTreatTagFont
  include Tk::Text::IndexModMethods

  TTagID_TBL = TkCore::INTERP.create_table

  (Tk_TextTag_ID = ['tag'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    TTagID_TBL.mutex.synchronize{ TTagID_TBL.clear }
  }

  def TkTextTag.id2obj(text, id)
    tpath = text.path
    TTagID_TBL.mutex.synchronize{
      if TTagID_TBL[tpath]
        TTagID_TBL[tpath][id]? TTagID_TBL[tpath][id]: id
      else
        id
      end
    }
  end

  def initialize(parent, *args)
    #unless parent.kind_of?(TkText)
    #  fail ArgumentError, "expect TkText for 1st argument"
    #end
    @parent = @t = parent
    @tpath = parent.path
    Tk_TextTag_ID.mutex.synchronize{
      # @path = @id = Tk_TextTag_ID.join('')
      @path = @id = Tk_TextTag_ID.join(TkCore::INTERP._ip_id_).freeze
      Tk_TextTag_ID[1].succ!
    }
    TTagID_TBL.mutex.synchronize{
      TTagID_TBL[@id] = self
      TTagID_TBL[@tpath] = {} unless TTagID_TBL[@tpath]
      TTagID_TBL[@tpath][@id] = self
    }
    #tk_call @t.path, "tag", "configure", @id, *hash_kv(keys)
    if args != []
      keys = args.pop
      if keys.kind_of?(Hash)
        add(*args) if args != []
        configure(keys)
      else
        args.push keys
        add(*args)
      end
    end
    @t._addtag id, self
  end

  def id
    Tk::Text::IndexString.new(@id)
  end

  def exist?
    #if ( tk_split_simplelist(_fromUTF8(tk_call_without_enc(@t.path, 'tag', 'names'))).find{|id| id == @id } )
    if ( tk_split_simplelist(tk_call_without_enc(@t.path, 'tag', 'names'), false, true).find{|id| id == @id } )
      true
    else
      false
    end
  end

  def first
    Tk::Text::IndexString.new(@id + '.first')
  end

  def last
    Tk::Text::IndexString.new(@id + '.last')
  end

  def add(*indices)
    tk_call_without_enc(@t.path, 'tag', 'add', @id,
                        *(indices.collect{|idx| _get_eval_enc_str(idx)}))
    self
  end

  def remove(*indices)
    tk_call_without_enc(@t.path, 'tag', 'remove', @id,
                        *(indices.collect{|idx| _get_eval_enc_str(idx)}))
    self
  end

  def ranges
    l = tk_split_simplelist(tk_call_without_enc(@t.path, 'tag', 'ranges', @id))
    r = []
    while key=l.shift
      r.push [Tk::Text::IndexString.new(key), Tk::Text::IndexString.new(l.shift)]
    end
    r
  end

  def nextrange(first, last=None)
    simplelist(tk_call_without_enc(@t.path, 'tag', 'nextrange', @id,
                                   _get_eval_enc_str(first),
                                   _get_eval_enc_str(last))).collect{|idx|
      Tk::Text::IndexString.new(idx)
    }
  end

  def prevrange(first, last=None)
    simplelist(tk_call_without_enc(@t.path, 'tag', 'prevrange', @id,
                                   _get_eval_enc_str(first),
                                   _get_eval_enc_str(last))).collect{|idx|
      Tk::Text::IndexString.new(idx)
    }
  end

  def [](key)
    cget key
  end

  def []=(key,val)
    configure key, val
    val
  end

  def cget_tkstring(key)
    @t.tag_cget_tkstring @id, key
  end
  def cget(key)
    @t.tag_cget @id, key
  end
  def cget_strict(key)
    @t.tag_cget_strict @id, key
  end
=begin
  def cget(key)
    case key.to_s
    when 'text', 'label', 'show', 'data', 'file'
      _fromUTF8(tk_call_without_enc(@t.path, 'tag', 'cget', @id, "-#{key}"))
    when 'font', 'kanjifont'
      #fnt = tk_tcl2ruby(tk_call(@t.path, 'tag', 'cget', @id, "-#{key}"))
      fnt = tk_tcl2ruby(_fromUTF8(tk_call_without_enc(@t.path, 'tag', 'cget',
                                                      @id, '-font')))
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(@id, fnt)
      end
      if key.to_s == 'kanjifont' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(_fromUTF8(tk_call_without_enc(@t.path, 'tag', 'cget',
                                                @id, "-#{key}")))
    end
  end
=end

  def configure(key, val=None)
    @t.tag_configure @id, key, val
  end
#  def configure(key, val=None)
#    if key.kind_of?(Hash)
#      tk_call @t.path, 'tag', 'configure', @id, *hash_kv(key)
#    else
#      tk_call @t.path, 'tag', 'configure', @id, "-#{key}", val
#    end
#  end
#  def configure(key, value)
#    if value == FALSE
#      value = "0"
#    elsif value.kind_of?(Proc)
#      value = install_cmd(value)
#    end
#    tk_call @t.path, 'tag', 'configure', @id, "-#{key}", value
#  end

  def configinfo(key=nil)
    @t.tag_configinfo @id, key
  end

  def current_configinfo(key=nil)
    @t.current_tag_configinfo @id, key
  end

  #def bind(seq, cmd=Proc.new, *args)
  #  _bind([@t.path, 'tag', 'bind', @id], seq, cmd, *args)
  #  self
  #end
  def bind(seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([@t.path, 'tag', 'bind', @id], seq, cmd, *args)
    self
  end

  #def bind_append(seq, cmd=Proc.new, *args)
  #  _bind_append([@t.path, 'tag', 'bind', @id], seq, cmd, *args)
  #  self
  #end
  def bind_append(seq, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([@t.path, 'tag', 'bind', @id], seq, cmd, *args)
    self
  end

  def bind_remove(seq)
    _bind_remove([@t.path, 'tag', 'bind', @id], seq)
    self
  end

  def bindinfo(context=nil)
    _bindinfo([@t.path, 'tag', 'bind', @id], context)
  end

  def raise(above=None)
    tk_call_without_enc(@t.path, 'tag', 'raise', @id,
                        _get_eval_enc_str(above))
    self
  end

  def lower(below=None)
    tk_call_without_enc(@t.path, 'tag', 'lower', @id,
                        _get_eval_enc_str(below))
    self
  end

  def destroy
    tk_call_without_enc(@t.path, 'tag', 'delete', @id)
    TTagID_TBL.mutex.synchronize{
      TTagID_TBL[@tpath].delete(@id) if TTagID_TBL[@tpath]
    }
    self
  end
end
TktTag = TkTextTag

class TkTextNamedTag<TkTextTag
  def self.new(parent, name, *args)
    tagobj = nil
    TTagID_TBL.mutex.synchronize{
      if TTagID_TBL[parent.path] && TTagID_TBL[parent.path][name]
        tagobj = TTagID_TBL[parent.path][name]
      else
        # super(parent, name, *args)
        (tagobj = self.allocate).instance_eval{
          @parent = @t = parent
          @tpath = parent.path
          @path = @id = name
          TTagID_TBL[@id] = self
          TTagID_TBL[@tpath] = {} unless TTagID_TBL[@tpath]
          TTagID_TBL[@tpath][@id] = self unless TTagID_TBL[@tpath][@id]
          @t._addtag @id, self
        }
      end
    }

    if args != []
      keys = args.pop
      if keys.kind_of?(Hash)
        tagobj.add(*args) if args != []
        tagobj.configure(keys)
      else
        args.push keys
        tagobj.add(*args)
      end
    end

    tagobj
  end

  def initialize(parent, name, *args)
    # dummy:: not called by 'new' method

    #unless parent.kind_of?(Tk::Text)
    #  fail ArgumentError, "expect Tk::Text for 1st argument"
    #end
    @parent = @t = parent
    @tpath = parent.path
    @path = @id = name

    #if mode
    #  tk_call @t.path, "addtag", @id, *args
    #end
    if args != []
      keys = args.pop
      if keys.kind_of?(Hash)
        add(*args) if args != []
        configure(keys)
      else
        args.push keys
        add(*args)
      end
    end
    @t._addtag @id, self
  end
end
TktNamedTag = TkTextNamedTag

class TkTextTagSel<TkTextNamedTag
  def self.new(parent, *args)
    super(parent, 'sel', *args)
  end
end
TktTagSel = TkTextTagSel
