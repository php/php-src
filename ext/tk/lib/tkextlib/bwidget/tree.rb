#
#  tkextlib/bwidget/tree.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/canvas'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class Tree < TkWindow
      class Node < TkObject
      end
    end
  end
end

class Tk::BWidget::Tree
  include TkItemConfigMethod
  include Scrollable

  TkCommandNames = ['Tree'.freeze].freeze
  WidgetClassName = 'Tree'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  class Event_for_Items < TkEvent::Event
    def self._get_extra_args_tbl
      [
        TkComm.method(:string)   # item idenfier
      ]
    end
  end

  def __strval_optkeys
    super() << 'crossfill' << 'linesfill'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'dragenabled' << 'dropenabled' <<
      'redraw' << 'selectfill' << 'showlines'
  end
  private :__boolval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys

  def tagid(tag)
    if tag.kind_of?(Tk::BWidget::Tree::Node)
      tag.id
    else
      # tag
      _get_eval_string(tag)
    end
  end

  def areabind(context, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_for_event_class(Event_for_Items, [path, 'bindArea'],
                          context, cmd, *args)
    self
  end

  def areabind_append(context, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append_for_event_class(Event_for_Items, [path, 'bindArea'],
                                 context, cmd, *args)
    self
  end

  def areabind_remove(*args)
    _bind_remove_for_event_class(Event_for_Items, [path, 'bindArea'], *args)
    self
  end

  def areabindinfo(*args)
    _bindinfo_for_event_class(Event_for_Items, [path, 'bindArea'], *args)
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

  def close_tree(node, recurse=None)
    tk_send('closetree', tagid(node), recurse)
    self
  end

  def delete(*args)
    tk_send('delete', *(args.collect{|node| tagid(node)}))
    self
  end

  def edit(node, text, *args)
    tk_send('edit', tagid(node), text, *args)
    self
  end

  def exist?(node)
    bool(tk_send('exists', tagid(node)))
  end

  def find(findinfo, confine=None)
    Tk::BWidget::Tree::Node.id2obj(self, tk_send(findinfo, confine))
  end
  def find_position(x, y, confine=None)
    self.find(_at(x,y), confine)
  end
  def find_line(linenum)
    self.find(linenum)
  end

  def index(node)
    num_or_str(tk_send('index', tagid(node)))
  end

  def insert(idx, parent, node, keys={})
    tk_send('insert', idx, tagid(parent), tagid(node), *hash_kv(keys))
    self
  end

  def line(node)
    number(tk_send('line', tagid(node)))
  end

  def move(parent, node, idx)
    tk_send('move', tagid(parent), tagid(node), idx)
    self
  end

  def get_node(node, idx)
    Tk::BWidget::Tree::Node.id2obj(self, tk_send('nodes', tagid(node), idx))
  end

  def nodes(node, first=None, last=None)
    simplelist(tk_send('nodes', tagid(node), first, last)).collect{|node|
      Tk::BWidget::Tree::Node.id2obj(self, node)
    }
  end

  def open?(node)
    bool(self.itemcget(tagid(node), 'open'))
  end

  def open_tree(node, recurse=None)
    tk_send('opentree', tagid(node), recurse)
    self
  end

  def parent(node)
    Tk::BWidget::Tree::Node.id2obj(self, tk_send('parent', tagid(node)))
  end

  def reorder(node, neworder)
    tk_send('reorder', tagid(node), neworder)
    self
  end

  def see(node)
    tk_send('see', tagid(node))
    self
  end

  def selection_add(*args)
    tk_send_without_enc('selection', 'add',
                        *(args.collect{|node| tagid(node)}))
    self
  end

  def selection_clear
    tk_send_without_enc('selection', 'clear')
    self
  end

  def selection_get
    list(tk_send_without_enc('selection', 'get'))
  end

  def selection_include?(*args)
    bool(tk_send_without_enc('selection', 'get',
                             *(args.collect{|node| tagid(node)})))
  end

  def selection_range(*args)
    tk_send_without_enc('selection', 'range',
                        *(args.collect{|node| tagid(node)}))
    self
  end

  def selection_remove(*args)
    tk_send_without_enc('selection', 'remove',
                        *(args.collect{|node| tagid(node)}))
    self
  end

  def selection_set(*args)
    tk_send_without_enc('selection', 'set',
                        *(args.collect{|node| tagid(node)}))
    self
  end

  def selection_toggle(*args)
    tk_send_without_enc('selection', 'toggle',
                        *(args.collect{|node| tagid(node)}))
    self
  end

  def toggle(node)
    tk_send_without_enc('toggle', tagid(node))
    self
  end

  def visible(node)
    bool(tk_send_without_enc('visible', tagid(node)))
  end
end

class Tk::BWidget::Tree::Node
  include TkTreatTagFont

  TreeNode_TBL = TkCore::INTERP.create_table

  (TreeNode_ID = ['bw:node'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    TreeNode_TBL.mutex.synchronize{ TreeNode_TBL.clear }
  }

  def self.id2obj(tree, id)
    tpath = tree.path
    TreeNode_TBL.mutex.synchronize{
      if TreeNode_TBL[tpath]
        TreeNode_TBL[tpath][id]? TreeNode_TBL[tpath][id]: id
      else
        id
      end
    }
  end

  def initialize(tree, *args)
    if tree.kind_of?(Tk::BWidget::Tree)
      @tree = tree
      parent = args.shift
      if parent.kind_of?(Tk::BWidget::Tree::Node)
        if parent.tree.path != @tree.path
          fail RuntimeError, 'tree of parent node is not match'
        end
      end
    elsif tree.kind_of?(Tk::BWidget::Tree::Node)
      @tree = tree.tree
      parent = tree.parent
    else
      fail RuntimeError,
        "expect Tk::BWidget::Tree or Tk::BWidget::Tree::Node for 1st argument"
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

    @tpath = @tree.path

    if keys.key?('nodename')
      @path = @id = keys.delete('nodename')
    else
      TreeNode_ID.mutex.synchronize{
        @path = @id = TreeNode_ID.join(TkCore::INTERP._ip_id_)
        TreeNode_ID[1].succ!
      }
    end

    TreeNode_TBL.mutex.synchronize{
      TreeNode_TBL[@id] = self
      TreeNode_TBL[@tpath] = {} unless TreeNode_TBL[@tpath]
      TreeNode_TBL[@tpath][@id] = self
    }

    @tree.insert(index, parent, @id, keys)
  end

  def tree
    @tree
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
    @tree.itemcget_tkstring(@id, key)
  end
  def cget(key)
    @tree.itemcget(@id, key)
  end
  def cget_strict(key)
    @tree.itemcget_strict(@id, key)
  end

  def configure(key, val=None)
    @tree.itemconfigure(@id, key, val)
  end

  def configinfo(key=nil)
    @tree.itemconfiginfo(@id, key)
  end

  def current_configinfo(key=nil)
    @tree.current_itemconfiginfo(@id, key)
  end

  def close_tree(recurse=None)
    @tree.close_tree(@id, recurse)
    self
  end

  def delete
    @tree.delete(@id)
    self
  end

  def edit(*args)
    @tree.edit(@id, *args)
    self
  end

  def exist?
    @tree.exist?(@id)
  end

  def index
    @tree.index(@id)
  end

  def move(index, parent=nil)
    if parent
      @tree.move(parent, @id, index)
    else
      @tree.move(self.parent, @id, index)
    end
  end

  def open_tree(recurse=None)
    @tree.open_tree(@id, recurse)
    self
  end

  def open?
    bool(@tree.itemcget(@id, 'open'))
  end

  def parent
    @tree.parent(@id)
  end

  def reorder(neworder)
    @tree.reorder(@id, neworder)
  end

  def see
    @tree.see(@id)
  end

  def selection_add
    @tree.selection_add(@id)
  end

  def selection_remove
    @tree.selection_remove(@id)
  end

  def selection_set
    @tree.selection_set(@id)
  end

  def selection_toggle
    @tree.selection_toggle(@id)
  end

  def toggle
    @tree.toggle(@id)
  end

  def visible
    @tree.visible(@id)
  end
end
