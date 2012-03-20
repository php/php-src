#
#  tkextlib/blt/treeview.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'
require 'tk/validation.rb'

module Tk::BLT
  class Treeview < TkWindow
    module ConfigMethod
    end

    module TagOrID_Methods
    end

    class Node < TkObject
    end

    class Tag < TkObject
    end
  end

  class Hiertable < Treeview
  end
end

######################################

module Tk::BLT::Treeview::ConfigMethod
  include TkItemConfigMethod

  def __item_boolval_optkeys(id)
    case id
    when Array
      # id := [ 'column', name ]
      ['edit', 'hide']
    when 'sort'
      ['decreasing']
    else
      []
    end
  end
  private :__item_boolval_optkeys

  def __item_strval_optkeys(id)
    case id
    when Array
      # id := [ 'column', name ]
      super() << 'titleforeground' << 'titleshadow'
    when 'sort'
      ['decreasing']
    else
      []
    end
  end
  private :__item_strval_optkeys

  def __item_listval_optkeys(id)
    case id
    when 'entry'
      ['bindtags']
    else
      []
    end
  end
  private :__item_listval_optkeys

  def __item_cget_cmd(id)
    if id.kind_of?(Array)
      # id := [ type, name ]
      [self.path, id[0], 'cget', id[1]]
    else
      [self.path, id, 'cget']
    end
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    if id.kind_of?(Array)
      # id := [ type, name ]
      [self.path, id[0], 'configure', id[1]]
    else
      [self.path, id, 'configure']
    end
  end
  private :__item_config_cmd

  def __item_pathname(id)
    if id.kind_of?(Array)
      id = tagid(id[1])
    end
    [self.path, id].join(';')
  end
  private :__item_pathname

  def column_cget_tkstring(name, option)
    itemcget_tkstring(['column', name], option)
  end
  def column_cget(name, option)
    itemcget(['column', name], option)
  end
  def column_cget_strict(name, option)
    itemcget_strict(['column', name], option)
  end
  def column_configure(name, slot, value=None)
    itemconfigure(['column', name], slot, value)
  end
  def column_configinfo(name, slot=nil)
    itemconfiginfo(['column', name], slot)
  end
  def current_column_configinfo(name, slot=nil)
    current_itemconfiginfo(['column', name], slot)
  end

  def button_cget_tkstring(option)
    itemcget_tkstring('button', option)
  end
  def button_cget(option)
    itemcget('button', option)
  end
  def button_cget_strict(option)
    itemcget_strict('button', option)
  end
  def button_configure(slot, value=None)
    itemconfigure('button', slot, value)
  end
  def button_configinfo(slot=nil)
    itemconfiginfo('button', slot)
  end
  def current_button_configinfo(slot=nil)
    current_itemconfiginfo('button', slot)
  end

  def entry_cget_tkstring(option)
    itemcget_tkstring('entry', option)
  end
  def entry_cget(option)
    ret = itemcget('entry', option)
    if option == 'bindtags' || option == :bindtags
      ret.collect{|tag| TkBindTag.id2obj(tag)}
    else
      ret
    end
  end
  def entry_cget_strict(option)
    ret = itemcget_strict('entry', option)
    if option == 'bindtags' || option == :bindtags
      ret.collect{|tag| TkBindTag.id2obj(tag)}
    else
      ret
    end
  end
  def entry_configure(slot, value=None)
    itemconfigure('entry', slot, value)
  end
  def entry_configinfo(slot=nil)
    ret = itemconfiginfo('entry', slot)

    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        if slot == 'bindtags' || slot == :bindtags
          ret[-2] = ret[-2].collect{|tag| TkBindTag.id2obj(tag)}
          ret[-1] = ret[-1].collect{|tag| TkBindTag.id2obj(tag)}
        end
      else
        inf = ret.assoc('bindtags')
        inf[-2] = inf[-2].collect{|tag| TkBindTag.id2obj(tag)}
        inf[-1] = inf[-1].collect{|tag| TkBindTag.id2obj(tag)}
      end

    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if (inf = ret['bindtags'])
        inf[-2] = inf[-2].collect{|tag| TkBindTag.id2obj(tag)}
        inf[-1] = inf[-1].collect{|tag| TkBindTag.id2obj(tag)}
        ret['bindtags'] = inf
      end
    end

    ret
  end
  def current_entry_configinfo(slot=nil)
    ret = current_itemconfiginfo('entry', slot)

    if (val = ret['bindtags'])
      ret['bindtags'] = val.collect{|tag| TkBindTag.id2obj(tag)}
    end

    ret
  end

  def sort_cget_tkstring(option)
    itemcget_tkstring('sort', option)
  end
  def sort_cget(option)
    itemcget('sort', option)
  end
  def sort_cget_strict(option)
    itemcget_strict('sort', option)
  end
  def sort_configure(slot, value=None)
    itemconfigure('sort', slot, value)
  end
  def sort_configinfo(slot=nil)
    itemconfiginfo('sort', slot)
  end
  def current_sort_configinfo(slot=nil)
    current_itemconfiginfo('sort', slot)
  end

  def text_cget_tkstring(option)
    itemcget_tkstring('text', option)
  end
  def text_cget(option)
    itemcget('text', option)
  end
  def text_cget_strict(option)
    itemcget_strict('text', option)
  end
  def text_configure(slot, value=None)
    itemconfigure('text', slot, value)
  end
  def text_configinfo(slot=nil)
    itemconfiginfo('text', slot)
  end
  def current_text_configinfo(slot=nil)
    current_itemconfiginfo('text', slot)
  end

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo
end

