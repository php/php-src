#
#  tkextlib/bwidget/listbox.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/canvas'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class ListBox < TkWindow
      # is NOT a subclass of a listbox widget class.
      # because it constructed on a canvas widget.

      class Item < TkObject
      end
    end
  end
end

class Tk::BWidget::ListBox
  include TkItemConfigMethod
  include Scrollable

  TkCommandNames = ['ListBox'.freeze].freeze
  WidgetClassName = 'ListBox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  class Event_for_Items < TkEvent::Event
    def self._get_extra_args_tbl
      [
        TkComm.method(:string)   # item idenfier
      ]
    end
  end

  def __boolval_optkeys
    super() << 'autofocus' << 'dragenabled' << 'dropenabled' << 'selectfill'
  end
  private :__boolval_optkeys

  def tagid(tag)
    if tag.kind_of?(Tk::BWidget::ListBox::Item)
      tag.id
    else
      # tag
      _get_eval_string(tag)
    end
  end

  #def imagebind(*args)
  #  _bind_for_event_class(Event_for_Items, [path, 'bindImage'], *args)
  #  self
  #end
  def imagebind(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_for_event_class(Event_for_Items, [path, 'bindImage'],
                          context, cmd, *args)
    self
  end

  #def imagebind_append(*args)
  #  _bind_append_for_event_class(Event_for_Items, [path, 'bindImage'], *args)
  #  self
  #end
  def imagebind_append(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append_for_event_class(Event_for_Items, [path, 'bindImage'],
                                 context, cmd, *args)
    self
  end

  def imagebind_remove(*args)
    _bind_remove_for_event_class(Event_for_Items, [path, 'bindImage'], *args)
    self
  end

  def imagebindinfo(*args)
    _bindinfo_for_event_class(Event_for_Items, [path, 'bindImage'], *args)
  end

  #def textbind(*args)
  #  _bind_for_event_class(Event_for_Items, [path, 'bindText'], *args)
  #  self
  #end
  def textbind(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_for_event_class(Event_for_Items, [path, 'bindText'],
                          context, cmd, *args)
    self
  end

  #def textbind_append(*args)
  #  _bind_append_for_event_class(Event_for_Items, [path, 'bindText'], *args)
  #  self
  #end
  def textbind_append(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append_for_event_class(Event_for_Items, [path, 'bindText'],
                                 context, cmd, *args)
    self
  end

  def textbind_remove(*args)
    _bind_remove_for_event_class(Event_for_Items, [path, 'bindText'], *args)
    self
  end

  def textbindinfo(*args)
    _bindinfo_for_event_class(Event_for_Items, [path, 'bindText'], *args)
  end

  def delete(*args)
    tk_send('delete', *args)
    self
  end

  def edit(item, text, *args)
    tk_send('edit', tagid(item), text, *args)
    self
  end

  def exist?(item)
    bool(tk_send('exists', tagid(item)))
  end

  def index(item)
    num_or_str(tk_send('index', tagid(item)))
  end

  def insert(idx, item, keys={})
    tk_send('insert', idx, tagid(item), *hash_kv(keys))
    self
  end

  def get_item(idx)
    tk_send('items', idx)
  end

  def items(first=None, last=None)
    list(tk_send('items', first, last))
  end

  def move(item, idx)
    tk_send('move', tagid(item), idx)
    self
  end

  def reorder(neworder)
    tk_send('reorder', neworder)
    self
  end

  def see(item)
    tk_send('see', tagid(item))
    self
  end

  def selection_clear
    tk_send_without_enc('selection', 'clear')
    self
  end

  def selection_set(*args)
    tk_send_without_enc('selection', 'set',
                        *(args.collect{|item| tagid(item)}))
    self
  end

  def selection_add(*args)
    tk_send_without_enc('selection', 'add',
                        *(args.collect{|item| tagid(item)}))
    self
  end

  def selection_remove(*args)
    tk_send_without_enc('selection', 'remove',
                        *(args.collect{|item| tagid(item)}))
    self
  end

  def selection_get(*args)
    simplelist(tk_send_without_enc('selection', 'get')).collect{|item|
      Tk::BWidget::ListBox::Item.id2obj(self, item)
    }
  end
end

class Tk::BWidget::ListBox::Item
  include TkTreatTagFont

  ListItem_TBL = TkCore::INTERP.create_table

  (ListItem_ID = ['bw:item'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    ListItem_TBL.mutex.synchronize{ ListItem_TBL.clear }
  }

  def self.id2obj(lbox, id)
    lpath = lbox.path
    ListItem_TBL.mutex.synchronize{
      if ListItem_TBL[lpath]
        ListItem_TBL[lpath][id]? ListItem_TBL[lpath][id]: id
      else
        id
      end
    }
  end

  def initialize(lbox, *args)
    if lbox.kind_of?(Tk::BWidget::ListBox)
      @listbox = lbox
    else
      fail RuntimeError,
        "expect Tk::BWidget::ListBox or Tk::BWidget::ListBox::Item for 1st argument"
    end

    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
    else
      keys = {}
    end

    index = keys.delete('index')
    unless args.empty?
      index = args.shift
    end
    index = 'end' unless index

    unless args.empty?
      fail RuntimeError, 'too much arguments'
    end

    @lpath = @listbox.path

    if keys.key?('itemname')
      @path = @id = keys.delete('itemname')
    else
      ListItem_ID.mutex.synchronize{
        @path = @id = ListItem_ID.join(TkCore::INTERP._ip_id_)
        ListItem_ID[1].succ!
      }
    end

    ListItem_TBL.mutex.synchronize{
      ListItem_TBL[@id] = self
      ListItem_TBL[@lpath] = {} unless ListItem_TBL[@lpath]
      ListItem_TBL[@lpath][@id] = self
    }

    @listbox.insert(index, @id, keys)
  end

  def listbox
    @listbox
  end

  def id
    @id
  end

  def [](key)
    cget(key)
  end

  def []=(key, val)
    configure(key, val)
    val
  end

  def cget_tkstring(key)
    @listbox.itemcget_tkstring(@id, key)
  end
  def cget(key)
    @listbox.itemcget(@id, key)
  end
  def cget_strict(key)
    @listbox.itemcget_strict(@id, key)
  end

  def configure(key, val=None)
    @listbox.itemconfigure(@id, key, val)
  end

  def configinfo(key=nil)
    @listbox.itemconfiginfo(@id, key)
  end

  def current_configinfo(key=nil)
    @listbox.current_itemconfiginfo(@id, key)
  end

  def delete
    @listbox.delete(@id)
    self
  end

  def edit(*args)
    @listbox.edit(@id, *args)
    self
  end

  def exist?
    @listbox.exist?(@id)
  end

  def index
    @listbox.index(@id)
  end

  def move(index)
    @listbox.move(@id, index)
  end

  def see
    @listbox.see(@id)
  end

  def selection_add
    @listbox.selection_add(@id)
  end

  def selection_remove
    @listbox.selection_remove(@id)
  end

  def selection_set
    @listbox.selection_set(@id)
  end

  def selection_toggle
    @listbox.selection_toggle(@id)
  end
end

