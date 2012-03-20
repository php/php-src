#
#  tkextlib/bwidget/dialog.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/frame'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/buttonbox'

module Tk
  module BWidget
    class Dialog < TkWindow
    end
  end
end

class Tk::BWidget::Dialog
  TkCommandNames = ['Dialog'.freeze].freeze
  WidgetClassName = 'Dialog'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  include TkItemConfigMethod

  def __numstrval_optkeys
    super() << 'buttonwidth'
  end
  private :__numstrval_optkeys

  def __strval_optkeys
    super() << 'title' << 'geometry'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'transient' << 'homogeneous'
  end
  private :__boolval_optkeys

  def initialize(parent=nil, keys=nil)
    @relative = ''
    if parent.kind_of?(Hash)
      keys = _symbolkey2str(parent)
      @relative = keys['parent'] if keys.key?('parent')
      @relative = keys.delete('relative') if keys.key?('relative')
      super(keys)
    elsif keys
      keys = _symbolkey2str(keys)
      @relative = keys.delete('parent') if keys.key?('parent')
      @relative = keys.delete('relative') if keys.key?('relative')
      super(parent, keys)
    else
      super(parent)
    end
  end

  def create_self(keys)
    cmd = self.class::TkCommandNames[0]
    if keys and keys != None
      tk_call_without_enc(cmd, @path, '-parent', @relative,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(cmd, @path, '-parent', @relative)
    end
  end

  def cget_tkstring(slot)
    if slot.to_s == 'relative'
      super('parent')
    else
      super(slot)
    end
  end
  def cget_strict(slot)
    if slot.to_s == 'relative'
      super('parent')
    else
      super(slot)
    end
  end
  def cget(slot)
    if slot.to_s == 'relative'
      super('parent')
    else
      super(slot)
    end
  end

  def configure(slot, value=None)
    if slot.kind_of?(Hash)
      slot = _symbolkey2str(slot)
      slot['parent'] = slot.delete('relative') if slot.key?('relative')
      super(slot)
    else
      if slot.to_s == 'relative'
        super('parent', value)
      else
        super(slot, value)
      end
    end
  end

  def configinfo(slot=nil)
    if slot
      if slot.to_s == 'relative'
        super('parent')
      else
        super(slot)
      end
    else
      ret = super()
      if TkComm::GET_CONFIGINFO_AS_ARRAY
        ret << ['relative', 'parent']
      else
        ret['relative'] = 'parent'
      end
    end
  end

  def tagid(tagOrId)
    if tagOrId.kind_of?(Tk::BWidget::Button)
      name = tagOrId[:name]
      return index(name) unless name.empty?
    end
    if tagOrId.kind_of?(Tk::Button)
      return index(tagOrId[:text])
    end
    # index(tagOrId.to_s)
    index(_get_eval_string(tagOrId))
  end

  def add(keys={}, &b)
    win = window(tk_send('add', *hash_kv(keys)))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_frame(&b)
    win = window(tk_send('getframe'))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_buttonbox(&b)
    win = window(@path + '.bbox')
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def draw(focus_win=None)
    tk_send('draw', focus_win)
  end

  def enddialog(ret)
    tk_send('enddialog', ret)
  end

  def index(idx)
    get_buttonbox.index(idx)
  end

  def invoke(idx)
    tk_send('invoke', tagid(idx))
    self
  end

  def set_focus(idx)
    tk_send('setfocus', tagid(idx))
    self
  end

  def withdraw
    tk_send('withdraw')
    self
  end
end