class Tk::BLT::Treeview
  TkCommandNames = ['::blt::treeview'.freeze].freeze
  WidgetClassName = 'TreeView'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  include Scrollable
  include ValidateConfigure
  include ItemValidateConfigure
  include Tk::BLT::Treeview::ConfigMethod

  ########################

  def __boolval_optkeys
    ['autocreate', 'allowduplicates', 'exportselection', 'flat', 'hideroot',
      'newtags', 'showtitles', 'sortselection']
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() + ['focusforeground', 'linecolor', 'separator', 'trim']
  end
  private :__strval_optkeys

  ########################

  class OpenCloseCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?W, ?w, :widget ],
        [ ?p, ?s, :name ],
        [ ?P, ?s, :fullpath ],
        [ ?#, ?x, :node_id ],
        nil
      ]

      PROC_TBL = [
        [ ?x, TkComm.method(:num_or_str) ],
        [ ?s, TkComm.method(:string) ],
        [ ?w, TkComm.method(:window) ],
        nil
      ]

=begin
      # for Ruby m17n :: ?x --> String --> char-code ( getbyte(0) )
      KEY_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
          inf[1] = inf[1].getbyte(0) if inf[1].kind_of?(String)
        end
        inf
      }

      PROC_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
        end
        inf
      }
=end

      _setup_subst_table(KEY_TBL, PROC_TBL);

      def self.ret_val(val)
        val
      end
    end

    def self._config_keys
      ['opencommand', 'closecomand']
    end
  end

  def __validation_class_list
    super() << OpenCloseCommand
  end

  Tk::ValidateConfigure.__def_validcmd(binding, OpenCloseCommand)

  ########################

  def __item_validation_class_list(id)
    case id
    when 'entry'
      super(id) << OpenCloseCommand
    else
      super(id)
    end
  end

  Tk::ItemValidateConfigure.__def_validcmd(binding, OpenCloseCommand)

  ########################

  def __destroy_hook__
    Tk::BLT::Treeview::Node::TreeNodeID_TBL.mutex.synchronize{
      Tk::BLT::Treeview::Node::TreeNodeID_TBL.delete(@path)
    }
    Tk::BLT::Treeview::Tag::TreeTagID_TBL.mutex.synchronize{
      Tk::BLT::Treeview::Tag::TreeTagID_TBL.delete(@path)
    }
  end

  def tagid(tag)
    if tag.kind_of?(Tk::BLT::Treeview::Node) \
      || tag.kind_of?(Tk::BLT::Treeview::Tag)
      tag.id
    else
      tag  # maybe an Array of configure paramters
    end
  end
  private :tagid

  def tagid2obj(tagid)
    if tagid.kind_of?(Integer)
      Tk::BLT::Treeview::Node.id2obj(self, tagid.to_s)
    elsif tagid.kind_of?(String)
      if tagid =~ /^\d+$/
        Tk::BLT::Treeview::Node.id2obj(self, tagid)
      else
        Tk::BLT::Treeview::Tag.id2obj(self, tagid)
      end
    else
      tagid
    end
  end

  def bbox(*tags)
    list(tk_send('bbox', *(tags.collect{|tag| tagid(tag)})))
  end

  def screen_bbox(*tags)
    list(tk_send('bbox', '-screen', *(tags.collect{|tag| tagid(tag)})))
  end

  def tag_bind(tag, seq, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([@path, 'bind', tagid(tag)], seq, cmd, *args)
    self
  end
  def tag_bind_append(tag, seq, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([@path, 'bind', tagid(tag)], seq, cmd, *args)
    self
  end
  def tag_bind_remove(tag, seq)
    _bind_remove([@path, 'bind', tagid(tag)], seq)
    self
  end
  def tag_bindinfo(tag, seq=nil)
    _bindinfo([@path, 'bind', tagid(tag)], seq)
  end

  def button_activate(tag)
    tk_send('button', 'activate', tagid(tag))
    self
  end

  def button_bind(tag, seq, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([@path, 'button', 'bind', tagid(tag)], seq, cmd, *args)
    self
  end
  def button_bind_append(tag, seq, *args)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([@path, 'button', 'bind', tagid(tag)], seq, cmd, *args)
    self
  end
  def button_bind_remove(tag, seq)
    _bind_remove([@path, 'button', 'bind', tagid(tag)], seq)
    self
  end
  def button_bindinfo(tag, seq=nil)
    _bindinfo([@path, 'button', 'bind', tagid(tag)], seq)
  end

  def close(*tags)
    tk_send('close', *(tags.collect{|tag| tagid(tag)}))
    self
  end
  def close_recurse(*tags)
    tk_send('close', '-recurse', *(tags.collect{|tag| tagid(tag)}))
    self
  end

  def column_activate(column=None)
    if column == None
      tk_send('column', 'activate')
    else
      tk_send('column', 'activate', column)
      self
    end
  end

  def column_delete(*fields)
    tk_send('column', 'delete', *fields)
    self
  end
  def column_insert(pos, field, *opts)
    tk_send('column', 'insert', pos, field, *opts)
    self
  end
  def column_invoke(field)
    tk_send('column', 'invoke', field)
    self
  end
  def column_move(name, dest)
    tk_send('column', 'move', name, dest)
    self
  end
  def column_names()
    simplelist(tk_send('column', 'names'))
  end
  def column_nearest(x, y=None)
    tk_send('column', 'nearest', x, y)
  end

  def curselection
    simplelist(tk_send('curselection')).collect{|id| tagid2obj(id)}
  end

  def delete(*tags)
    tk_send('delete', *(tags.collect{|tag| tagid(tag)}))
    self
  end

  def entry_activate(tag)
    tk_send('entry', 'activate', tagid(tag))
    self
  end
  def entry_children(tag, first=None, last=None)
    simplelist(tk_send('entry', 'children', tagid(tag),
                       first, last)).collect{|id| tagid2obj(id)}
  end
  def entry_delete(tag, first=None, last=None)
    tk_send('entry', 'delete', tagid(tag), first, last)
  end
  def entry_before?(tag1, tag2)
    bool(tk_send('entry', 'isbefore', tagid(tag1), tagid(tag2)))
  end
  def entry_hidden?(tag)
    bool(tk_send('entry', 'ishidden', tagid(tag)))
  end
  def entry_open?(tag)
    bool(tk_send('entry', 'isopen', tagid(tag)))
  end

  def entry_size(tag)
    number(tk_send('entry', 'size', tagid(tag)))
  end
  def entry_size_recurse(tag)
    number(tk_send('entry', 'size', '-recurse', tagid(tag)))
  end

  def _search_flags(keys)
    keys = _symbolkey2str(keys)
    keys['exact'] = None if keys.delete('exact')
    keys['glob'] = None if keys.delete('glob')
    keys['regexp'] = None if keys.delete('regexp')
    keys['nonmatching'] = None if keys.delete('nonmatching')
  end
  private :_search_flags

  ################################

  class FindExecFlagValue < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?W, ?w, :widget ],
        [ ?p, ?s, :name ],
        [ ?P, ?s, :fullpath ],
        [ ?#, ?x, :node_id ],
        nil
      ]

      PROC_TBL = [
        [ ?x, TkComm.method(:num_or_str) ],
        [ ?s, TkComm.method(:string) ],
        [ ?w, TkComm.method(:window) ],
        nil
      ]

=begin
      # for Ruby m17n :: ?x --> String --> char-code ( getbyte(0) )
      KEY_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
          inf[1] = inf[1].getbyte(0) if inf[1].kind_of?(String)
        end
        inf
      }

      PROC_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
        end
        inf
      }
