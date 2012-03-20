#
#  tkextlib/tktable/tktable.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/validation'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tktable/setup.rb'

# TkPackage.require('Tktable', '2.8')
TkPackage.require('Tktable')

module Tk
  class TkTable < TkWindow
    PACKAGE_NAME = 'Tktable'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('Tktable')
      rescue
        ''
      end
    end

    class CellTag < TkObject
    end

    module ConfigMethod
    end
  end
end

module Tk::TkTable::ConfigMethod
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

  def __item_boolval_optkeys(id)
    super(id) << 'multiline' << 'showtext' << 'wrap'
  end
  private :__item_boolval_optkeys

  def __item_strval_optkeys(id)
    super(id) << 'ellipsis'
  end
  private :__item_strval_optkeys

  def __item_val2ruby_optkeys(id)  # { key=>method, ... }
    super(id).update('window'=>proc{|v| window(v)})
  end
  private :__item_val2ruby_optkeys

  def tag_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['tag', tagid(tagOrId)], option)
  end
  def tag_cget(tagOrId, option)
    itemcget(['tag', tagid(tagOrId)], option)
  end
  def tag_cget_strict(tagOrId, option)
    itemcget_strict(['tag', tagid(tagOrId)], option)
  end
  def tag_configure(tagOrId, slot, value=None)
    itemconfigure(['tag', tagid(tagOrId)], slot, value)
  end
  def tag_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['tag', tagid(tagOrId)], slot)
  end
  def current_tag_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['tag', tagid(tagOrId)], slot)
  end

  def window_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['window', tagid(tagOrId)], option)
  end
  def window_cget(tagOrId, option)
    itemcget(['window', tagid(tagOrId)], option)
  end
  def window_cget_strict(tagOrId, option)
    itemcget_strict(['window', tagid(tagOrId)], option)
  end
  def window_configure(tagOrId, slot, value=None)
    if slot == :window || slot == 'window'
      value = _epath(value)
    elsif slot.kind_of?(Hash)
      if slot.key?(:window) || slot.key?('window')
        slot = _symbolkey2str(slot)
        slot['window'] = _epath(slot['window'])
      end
    end
    itemconfigure(['window', tagid(tagOrId)], slot, value)
  end
  def window_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['window', tagid(tagOrId)], slot)
  end
  def current_window_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['window', tagid(tagOrId)], slot)
  end

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo
end

#####################################################

