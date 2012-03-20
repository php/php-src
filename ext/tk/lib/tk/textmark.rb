#
# tk/textmark.rb - methods for treating text marks
#
require 'tk'
require 'tk/text'

class TkTextMark<TkObject
  include Tk::Text::IndexModMethods

  TMarkID_TBL = TkCore::INTERP.create_table

  (Tk_TextMark_ID = ['mark'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    TMarkID_TBL.mutex.synchronize{ TMarkID_TBL.clear }
  }

  def TkTextMark.id2obj(text, id)
    tpath = text.path
    TMarkID_TBL.mutex.synchronize{
      if TMarkID_TBL[tpath]
        TMarkID_TBL[tpath][id]? TMarkID_TBL[tpath][id]: id
      else
        id
      end
    }
  end

  def initialize(parent, index)
    #unless parent.kind_of?(Tk::Text)
    #  fail ArgumentError, "expect Tk::Text for 1st argument"
    #end
    @parent = @t = parent
    @tpath = parent.path
    Tk_TextMark_ID.mutex.synchronize{
      # @path = @id = Tk_TextMark_ID.join('')
      @path = @id = Tk_TextMark_ID.join(TkCore::INTERP._ip_id_).freeze
      Tk_TextMark_ID[1].succ!
    }
    TMarkID_TBL.mutex.synchronize{
      TMarkID_TBL[@id] = self
      TMarkID_TBL[@tpath] = {} unless TMarkID_TBL[@tpath]
      TMarkID_TBL[@tpath][@id] = self
    }
    tk_call_without_enc(@t.path, 'mark', 'set', @id,
                        _get_eval_enc_str(index))
    @t._addtag id, self
  end

  def id
    Tk::Text::IndexString.new(@id)
  end

  def exist?
    #if ( tk_split_simplelist(_fromUTF8(tk_call_without_enc(@t.path, 'mark', 'names'))).find{|id| id == @id } )
    if ( tk_split_simplelist(tk_call_without_enc(@t.path, 'mark', 'names'), false, true).find{|id| id == @id } )
      true
    else
      false
    end
  end

=begin
  # move to Tk::Text::IndexModMethods module
  def +(mod)
    return chars(mod) if mod.kind_of?(Numeric)

    mod = mod.to_s
    if mod =~ /^\s*[+-]?\d/
      Tk::Text::IndexString.new(@id + ' + ' + mod)
    else
      Tk::Text::IndexString.new(@id + ' ' + mod)
    end
  end

  def -(mod)
    return chars(-mod) if mod.kind_of?(Numeric)

    mod = mod.to_s
    if mod =~ /^\s*[+-]?\d/
      Tk::Text::IndexString.new(@id + ' - ' + mod)
    elsif mod =~ /^\s*[-]\s+(\d.*)$/
      Tk::Text::IndexString.new(@id + ' - -' + $1)
    else
      Tk::Text::IndexString.new(@id + ' ' + mod)
    end
  end
=end

  def pos
    @t.index(@id)
  end

  def pos=(where)
    set(where)
  end

  def set(where)
    tk_call_without_enc(@t.path, 'mark', 'set', @id,
                        _get_eval_enc_str(where))
    self
  end

  def unset
    tk_call_without_enc(@t.path, 'mark', 'unset', @id)
    self
  end
  alias destroy unset

  def gravity
    tk_call_without_enc(@t.path, 'mark', 'gravity', @id)
  end

  def gravity=(direction)
    tk_call_without_enc(@t.path, 'mark', 'gravity', @id, direction)
    #self
    direction
  end

  def next(index = nil)
    if index
      @t.tagid2obj(_fromUTF8(tk_call_without_enc(@t.path, 'mark', 'next', _get_eval_enc_str(index))))
    else
      @t.tagid2obj(_fromUTF8(tk_call_without_enc(@t.path, 'mark', 'next', @id)))
    end
  end

  def previous(index = nil)
    if index
      @t.tagid2obj(_fromUTF8(tk_call_without_enc(@t.path, 'mark', 'previous', _get_eval_enc_str(index))))
    else
      @t.tagid2obj(_fromUTF8(tk_call_without_enc(@t.path, 'mark', 'previous', @id)))
    end
  end
end
TktMark = TkTextMark

class TkTextNamedMark<TkTextMark
  def self.new(parent, name, index=nil)
    TMarkID_TBL.mutex.synchronize{
      if TMarkID_TBL[parent.path] && TMarkID_TBL[parent.path][name]
        obj = TMarkID_TBL[parent.path][name]
      else
        # super(parent, name, *args)
        (obj = self.allocate).instance_eval{
          @parent = @t = parent
          @tpath = parent.path
          @path = @id = name
          TMarkID_TBL[@id] = self
          TMarkID_TBL[@tpath] = {} unless TMarkID_TBL[@tpath]
          TMarkID_TBL[@tpath][@id] = self unless TMarkID_TBL[@tpath][@id]
          @t._addtag @id, self
        }
        obj
      end

      if obj && index
        tk_call_without_enc(parent.path, 'mark', 'set', name,
                            _get_eval_enc_str(index))
      end
      obj
    }
  end

  def initialize(parent, name, index=nil)
    # dummy:: not called by 'new' method

    #unless parent.kind_of?(Tk::Text)
    #  fail ArgumentError, "expect Tk::Text for 1st argument"
    #end
    @parent = @t = parent
    @tpath = parent.path
    @path = @id = name
    tk_call_without_enc(@t.path, 'mark', 'set', @id,
                        _get_eval_enc_str(index)) if index
    @t._addtag @id, self
  end
end
TktNamedMark = TkTextNamedMark

class TkTextMarkInsert<TkTextNamedMark
  def self.new(parent,*args)
    super(parent, 'insert', *args)
  end
end
TktMarkInsert = TkTextMarkInsert

class TkTextMarkCurrent<TkTextNamedMark
  def self.new(parent,*args)
    super(parent, 'current', *args)
  end
end
TktMarkCurrent = TkTextMarkCurrent

class TkTextMarkAnchor<TkTextNamedMark
  def self.new(parent,*args)
    super(parent, 'anchor', *args)
  end
end
TktMarkAnchor = TkTextMarkAnchor