=end

      _setup_subst_table(KEY_TBL, PROC_TBL);

      def self.ret_val(val)
        val
      end
    end

    def self._config_keys
      []
    end
  end

  def _find_exec_flag_value(val)
    if val.kind_of?(Array)
      cmd, *args = val
      #FindExecFlagValue.new(cmd, args.join(' '))
      FindExecFlagValue.new(cmd, *args)
    elsif TkComm._callback_entry?(val)
      FindExecFlagValue.new(val)
    else
      val
    end
  end

  ################################

  def find(first, last, keys={})
    keys = _search_flags(keys)
    keys['exec'] = _find_exec_flag_value(keys['exec']) if keys.key?('exec')
    args = hash_kv(keys) << '--' << tagid(first) << tagid(last)
    simplelist(tk_send('find', *args)).collect{|id| tagid2obj(id)}
  end

  def tag_focus(tag)
    tk_send('focus', tagid(tag))
    self
  end
  def get(*tags)
    simplelist(tk_send('get', *(tags.collect{|tag| tagid(tag)})))
  end
  def get_full(*tags)
    simplelist(tk_send('get', '-full', *(tags.collect{|tag| tagid(tag)})))
  end

  def hide(*tags)
    if tags[-1].kind_of?(Hash)
      keys = tags.pop
    else
      keys = {}
    end
    keys = _search_flags(keys)
    args = hash_kv(keys) << '--'
    args.concat(tags.collect{|t| tagid(t)})
    tk_send('hide', *args)
    self
  end

  def index(str)
    tagid2obj(tk_send('index', str))
  end
  def index_at(tag, str)
    tagid2obj(tk_send('index', '-at', tagid(tag), str))
  end
  def index_at_path(tag, str)
    tagid2obj(tk_send('index', '-at', tagid(tag), '-path', str))
  end

  def insert(pos, parent=nil, keys={})
    Tk::BLT::Treeview::Node.new(pos, parent, keys)
  end
  def insert_at(tag, pos, parent=nil, keys={})
    if parent.kind_of?(Hash)
      keys = parent
      parent = nil
    end

    keys = _symbolkey2str(keys)
    keys['at'] = tagid(tag)

    Tk::BLT::Treeview::Node.new(pos, parent, keys)
  end

  def move_before(tag, dest)
    tk_send('move', tagid(tag), 'before', tagid(dest))
    self
  end
  def move_after(tag, dest)
    tk_send('move', tagid(tag), 'after', tagid(dest))
    self
  end
  def move_into(tag, dest)
    tk_send('move', tagid(tag), 'into', tagid(dest))
    self
  end

  def nearest(x, y, var=None)
    tagid2obj(tk_send('nearest', x, y, var))
  end

  def open(*tags)
    tk_send('open', *(tags.collect{|tag| tagid(tag)}))
    self
  end
  def open_recurse(*tags)
    tk_send('open', '-recurse', *(tags.collect{|tag| tagid(tag)}))
    self
  end

  def range(first, last)
    simplelist(tk_send('range', tagid(first), tagid(last))).collect{|id|
      tagid2obj(id)
    }
  end
  def range_open(first, last)
    simplelist(tk_send('range', '-open',
                       tagid(first), tagid(last))).collect{|id|
      tagid2obj(id)
    }
  end

  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send_without_enc('scan', 'dragto', x, y)
    self
  end

  def see(tag)
    tk_send_without_enc('see', tagid(tag))
    self
  end
  def see_anchor(anchor, tag)
    tk_send_without_enc('see', '-anchor', anchor, tagid(tag))
    self
  end

  def selection_anchor(tag)
    tk_send_without_enc('selection', 'anchor', tagid(tag))
    self
  end
  def selection_cancel()
    tk_send_without_enc('selection', 'cancel')
    self
  end
  def selection_clear(first, last=None)
    tk_send_without_enc('selection', 'clear', tagid(first), tagid(last))
    self
  end
  def selection_clear_all()
    tk_send_without_enc('selection', 'clearall')
    self
  end
  def selection_mark(tag)
    tk_send_without_enc('selection', 'mark', tagid(tag))
    self
  end
  def selection_include?(tag)
    bool(tk_send('selection', 'include', tagid(tag)))
  end
  def selection_present?()
    bool(tk_send('selection', 'present'))
  end
  def selection_set(first, last=None)
    tk_send_without_enc('selection', 'set', tagid(first), tagid(last))
    self
  end
  def selection_toggle(first, last=None)
    tk_send_without_enc('selection', 'toggle', tagid(first), tagid(last))
    self
  end

  def show(*tags)
    if tags[-1].kind_of?(Hash)
      keys = tags.pop
    else
      keys = {}
    end
    keys = _search_flags(keys)
    args = hash_kv(keys) << '--'
    args.concat(tags.collect{|t| tagid(t)})
    tk_send('show', *args)
    self
  end

  def sort_auto(mode)
    tk_send('sort', 'auto', mode)
    self
  end
  def sort_auto=(mode)
    tk_send('sort', 'auto', mode)
    mode
  end
  def sort_auto?
    bool(tk_send('sort', 'auto'))
  end
  def sort_once(*tags)
    tk_send('sort', 'once', *(tags.collect{|tag| tagid(tag)}))
    self
  end
  def sort_once_recurse(*tags)
    tk_send('sort', 'once', '-recurse', *(tags.collect{|tag| tagid(tag)}))
    self
  end

  def tag_add(tag, *ids)
    tk_send('tag', 'add', tagid(tag), *ids)
    self
  end
  def tag_delete(tag, *ids)
    tk_send('tag', 'delete', tagid(tag), *ids)
    self
  end
  def tag_forget(tag)
    tk_send('tag', 'forget', tagid(tag))
    self
  end
  def tag_names(id=nil)
    id = (id)? tagid(id): None

    simplelist(tk_send('tag', 'nodes', id)).collect{|tag|
      Tk::BLT::Treeview::Tag.id2obj(self, tag)
    }
  end
  def tag_nodes(tag)
    simplelist(tk_send('tag', 'nodes', tagid(tag))).collect{|id|
      Tk::BLT::Treeview::Node.id2obj(self, id)
    }
  end

  def text_apply
    tk_send('text', 'apply')
    self
  end
  def text_cancel
    tk_send('text', 'cancel')
    self
  end

  def text_delete(first, last)
    tk_send('text', 'delete', first, last)
    self
  end
  def text_get(x, y)
    tk_send('text', 'get', x, y)
  end
  def text_get_root(x, y)
    tk_send('text', 'get', '-root', x, y)
  end
  def text_icursor(idx)
    tk_send('text', 'icursor', idx)
    self
  end
  def text_index(idx)
    num_or_str(tk_send('text', 'index', idx))
  end
  def text_insert(idx, str)
    tk_send('text', 'insert', idx, str)
    self
  end

  def text_selection_adjust(idx)
    tk_send('text', 'selection', 'adjust', idx)
    self
  end
  def text_selection_clear
    tk_send('text', 'selection', 'clear')
    self
  end
  def text_selection_from(idx)
    tk_send('text', 'selection', 'from', idx)
    self
  end
  def text_selection_present
    num_or_str(tk_send('text', 'selection', 'present'))
  end
  def text_selection_range(start, last)
    tk_send('text', 'selection', 'range', start, last)
    self
  end
  def text_selection_to(idx)
    tk_send('text', 'selection', 'to', idx)
    self
  end

  def toggle(tag)
    tk_send('toggle', tagid(tag))
    self
  end
