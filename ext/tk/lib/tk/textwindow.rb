#
# tk/textwindow.rb - treat Tk text window object
#
require 'tk'
require 'tk/text'

class TkTextWindow<TkObject
  include Tk::Text::IndexModMethods

  def initialize(parent, index, keys = {})
    #unless parent.kind_of?(Tk::Text)
    #  fail ArgumentError, "expect Tk::Text for 1st argument"
    #end
    @t = parent
    if index == 'end' || index == :end
      @path = TkTextMark.new(@t, tk_call_without_enc(@t.path, 'index',
                                                     'end - 1 chars'))
    elsif index.kind_of?(TkTextMark)
      if tk_call_without_enc(@t.path,'index',index.path) == tk_call_without_enc(@t.path,'index','end')
        @path = TkTextMark.new(@t, tk_call_without_enc(@t.path, 'index',
                                                       'end - 1 chars'))
      else
        @path = TkTextMark.new(@t, tk_call_without_enc(@t.path, 'index',
                                                       index.path))
      end
    else
      @path = TkTextMark.new(@t, tk_call_without_enc(@t.path, 'index', _get_eval_enc_str(index)))
    end
    @path.gravity = 'left'
    @index = @path.path
    keys = _symbolkey2str(keys)
    @id = keys['window']
    # keys['window'] = @id.epath if @id.kind_of?(TkWindow)
    keys['window'] = _epath(@id) if @id
    if keys['create']
      @p_create = keys['create']
      # if @p_create.kind_of?(Proc)
      if TkComm._callback_entry?(@p_create)
=begin
        keys['create'] = install_cmd(proc{
                                       @id = @p_create.call
                                       if @id.kind_of?(TkWindow)
                                         @id.epath
                                       else
                                         @id
                                       end
                                     })
=end
        keys['create'] = install_cmd(proc{@id = @p_create.call; _epath(@id)})
      end
    end
    tk_call_without_enc(@t.path, 'window', 'create', @index,
                        *hash_kv(keys, true))
    @path.gravity = 'right'
  end

  def id
    Tk::Text::IndexString.new(_epath(@id))
  end
  def mark
    @path
  end

  def [](slot)
    cget(slot)
  end
  def []=(slot, value)
    configure(slot, value)
    value
  end

  def cget(slot)
    @t.window_cget(@index, slot)
  end
  def cget_strict(slot)
    @t.window_cget_strict(@index, slot)
  end

  def configure(slot, value=None)
    if slot.kind_of?(Hash)
      slot = _symbolkey2str(slot)
      if slot['window']
        @id = slot['window']
        # slot['window'] = @id.epath if @id.kind_of?(TkWindow)
        slot['window'] = _epath(@id) if @id
      end
      if slot['create']
        self.create=slot.delete('create')
      end
      if slot.size > 0
        tk_call_without_enc(@t.path, 'window', 'configure', @index,
                            *hash_kv(slot, true))
      end
    else
      if slot == 'window' || slot == :window
        @id = value
        # value = @id.epath if @id.kind_of?(TkWindow)
        value = _epath(@id) if @id
      end
      if slot == 'create' || slot == :create
        self.create=value
      else
        tk_call_without_enc(@t.path, 'window', 'configure', @index,
                            "-#{slot}", _get_eval_enc_str(value))
      end
    end
    self
  end

  def configinfo(slot = nil)
    @t.window_configinfo(@index, slot)
  end

  def current_configinfo(slot = nil)
    @t.current_window_configinfo(@index, slot)
  end

  def window
    @id
  end

  def window=(value)
    @id = value
    # value = @id.epath if @id.kind_of?(TkWindow)
    value = _epath(@id) if @id
    tk_call_without_enc(@t.path, 'window', 'configure', @index,
                        '-window', _get_eval_enc_str(value))
    value
  end

  def create
    @p_create
  end

  def create=(value)
    @p_create = value
    # if @p_create.kind_of?(Proc)
    if TkComm._callback_entry?(@p_create)
      value = install_cmd(proc{
                            @id = @p_create.call
                            if @id.kind_of?(TkWindow)
                              @id.epath
                            else
                              @id
                            end
                          })
    end
    tk_call_without_enc(@t.path, 'window', 'configure', @index,
                        '-create', _get_eval_enc_str(value))
    value
  end
end

TktWindow = TkTextWindow