class Tk::TkTable::CellTag
  include TkTreatTagFont

  CellTagID_TBL = TkCore::INTERP.create_table

  (CellTag_ID = ['tktbl:celltag'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  TkCore::INTERP.init_ip_env{
    CellTagID_TBL.mutex.synchronize{ CellTagID_TBL.clear }
  }

  def self.id2obj(table, id)
    tpath = table.path
    CellTagID_TBL.mutex.synchronize{
      if CellTagID_TBL[tpath]
        CellTagID_TBL[tpath][id]? CellTagID_TBL[tpath][id] : id
      else
        id
      end
    }
  end

  def initialize(parent, keys=nil)
    @parent = @t = parent
    @tpath - parent.path
    CellTag_ID.mutex.synchronize{
      @path = @id = CellTag_ID.join(TkCore::INTERP._ip_id_)
      CellTag_ID[1].succ!
    }
    CellTagID_TBL.mutex.synchronize{
      CellTagID_TBL[@tpath] = {} unless CellTagID_TBL[@tpath]
      CellTagID_TBL[@tpath][@id] = self
    }
    configure(keys) if keys
  end

  def id
    @id
  end

  def destroy
    tk_call(@tpath, 'tag', 'delete', @id)
    CellTagID_TBL.mutex.synchronize{
      CellTagID_TBL[@tpath].delete(@id) if CellTagID_TBL[@tpath]
    }
    self
  end
  alias delete destroy

  def exist?
    @t.tag_exist?(@id)
  end
  def include?(idx)
    @t.tag_include?(@id, idx)
  end

  def add_cell(*args)
    @t.tag_cell(@id, *args)
  end
  def add_col(*args)
    @t.tag_col(@id, *args)
  end
  def add_row(*args)
    @t.tag_row(@id, *args)
  end

  def raise(target=None)
    @t.tag_raise(@id, target)
  end
  def lower(target=None)
    @t.tag_lower(@id, target)
  end

  def cget_tkstring(key)
    @t.tag_cget_tkstring(@id, key)
  end
  def cget(key)
    @t.tag_cget(@id, key)
  end
  def cget_strict(key)
    @t.tag_cget_strict(@id, key)
  end
  def configure(key, val=None)
    @t.tag_configure(@id, key, val)
  end
  def configinfo(key=nil)
    @t.tag_configinfo(@id, key)
  end
  def current_configinfo(key=nil)
    @t.current_tag_configinfo(@id, key)
  end
end

class Tk::TkTable::NamedCellTag < Tk::TkTable::CellTag
  def self.new(parent, name, keys=nil)
    obj = nil
    CellTagID_TBL.mutex.synchronize{
      if CellTagID_TBL[parent.path] && CellTagID_TBL[parent.path][name]
        obj = CellTagID_TBL[parent.path][name]
      else
        #super(parent, name, keys)
        (obj = self.allocate).instance_eval{
          @parent = @t = parent
          @tpath = parent.path
          @path = @id = name
          CellTagID_TBL[@tpath] = {} unless CellTagID_TBL[@tpath]
          CellTagID_TBL[@tpath][@id] = self
        }
      end
    }
    obj.configure(keys) if keys && ! keys.empty?
    obj
  end

  def initialize(parent, name, keys=nil)
    # dummy:: not called by 'new' method
    @parent = @t = parent
    @tpath = parent.path
    @path = @id = name
    CellTagID_TBL.mutex.synchronize{
      CellTagID_TBL[@tpath] = {} unless CellTagID_TBL[@tpath]
      CellTagID_TBL[@tpath][@id] = self
    }
    configure(keys) if keys && ! keys.empty?
  end
end

#####################################################

class Tk::TkTable
  TkCommandNames = ['table'.freeze].freeze
  WidgetClassName = 'Table'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  include Scrollable
  include Tk::TkTable::ConfigMethod
  include Tk::ValidateConfigure

  def __destroy_hook__
    Tk::TkTable::CelTag::CellTagID_TBL.mutex.synchronize{
      Tk::TkTable::CelTag::CellTagID_TBL.delete(@path)
    }
  end

  def __boolval_optkeys
    super() << 'autoclear' << 'flashmode' << 'invertselected' <<
      'multiline' << 'selecttitle' << 'wrap'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'colseparator' << 'ellipsis' << 'rowseparator' << 'sparsearray'
  end
  private :__strval_optkeys


  #################################

  class BrowseCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?c, ?n, :column ],
        [ ?C, ?s, :index ],
        [ ?i, ?x, :cursor ],
        [ ?r, ?n, :row ],
        [ ?s, ?s, :last_index ],
        [ ?S, ?s, :new_index ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:number) ],
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
      ['browsecommand', 'browsecmd']
    end
  end
  #--------------------------------
  class CellCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?c, ?n, :column ],
        [ ?C, ?s, :index ],
        [ ?i, ?m, :rw_mode ],
        [ ?r, ?n, :row ],
        [ ?s, ?v, :value ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:number) ],
        [ ?s, TkComm.method(:string) ],
        [ ?w, TkComm.method(:window) ],
        [ ?m, proc{|val| (val == '0')? (:r) : (:w)} ],
        [ ?v, proc{|val| TkComm.tk_tcl2ruby(val, true, false)} ],
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
        TkComm._get_eval_string(val)
      end
    end

    def self._config_keys
      ['command']
    end
  end
  #--------------------------------
  class SelectionCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?c, ?n, :sel_columns ],
        [ ?C, ?s, :sel_area ],
        [ ?i, ?n, :total ],
        [ ?r, ?n, :sel_rows ],
        [ ?s, ?s, :value ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:number) ],
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
        val.to_s
      end
    end

    def self._config_keys
      ['selectioncommand', 'selcmd']
    end
  end
  #--------------------------------
  class ValidateCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?c, ?n, :column ],
        [ ?C, ?s, :index ],
        [ ?i, ?x, :cursor ],
        [ ?r, ?n, :row ],
        [ ?s, ?v, :current_value ],
        [ ?S, ?v, :new_value ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:number) ],
        [ ?x, TkComm.method(:num_or_str) ],
        [ ?s, TkComm.method(:string) ],
        [ ?w, TkComm.method(:window) ],
        [ ?v, proc{|val| TkComm.tk_tcl2ruby(val, true, false)} ],
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
    end

    def self._config_keys
      ['vcmd', 'validatecommand']
    end
  end

  #################################

  def __validation_class_list
    super() <<
      BrowseCommand << CellCommand << SelectionCommand << ValidateCommand
  end

  Tk::ValidateConfigure.__def_validcmd(binding, BrowseCommand)
  Tk::ValidateConfigure.__def_validcmd(binding, CellCommand)
  Tk::ValidateConfigure.__def_validcmd(binding, SelectionCommand)
  Tk::ValidateConfigure.__def_validcmd(binding, ValidateCommand)

  #################################

  def activate(idx)
    tk_send('activate', tagid(idx))
  end

  def bbox(idx)
    list(tk_send('bbox', tagid(idx)))
  end

  def border_mark(x, y)
    simplelist(tk_send('border', 'mark', x, y))
  end
  def border_mark_row(x, y)
    tk_send('border', 'mark', x, y, 'row')
  end
  def border_mark_col(x, y)
    tk_send('border', 'mark', x, y, 'col')
  end
  def border_dragto(x, y)
    tk_send('border', 'dragto', x, y)
  end

  def clear_cache(first=None, last=None)
    tk_send('clear', 'cache', tagid(first), tagid(last))
    self
  end
  def clear_sizes(first=None, last=None)
    tk_send('clear', 'sizes', tagid(first), tagid(last))
    self
  end
  def clear_tags(first=None, last=None)
    tk_send('clear', 'tags', tagid(first), tagid(last))
    self
  end
  def clear_all(first=None, last=None)
    tk_send('clear', 'all', tagid(first), tagid(last))
    self
  end

  def curselection
    simplelist(tk_send('curselection'))
  end
  def curselection=(val)
    tk_send('curselection', val)
    val
  end

  def curvalue
    tk_tcl2ruby(tk_send('curvalue'), true, false)
  end
  def curvalue=(val)
    tk_send('curvalue', val)
    val
  end

  def delete_active(idx1, idx2=None)
    tk_send('delete', 'active', tagid(idx1), tagid(idx2))
    self
  end
  def delete_cols(*args) # ?switches_array?, index, ?count?
    params = []
    if args[0].kind_of?(Array)
      switches = args.shift
      switches.each{|k| params << "-#{k}"}
    end
    params << '--'
    params << tagid(args.shift)
    params.concat(args)
    tk_send('delete', 'cols', *params)
    self
  end
  def delete_rows(*args) # ?switches_array?, index, ?count?
    params = []
    if args[0].kind_of?(Array)
      switches = args.shift
      switches.each{|k| params << "-#{k}"}
    end
    params << '--'
    params << tagid(args.shift)
    params.concat(args)
    tk_send('delete', 'rows', *params)
    self
  end

  def get(idx)
    tk_tcl2ruby(tk_send('get', tagid(idx)), true, false)
  end
  def get_area(idx1, idx2)
    simplelist(tk_send('get', tagid(idx1), tagid(idx2))).collect{|v|
      tk_tcl2ruby(v, true, false)
    }
  end

  def height_list
    list(tk_send('height'))
  end
  def height(row)
    number(tk_send('height', row))
  end
  def set_height(*pairs)
    tk_send('height', *(pairs.flatten))
    self
  end

  def hidden_list
    simplelist(tk_send('hidden'))
  end
  def hidden?(idx, *args)
    if args.empty?
      if (ret = tk_send('hidden', tagid(idx))) == ''
        false
      else
        ret
      end
    else
      bool(tk_send('hidden', tagid(idx), *(args.collect{|i| tagid(i)})))
    end
  end

  def icursor
    number(tk_send('icursor'))
  end
  def icursor_set(idx)
    number(tk_send('icursor', tagid(idx)))
  end

  def index(idx)
    tk_send('index', tagid(idx))
  end
  def row_index(idx)
    number(tk_send('index', tagid(idx), 'row'))
  end
  def col_index(idx)
    number(tk_send('index', tagid(idx), 'col'))
  end

  def insert_active(idx, val)
    tk_send('insert', 'active', tagid(idx), val)
    self
  end
  def insert_cols(*args) # ?switches_array?, index, ?count?
    params = []
    if args[0].kind_of?(Array)
      switches = args.shift
      switches.each{|k| params << "-#{k}"}
    end
    params << '--'
    params.concat(args)
    params << tagid(args.shift)
    tk_send('insert', 'cols', *params)
    self
  end
  def insert_rows(*args) # ?switches_array?, index, ?count?
    params = []
    if args[0].kind_of?(Array)
      switches = args.shift
      switches.each{|k| params << "-#{k}"}
    end
    params << '--'
    params << tagid(args.shift)
    params.concat(args)
    tk_send('insert', 'rows', *params)
    self
  end

  # def postscript(*args)
  #   tk_send('postscript', *args)
  # end

  def reread
    tk_send('reread')
    self
  end

  def scan_mark(x, y)
    tk_send('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send('scan', 'dragto', x, y)
    self
  end

  def see(idx)
    tk_send('see', tagid(idx))
    self
  end

  def selection_anchor(idx)
    tk_send('selection', 'anchor', tagid(idx))
    self
  end
  def selection_clear(first, last=None)
    tk_send('selection', 'clear', tagid(first), tagid(last))
    self
  end
  def selection_clear_all
    selection_clear('all')
  end
  def selection_include?(idx)
    bool(tk_send('selection', 'includes', tagid(idx)))
  end
  def selection_present
    bool(tk_send('selection', 'present'))
  end
  def selection_set(first, last=None)
    tk_send('selection', 'set', tagid(first), tagid(last))
    self
  end

  def set(*pairs) # idx, val, idx, val, ...
    args = []
    0.step(pairs.size-1, 2){|i|
      args << tagid(pairs[i])
      args << pairs[i+1]
    }
    tk_send('set', *args)
    self
  end
  def set_row(*pairs) # idx, val, idx, val, ...
    args = []
    0.step(pairs.size-1, 2){|i|
      args << tagid(pairs[i])
      args << pairs[i+1]
    }
    tk_send('set', 'row', *args)
    self
  end
  def set_col(*pairs) # idx, val, idx, val, ...
    args = []
    0.step(pairs.size-1, 2){|i|
      args << tagid(pairs[i])
      args << pairs[i+1]
    }
    tk_send('set', 'col', *args)
    self
  end
=begin
  def set(*pairs) # idx, val, idx, val, ...  OR [idx, val], [idx, val], ...
    if pairs[0].kind_of?(Array)
      # [idx, val], [idx, val], ...
      args = []
      pairs.each{|idx, val| args << tagid(idx) << val }
      tk_send('set', *args)
    else
      # idx, val, idx, val, ...
      args = []
      0.step(pairs.size-1, 2){|i|
        args << tagid(pairs[i])
        args << pairs[i+1]
      }
      tk_send('set', *args)
    end
    self
  end
  def set_row(*pairs)
    if pairs[0].kind_of?(Array)
      # [idx, val], [idx, val], ...
      args = []
      pairs.each{|idx, val| args << tagid(idx) << val }
      tk_send('set', 'row', *args)
    else
      # idx, val, idx, val, ...
      args = []
      0.step(pairs.size-1, 2){|i|
        args << tagid(pairs[i])
        args << pairs[i+1]
      }
      tk_send('set', 'row', *args)
    end
    self
  end
  def set_col(*pairs)
    if pairs[0].kind_of?(Array)
      # [idx, val], [idx, val], ...
      args = []
      pairs.each{|idx, val| args << idx << val }
      tk_send('set', 'col', *args)
    else
      # idx, val, idx, val, ...
      args = []
      0.step(pairs.size-1, 2){|i|
        args << tagid(pairs[i])
        args << pairs[i+1]
      }
      tk_send('set', 'col', *args)
    end
    self
  end
=end

  def spans
    simplelist(tk_send('spans')).collect{|inf|
      lst = simplelist(inf)
      idx = lst[0]
      rows, cols = lst[1].split(',').map!{|n| Integer(n)}
      [idx [rows, cols]]
    }
  end
  alias span_list spans
  def span(idx)
    lst = simplelist(tk_send('spans', tagid(idx)))
    idx = lst[0]
    rows, cols = lst[1].split(',').map!{|n| Integer(n)}
    [idx [rows, cols]]
  end
  def set_spans(*pairs)
    # idx, val, idx, val, ...
    args = []
    0.step(pairs.size-1, 2){|i|
      args << tagid(pairs[i])
      val = pairs[i+1]
      if val.kind_of?(Array)
        args << val.join(',')
      else
        args << val
      end
    }
    tk_send('spans', *args)
    self
  end
=begin
  def set_spans(*pairs)
    if pairs[0].kind_of?(Array)
      # [idx, val], [idx, val], ...
      args = []
      pairs.each{|idx, val|
        args << tagid(idx)
        if val.kind_of?(Array)
          args << val.join(',')
        else
          args << val
        end
      }
      tk_send('spans', *args)
    else
      # idx, val, idx, val, ...
      args = []
      0.step(pairs.size-1, 2){|i|
        args << tagid(pairs[i])
        val = pairs[i+1]
        if val.kind_of?(Array)
          args << val.join(',')
        else
          args << val
        end
      }
      tk_send('spans', *args)
    end
    self
  end
=end

  def tagid(tag)
    if tag.kind_of?(Tk::TkTable::CellTag)
      tag.id
    elsif tag.kind_of?(Array)
      if tag[0].kind_of?(Integer) && tag[1].kind_of?(Integer)
        # [row, col]
        tag.join(',')
      else
        tag
      end
    else
      tag
    end
  end

  def tagid2obj(tagid)
    Tk::TkTable::CellTag::CellTagID_TBL.mutex.synchronize{
      if Tk::TkTable::CellTag::CellTagID_TBL.key?(@path)
        if Tk::TkTable::CellTag::CellTagID_TBL[@path].key?(tagid)
          Tk::TkTable::CellTag::CellTagID_TBL[@path][tagid]
        else
          tagid
        end
      else
        tagid
      end
    }
  end

  def tag_cell(tag, *cells)
    tk_send('tag', 'cell', tagid(tag), *(cells.collect{|idx| tagid(idx)}))
    self
  end
  def tag_reset(*cells)
    tk_send('tag', 'cell', '', *(cells.collect{|idx| tagid(idx)}))
    self
  end
  def tag_col(tag, *cols)
    tk_send('tag', 'col', tagid(tag), *cols)
    self
  end
  def tag_col_reset(*cols)
    tk_send('tag', 'col', '', *cols)
    self
  end
  def tag_delete(tag)
    tk_send('tag', 'delete', tagid(tag))
    Tk::TkTable::CellTag::CellTagID_TBL.mutex.synchronize{
      if Tk::TkTable::CellTag::CellTagID_TBL[@path]
        if tag.kind_of? Tk::TkTable::CellTag
          Tk::TkTable::CellTag::CellTagID_TBL[@path].delete(tag.id)
        else
          Tk::TkTable::CellTag::CellTagID_TBL[@path].delete(tag)
        end
      end
    }
    self
  end
  def tag_exist?(tag)
    bool(tk_send('tag', 'exists', tagid(tag)))
  end
  def tag_include?(tag, idx)
    bool(tk_send('tag', 'includes', tagid(tag), tagid(idx)))
  end
  def tag_lower(tag, target=None)
    tk_send('tag', 'lower', tagid(tag), tagid(target))
    self
  end
  def tag_names(pat=None)
    simplelist(tk_send('tag', 'names', pat)).collect{|tag| tagid2obj(tag)}
  end
  def tag_raise(tag, target=None)
    tk_send('tag', 'raise', tagid(tag), tagid(target))
    self
  end
  def tag_row(tag, *rows)
    tk_send('tag', 'row', tagid(tag), *rows)
    self
  end
  def tag_row_reset(*rows)
    tk_send('tag', 'row', '', *rows)
    self
  end

  def validate(idx)
    bool(tk_send('validate', tagid(idx)))
  end

  def width_list
    list(tk_send('width'))
  end
  def width(row)
    number(tk_send('width', row))
  end
  def set_width(*pairs)
    tk_send('width', *(pairs.flatten))
    self
  end

  def window_delete(*args)
    tk_send('window', 'delete', *(args.collect{|idx| tagid(idx)}))
    self
  end
  def window_move(from_idx, to_idx)
    tk_send('window', 'move', tagid(from_idx), tagid(to_idx))
    self
  end
  def window_names(pat=None)
    simplelist(tk_send('window', 'names', pat))
  end
end