end

######################################

module Tk::BLT::Treeview::TagOrID_Methods
  def bbox
    @tree.bbox(self)
  end
  def screen_bbox
    @tree.screen_bbox(self)
  end

  def bind(seq, *args)
    @tree.tag_bind(self, seq, *args)
    self
  end
  def bind_append(seq, *args)
    @tree.tag_bind_append(self, seq, *args)
    self
  end
  def bind_remove(seq)
    @tree.tag_bind_remove(self, seq)
    self
  end
  def bindinfo(seq=nil)
    @tree.tag_bindinfo(self, seq)
  end

  def button_activate
    @tree.button_activate(self)
    self
  end

  def button_bind(seq, *args)
    @tree.button_bind(self, seq, *args)
    self
  end
  def button_bind_append(seq, *args)
    @tree.button_bind_append(self, seq, *args)
    self
  end
  def button_bind_remove(seq)
    @tree.button_bind_remove(self, seq)
    self
  end
  def button_bindinfo(seq=nil)
    @tree.button_bindinfo(self, seq)
  end

  def close
    @tree.close(self)
    self
  end
  def close_recurse
    @tree.close_recurse(self)
    self
  end

  def delete
    @tree.delete(self)
    self
  end

  def entry_activate
    @tree.entry_activate(self)
    self
  end
  def entry_children(first=None, last=None)
    @tree.entry_children(self, first, last)
  end
  def entry_delete(first=None, last=None)
    @tree.entry_delete(self, first, last)
  end
  def entry_before?(tag)
    @tree.entry_before?(self, tag)
  end
  def entry_hidden?
    @tree.entry_before?(self)
  end
  def entry_open?
    @tree.entry_open?(self)
  end

  def entry_size
    @tree.entry_size(self)
  end
  def entry_size_recurse
    @tree.entry_size_recurse(self)
  end

  def focus
    @tree.tag_focus(self)
    self
  end

  def get
    @tree.get(self)
  end
  def get_full
    @tree.get_full(self)
  end

  def hide
    @tree.hide(self)
    self
  end

  def index(str)
    @tree.index_at(self, str)
  end
  def index_path(str)
    @tree.index_at_path(self, str)
  end

  def insert(pos, parent=nil, keys={})
    @tree.insert_at(self, pos, parent, keys)
  end

  def move_before(dest)
    @tree.move_before(self, dest)
    self
  end
  def move_after(dest)
    @tree.move_after(self, dest)
    self
  end
  def move_into(dest)
    @tree.move_into(self, dest)
    self
  end

  def open
    @tree.open(self)
    self
  end
  def open_recurse
    @tree.open_recurse(self)
    self
  end

  def range_to(tag)
    @tree.range(self, tag)
  end
  def range_open_to(tag)
    @tree.range(self, tag)
  end

  def see
    @tree.see(self)
    self
  end
  def see_anchor(anchor)
    @tree.see_anchor(anchor, self)
    self
  end

  def selection_anchor
    @tree.selection_anchor(self)
    self
  end
  def selection_clear
    @tree.selection_clear(self)
    self
  end
  def selection_mark
    @tree.selection_mark(self)
    self
  end
  def selection_include?
    @tree.selection_include?(self)
  end
  def selection_set
    @tree.selection_set(self)
    self
  end
  def selection_toggle
    @tree.selection_toggle(self)
    self
  end

  def show
    @tree.show(self)
    self
  end

  def sort_once
    @tree.sort_once(self)
    self
  end
  def sort_once_recurse
    @tree.sort_once_recurse(self)
    self
  end

  def toggle
    @tree.toggle(self)
    self
  end
