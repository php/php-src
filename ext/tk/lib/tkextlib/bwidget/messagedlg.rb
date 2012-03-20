#
#  tkextlib/bwidget/messagedlg.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/dialog.rb'

module Tk
  module BWidget
    class MessageDlg < TkWindow
    end
  end
end

class Tk::BWidget::MessageDlg
  TkCommandNames = ['MessageDlg'.freeze].freeze
  WidgetClassName = 'MessageDlg'.freeze
  WidgetClassNames[WidgetClassName] ||= self

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
    # NOT create widget.
    # Because the widget no longer exist when returning from creation.
    @keys = _symbolkey2str(keys).update('parent'=>@relative)
    @info = nil
  end
  private :create_self

  def __strval_optkeys
    super() << 'message' << 'title'
  end
  private :__strval_optkeys

  def __listval_optkeys
    super() << 'buttons'
  end
  private :__listval_optkeys

  def cget(slot)
    slot = slot.to_s
    if slot == 'relative'
      slot = 'parent'
    end
    if winfo_exist?
      val = super(slot)
      @keys[slot] = val
    end
    @keys[slot]
  end
  def cget_strict(slot)
    slot = slot.to_s
    if slot == 'relative'
      slot = 'parent'
    end
    if winfo_exist?
      val = super(slot)
      @keys[slot] = val
    end
    @keys[slot]
  end

  def configure(slot, value=None)
    if winfo_exist?
      super(slot, value)
    end
    if slot.kind_of?(Hash)
      slot = _symbolkey2str(slot)
      slot['parent'] = slot.delete('relative') if slot.key?('relative')
      @keys.update(slot)

      if @info
        # update @info
        slot.each{|k, v|
          if TkComm::GET_CONFIGINFO_AS_ARRAY
            if (inf = @info.assoc(k))
              inf[-1] = v
            else
              @info << [k, '', '', '', v]
            end
          else
            if (inf = @info[k])
              inf[-1] = v
            else
              @info[k] = ['', '', '', v]
            end
          end
        }
      end

    else # ! Hash
      slot = slot.to_s
      slot = 'parent' if slot == 'relative'
      @keys[slot] = value

      if @info
        # update @info
        if TkComm::GET_CONFIGINFO_AS_ARRAY
          if (inf = @info.assoc(slot))
            inf[-1] = value
          else
            @info << [slot, '', '', '', value]
          end
        else
          if (inf = @info[slot])
            inf[-1] = value
          else
            @info[slot] = ['', '', '', value]
          end
        end
      end
    end

    self
  end

  def configinfo(slot=nil)
    if winfo_exist?
      @info = super()
      if TkComm::GET_CONFIGINFO_AS_ARRAY
        @info << ['relative', 'parent']
      else
        @info['relative'] = 'parent'
      end
    end

    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if @info
        if winfo_exist?
          # update @keys
          @info.each{|inf| @keys[inf[0]] = inf[-1] if inf.size > 2 }
        end
      else
        @info = []
        @keys.each{|k, v|
          @info << [k, '', '', '', v]
        }
        @info << ['relative', 'parent']
      end

      if slot
        @info.asoc(slot.to_s).dup
      else
        @info.dup
      end

    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if @info
        if winfo_exist?
          # update @keys
          @info.each{|k, inf| @keys[k] = inf[-1] if inf.size > 2 }
        end
      else
        @info = {}
        @keys.each{|k, v|
          @info[k] = ['', '', '', v]
        }
        @info['relative'] = 'parent'
      end

      if slot
        @info[slot.to_s].dup
      else
        @info.dup
      end
    end
  end

  def create
    # return the index of the pressed button, or nil if it is destroyed
    ret = num_or_str(tk_call(self.class::TkCommandNames[0],
                             @path, *hash_kv(@keys)))
    (ret < 0)? nil: ret
  end
end