end

######################################

class Tk::BLT::Treeview::Node < TkObject
  include Tk::BLT::Treeview::TagOrID_Methods

  TreeNodeID_TBL = TkCore::INTERP.create_table

  (TreeNode_ID = ['blt_treeview_node'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    TreeNodeID_TBL.mutex.synchronize{ TreeNodeID_TBL.clear }
  }

  def self.id2obj(tree, id)
    tpath = tree.path
    TreeNodeID_TBL.mutex.synchronize{
      if TreeNodeID_TBL[tpath]
        if TreeNodeID_TBL[tpath][id]
          TreeNodeID_TBL[tpath][id]
        else
          begin
            # self.new(tree, nil, nil, 'node'=>Integer(id))
            unless (tk_call(@tpath, 'get', id)).empty?
              id = Integer(id)
              (obj = self.allocate).instance_eval{
                @parent = @tree = tree
                @tpath = @parent.path
                @path = @id = id
                TreeNodeID_TBL[@tpath] ||= {}
                TreeNodeID_TBL[@tpath][@id] = self
              }
              obj
            else
              id
            end
          rescue
            id
          end
        end
      else
        id
      end
    }
  end

  def self.new(tree, pos, parent=nil, keys={})
    if parent.kind_of?(Hash)
      keys = parent
      parent = nil
    end

    keys = _symbolkey2str(keys)
    tpath = tree.path

    TreeNodeID_TBL.mutex.synchronize{
      TreeNodeID_TBL[tpath] ||= {}
      if (id = keys['node']) && (obj = TreeNodeID_TBL[tpath][id])
        keys.delete('node')
        tk_call(tree.path, 'move', id, pos, parent) if parent
        return obj
      end

      #super(tree, pos, parent, keys)
      (obj = self.allocate).instance_eval{
        initialize(tree, pos, parent, keys)
        TreeNodeID_TBL[tpath][@id] = self
      }
      obj
    }
  end

  def initialize(tree, pos, parent, keys)
    @parent = @tree = tree
    @tpath = @parent.path

    if (id = keys['node'])
      # if tk_call(@tpath, 'get', id).empty?
      #   fail RuntimeError, "not exist the node '#{id}'"
      # end
      @path = @id = id
      tk_call(@tpath, 'move', @id, pos, tagid(parent)) if parent
      configure(keys) if keys && ! keys.empty?
    else
      name = nil
      TreeNode_ID.mutex.synchronize{
        name = TreeNode_ID.join(TkCore::INTERP._ip_id_).freeze
        TreeNode_ID[1].succ!
      }

      at = keys.delete['at']

      if parent
        if parent.kind_of?(Tk::BLT::Treeview::Node) ||
            parent.kind_of?(Tk::BLT::Treeview::Tag)
          path = [get_full(parent.id)[0], name]
          at = nil # ignore 'at' option
        else
          path = [parent.to_s, name]
        end
      else
        path = name
      end

      if at
        @id = tk_call(@tpath, 'insert', '-at', tagid(at), pos, path, keys)
      else
        @id = tk_call(@tpath, 'insert', pos, path, keys)
      end
      @path = @id
    end
  end

  def id
    @id
  end
end

######################################

class Tk::BLT::Treeview::Tag < TkObject
  include Tk::BLT::Treeview::TagOrID_Methods

  TreeTagID_TBL = TkCore::INTERP.create_table

  (TreeTag_ID = ['blt_treeview_tag'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    TreeTagID_TBL.mutex.synchronize{ TreeTagID_TBL.clear }
  }

  def self.id2obj(tree, name)
    tpath = tree.path
    TreeTagID_TBL.mutex.synchronize{
      if TreeTagID_TBL[tpath]
        if TreeTagID_TBL[tpath][name]
          TreeTagID_TBL[tpath][name]
        else
          #self.new(tree, name)
          (obj = self.allocate).instance_eval{
            @parent = @tree = tree
            @tpath = @parent.path
            @path = @id = name
            TreeTagID_TBL[@tpath] = {} unless TreeTagID_TBL[@tpath]
            TreeTagID_TBL[@tpath][@id] = self
          }
          obj
        end
      else
        id
      end
    }
  end

  def self.new_by_name(tree, name, *ids)
    TreeTagID_TBL.mutex.synchronize{
      unless (obj = TreeTagID_TBL[tree.path][name])
        (obj = self.allocate).instance_eval{
          initialize(tree, name, ids)
          TreeTagID_TBL[@tpath] = {} unless TreeTagID_TBL[@tpath]
          TreeTagID_TBL[@tpath][@id] = self
        }
      end
      obj
    }
  end

  def self.new(tree, *ids)
    TreeTagID_TBL.mutex.synchronize{
      (obj = self.allocate).instance_eval{
        if tree.kind_of?(Array)
          initialize(tree[0], tree[1], ids)
        else
          initialize(tree, nil, ids)
        end
        TreeTagID_TBL[@tpath] = {} unless TreeTagID_TBL[@tpath]
        TreeTagID_TBL[@tpath][@id] = self
      }
      obj
    }
  end

  def initialize(tree, name, ids)
    @parent = @tree = tree
    @tpath = @parent.path

    if name
      @path = @id = name
    else
      TreeTag_ID.mutex.synchronize{
        @path = @id = TreeTag_ID.join(TkCore::INTERP._ip_id_).freeze
        TreeTag_ID[1].succ!
      }
    end

    unless ids.empty?
      tk_call(@tpath, 'tag', 'add', @id, *(ids.collect{|id| tagid(id)}))
    end
  end

  def tagid(tag)
    if tag.kind_of?(Tk::BLT::Treeview::Node) \
      || tag.kind_of?(Tk::BLT::Treeview::Tag)
      tag.id
    else
      tag
    end
  end
  private :tagid

  def id
    @id
  end

  def add(*ids)
    tk_call(@tpath, 'tag', 'add', @id, *(ids{|id| tagid(id)}))
    self
  end

  def remove(*ids)
    tk_call(@tpath, 'tag', 'delete', @id, *(ids{|id| tagid(id)}))
    self
  end

  def forget
    tk_call(@tpath, 'tag', 'forget', @id)
    self
  end

  def nodes
    simplelist(tk_call(@tpath, 'tag', 'nodes', @id)).collect{|id|
      Tk::BLT::Treeview::Node.id2obj(@tree, id)
    }
  end
end

class Tk::BLT::Hiertable
  TkCommandNames = ['::blt::hiertable'.freeze].freeze
  WidgetClassName = 'Hiertable'.freeze
  WidgetClassNames[WidgetClassName] ||= self
end
