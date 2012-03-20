#
#  tkextlib/tcllib/tablelist_core.rb
#
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * This file is required by 'tkextlib/tcllib/tablelist.rb' or
#     'tkextlib/tcllib/tablelist_tile.rb'.
#

module Tk
  module Tcllib
    class Tablelist < TkWindow
      if Tk::Tcllib::Tablelist_usingTile
        PACKAGE_NAME = 'Tablelist_tile'.freeze
      else
        PACKAGE_NAME = 'Tablelist'.freeze
      end
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require(self.package_name)
        rescue
          ''
        end
      end

      def self.use_Tile?
        (Tk::Tcllib::Tablelist_usingTile)? true: false
      end
    end
    TableList = Tablelist
  end
end

module Tk::Tcllib::TablelistItemConfig
  include TkItemConfigMethod

  def _to_idx(idx)
    if idx.kind_of?(Array)
      idx.collect{|elem| _get_eval_string(elem)}.join(',')
    else
      idx
    end
  end
  def _from_idx(idx)
    return idx unless idx.kind_of?(String)

    if idx[0] == ?@  # '@x,y'
      idx
    elsif idx =~ /([^,]+),([^,]+)/
      row = $1, column = $2
      [num_or_str(row), num_or_str(column)]
    else
      num_or_str(idx)
    end
  end
  private :_to_idx, :_from_idx

  def __item_cget_cmd(mixed_id)
    [self.path, mixed_id[0] + 'cget', _to_idx(mixed_id[1])]
  end
  def __item_config_cmd(mixed_id)
    [self.path, mixed_id[0] + 'configure', _to_idx(mixed_id[1])]
  end

  def cell_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['cell', tagOrId], option)
  end
  def cell_cget(tagOrId, option)
    itemcget(['cell', tagOrId], option)
  end
  def cell_cget_strict(tagOrId, option)
    itemcget_strict(['cell', tagOrId], option)
  end
  def cell_configure(tagOrId, slot, value=None)
    itemconfigure(['cell', tagOrId], slot, value)
  end
  def cell_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['cell', tagOrId], slot)
  end
  def current_cell_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['cell', tagOrId], slot)
  end
  alias cellcget_tkstring cell_cget_tkstring
  alias cellcget cell_cget
  alias cellcget_strict cell_cget_strict
  alias cellconfigure cell_configure
  alias cellconfiginfo cell_configinfo
  alias current_cellconfiginfo current_cell_configinfo

  def column_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['column', tagOrId], option)
  end
  def column_cget(tagOrId, option)
    itemcget(['column', tagOrId], option)
  end
  def column_cget_strict(tagOrId, option)
    itemcget_strict(['column', tagOrId], option)
  end
  def column_configure(tagOrId, slot, value=None)
    itemconfigure(['column', tagOrId], slot, value)
  end
  def column_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['column', tagOrId], slot)
  end
  def current_column_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['column', tagOrId], slot)
  end
  alias columncget_tkstring column_cget_tkstring
  alias columncget column_cget
  alias columncget_strict column_cget_strict
  alias columnconfigure column_configure
  alias columnconfiginfo column_configinfo
  alias current_columnconfiginfo current_column_configinfo

  def row_cget_tkstring(tagOrId, option)
    itemcget_tkstring(['row', tagOrId], option)
  end
  def row_cget(tagOrId, option)
    itemcget(['row', tagOrId], option)
  end
  def row_cget_strict(tagOrId, option)
    itemcget_strict(['row', tagOrId], option)
  end
  def row_configure(tagOrId, slot, value=None)
    itemconfigure(['row', tagOrId], slot, value)
  end
  def row_configinfo(tagOrId, slot=nil)
    itemconfiginfo(['row', tagOrId], slot)
  end
  def current_row_configinfo(tagOrId, slot=nil)
    current_itemconfiginfo(['row', tagOrId], slot)
  end
  alias rowcget_tkstring row_cget_tkstring
  alias rowcget row_cget
  alias rowcget_strict row_cget_strict
  alias rowconfigure row_configure
  alias rowconfiginfo row_configinfo
  alias current_rowconfiginfo current_row_configinfo

  private :itemcget_tkstring, :itemcget, :itemcget_strict
  private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo
end

class Tk::Tcllib::Tablelist
  include Tk::Tcllib::TablelistItemConfig
  include Scrollable

  TkCommandNames = ['::tablelist::tablelist'.freeze].freeze
  WidgetClassName = 'Tablelist'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  ##########################

  def __numval_optkeys
    super() + ['titlecolumns']
  end
  private :__numval_optkeys

  def __strval_optkeys
    super() + ['snipstring']
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() - ['takefocus'] + [
      'forceeditendcommand', 'movablecolumns', 'movablerows',
      'protecttitlecolumns', 'resizablecolumns', 'setfocus',
      'showarrow', 'showlabels', 'showseparators'
    ]
  end
  private :__boolval_optkeys

  def __listval_optkeys
    super() + ['columns', 'columntitles']
  end
  private :__listval_optkeys

  def __tkvariable_optkeys
    super() + ['listvariable']
  end
  private :__tkvariable_optkeys

  def __val2ruby_optkeys  # { key=>proc, ... }
    # The method is used to convert a opt-value to a ruby's object.
    # When get the value of the option "key", "proc.call(value)" is called.
    super().update('stretch'=>proc{|v|
                     (v == 'all')? v: simplelist(v)
                   },
                   'takefocus'=>proc{|v|
                     case v
                     when '1'
                       true
                     when '0'
                       false
                     when ''
                       nil
                     else # cmd
                       tk_tcl2ruby(cmd)
                       end
                   })
  end
  private :__val2ruby_optkeys

  def __ruby2val_optkeys  # { key=>proc, ... }
    # The method is used to convert a ruby's object to a opt-value.
    # When set the value of the option "key", "proc.call(value)" is called.
    # That is, "-#{key} #{proc.call(value)}".
    super().update('stretch'=>proc{|v|
                     (v.kind_of?(Array))? v.collect{|e| _to_idx(e)}: v
                   },
                   'takefocus'=>proc{|v|
                     case v
                     when true
                       '1'
                     when false
                       '0'
                     when nil
                       ''
                     else
                       _get_eval_string(v)
                     end
                   })
  end
  private :__ruby2val_optkeys

  def __font_optkeys
    super() + ['labelfont']
  end
  private :__font_optkeys

  ##########################

  def __item_strval_optkeys(id)
    if id[0] == 'cell'
      super(id) + ['title']
    else
      super(id) - ['text'] + ['title', 'name']
    end
  end
  private :__item_strval_optkeys

  def __item_boolval_optkeys(id)
    super(id) + [
      'changesnipside', 'editable', 'hide', 'resizable', 'selectable',
      'showarrow', 'showlinenumbers', 'stretchable', 'stretchwindow', 'wrap'
    ]
  end
  private :__item_boolval_optkeys

  def __item_listval_optkeys(id)
    if id[0] == 'cell'
      super(id)
    else
      super(id) + ['text']
    end
  end
  private :__item_listval_optkeys

  def __item_font_optkeys(id)
    # maybe need to override
    super(id) + ['labelfont']
  end
  private :__item_font_optkeys

  ##########################

  def activate(index)
    tk_send('activate', _to_idx(index))
    self
  end

  def activate_cell(index)
    tk_send('activatecell', _to_idx(index))
    self
  end
  alias activatecell activate_cell

  def get_attrib(name=nil)
    if name && name != None
      tk_send('attrib', name)
    else
      ret = []
      lst = simplelist(tk_send('attrib'))
      until lst.empty?
        ret << ( [lst.shift] << lst.shift )
      end
      ret
    end
  end
  def set_attrib(*args)
    tk_send('attrib', *(args.flatten))
    self
  end

  def bbox(index)
    list(tk_send('bbox', _to_idx(index)))
  end

  def bodypath
    window(tk_send('bodypath'))
  end

  def bodytag
    TkBindTag.new_by_name(tk_send('bodytag'))
  end

  def cancel_editing
    tk_send('cancelediting')
    self
  end
  alias cancelediting cancel_editing

  def get_cellattrib(name=nil)
    if name && name != None
      tk_send('cellattrib', name)
    else
      ret = []
      lst = simplelist(tk_send('cellattrib'))
      until lst.empty?
        ret << ( [lst.shift] << lst.shift )
      end
      ret
    end
  end
  def set_cellattrib(*args)
    tk_send('cellattrib', *(args.flatten))
    self
  end

  def cellindex(idx)
    _from_idx(tk_send('cellindex', _to_idx(idx)))
  end

  def cellselection_anchor(idx)
    tk_send('cellselection', 'anchor', _to_idx(idx))
    self
  end

  def cellselection_clear(first, last=nil)
    if first.kind_of?(Array)
      tk_send('cellselection', 'clear', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('cellselection', 'clear', first, last)
    end
    self
  end

  def cellselection_includes(idx)
    bool(tk_send('cellselection', 'includes', _to_idx(idx)))
  end

  def cellselection_set(first, last=nil)
    if first.kind_of?(Array)
      tk_send('cellselection', 'set', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('cellselection', 'set', first, last)
    end
    self
  end

  def get_columnattrib(name=nil)
    if name && name != None
      tk_send('columnattrib', name)
    else
      ret = []
      lst = simplelist(tk_send('columnattrib'))
      until lst.empty?
        ret << ( [lst.shift] << lst.shift )
      end
      ret
    end
  end
  def set_columnattrib(*args)
    tk_send('columnattrib', *(args.flatten))
    self
  end

  def columncount
    number(tk_send('columncount'))
  end

  def columnindex(idx)
    number(tk_send('columnindex', _to_idx(idx)))
  end

  def columnwidth(idx, opt=nil)
    if opt
      number(tk_send('columnwidth', _to_idx(idx), "-#{opt}"))
    else
      number(tk_send('columnwidth', _to_idx(idx)))
    end
  end
  def requested_columnwidth(idx)
    columnwidth(idx, 'requested')
  end
  def stretched_columnwidth(idx)
    columnwidth(idx, 'stretched')
  end
  def total_columnwidth(idx)
    columnwidth(idx, 'total')
  end

  def configcelllist(lst) # lst ==> [idx, opt, val, idx, opt, val, ...]
    ary = []
    lst.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configcelllist', ary)
    self
  end
  alias config_celllist configcelllist

  def configcells(*args) # args ==> idx, opt, val, idx, opt, val, ...
    ary = []
    args.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configcells', *ary)
    self
  end
  alias config_cells configcells

  def configcolumnlist(lst) # lst ==> [idx, opt, val, idx, opt, val, ...]
    ary = []
    lst.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configcolumnlist', ary)
    self
  end
  alias config_columnlist configcolumnlist

  def configcolumns(*args) # args ==> idx, opt, val, idx, opt, val, ...
    ary = []
    args.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configcolumns', *ary)
    self
  end
  alias config_columns configcolumns

  def configrowlist(lst) # lst ==> [idx, opt, val, idx, opt, val, ...]
    ary = []
    lst.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configrowlist', ary)
    self
  end
  alias config_rowlist configrowlist

  def configrows(*args) # args ==> idx, opt, val, idx, opt, val, ...
    ary = []
    args.slice(3){|idx, opt, val|
      ary << _to_idx(idx) << "-#{opt}" << val
    }
    tk_send('configrows', *ary)
    self
  end
  alias config_rows configrows

  def containing(y)
    idx = num_or_str(tk_send('containing', y))
    (idx.kind_of?(Fixnum) && idx < 0)?  nil: idx
  end

  def containing_cell(x, y)
    idx = _from_idx(tk_send('containingcell', x, y))
    if idx.kind_of?(Array)
      [
        ((idx[0].kind_of?(Fixnum) && idx[0] < 0)?  nil: idx[0]),
        ((idx[1].kind_of?(Fixnum) && idx[1] < 0)?  nil: idx[1])
      ]
    else
      idx
    end
  end
  alias containingcell containing_cell

  def containing_column(x)
    idx = num_or_str(tk_send('containingcolumn', x))
    (idx.kind_of?(Fixnum) && idx < 0)?  nil: idx
  end
  alias containingcolumn containing_column

  def curcellselection
    simplelist(tk_send('curcellselection')).collect!{|idx| _from_idx(idx)}
  end

  def curselection
    list(tk_send('curselection'))
  end

  def delete_items(first, last=nil)
    if first.kind_of?(Array)
      tk_send('delete', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('delete', first, last)
    end
    self
  end
  alias delete delete_items
  alias deleteitems delete_items

  def delete_columns(first, last=nil)
    if first.kind_of?(Array)
      tk_send('deletecolumns', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('deletecolumns', first, last)
    end
    self
  end
  alias deletecolumns delete_columns

  def edit_cell(idx)
    tk_send('editcell', _to_idx(idx))
    self
  end
  alias editcell edit_cell

  def editwintag
    TkBindTag.new_by_name(tk_send('editwintag'))
  end

  def editwinpath
    window(tk_send('editwinpath'))
  end

  def entrypath
    window(tk_send('entrypath'))
  end

  def fill_column(idx, txt)
    tk_send('fillcolumn', _to_idx(idx), txt)
    self
  end
  alias fillcolumn fill_column

  def finish_editing
    tk_send('finishediting')
    self
  end
  alias finishediting finish_editing

  def formatinfo
    key, row, col = simplelist(tk_send('formatinfo'))
    [key, number(row), number(col)]
  end

  def get(first, last=nil)
    if first.kind_of?(Array)
      simplelist(tk_send('get', first.collect{|idx| _to_idx(idx)})).collect!{|elem| simplelist(elem) }
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      simplelist(tk_send('get', first, last))
    end
  end

  def get_cells(first, last=nil)
    if first.kind_of?(Array)
      simplelist(tk_send('getcells', first.collect{|idx| _to_idx(idx)})).collect!{|elem| simplelist(elem) }
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      simplelist(tk_send('getcells', first, last))
    end
  end
  alias getcells get_cells

  def get_columns(first, last=nil)
    if first.kind_of?(Array)
      simplelist(tk_send('getcolumns', first.collect{|idx| _to_idx(idx)})).collect!{|elem| simplelist(elem) }
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      simplelist(tk_send('getcolumns', first, last))
    end
  end
  alias getcolumns get_columns

  def get_keys(first, last=nil)
    if first.kind_of?(Array)
      simplelist(tk_send('getkeys', first.collect{|idx| _to_idx(idx)})).collect!{|elem| simplelist(elem) }
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      simplelist(tk_send('getkeys', first, last))
    end
  end
  alias getkeys get_keys

  def has_attrib?(name)
    bool(tk_send('hasattrib', name))
  end

  def has_cellattrib?(idx, name)
    bool(tk_send('hascellattrib', _to_idx(idx), name))
  end

  def has_columnattrib?(idx, name)
    bool(tk_send('hascolumnattrib', _to_idx(idx), name))
  end

  def has_rowattrib?(idx, name)
    bool(tk_send('hasrowattrib', _to_idx(idx), name))
  end

  def imagelabelpath(idx)
    window(tk_send('imagelabelpath', _to_idx(idx)))
  end

  def index(idx)
    number(tk_send('index', _to_idx(idx)))
  end

  def insert(idx, *items)
    tk_send('insert', _to_idx(idx), *items)
    self
  end

  def insert_columnlist(idx, columnlist)
    tk_send('insertcolumnlist', _to_idx(idx), columnlist)
    self
  end
  alias insertcolumnlist insert_columnlist

  def insert_columns(idx, *args)
    tk_send('insertcolums', _to_idx(idx), *args)
    self
  end
  alias insertcolumns insert_columns

  def insert_list(idx, list)
    tk_send('insertlist', _to_idx(idx), list)
    self
  end
  alias insertlist insert_list

  def is_elem_snipped?(cellidx, tkvar)
    bool(tk_send('iselemsnipped', _to_idx(cellidx), tkvar))
  end
  alias elem_snipped? is_elem_snipped?

  def is_title_snipped?(colidx, tkvar)
    bool(tk_send('istitlesnipped', _to_idx(colidx), tkvar))
  end
  alias title_snipped? is_title_snipped?

  def itemlistvar
    TkVarAccess.new(tk_send('itemlistvar'))
  end

  def labelpath(idx)
    window(tk_send('labelpath', _to_idx(idx)))
  end

  def labels
    simplelist(tk_send('labels'))
  end

  def labeltag
    TkBindTag.new_by_name(tk_send('labeltag'))
  end

  def move(src, target)
    tk_send('move', _to_idx(src), _to_idx(target))
    self
  end

  def move_column(src, target)
    tk_send('movecolumn', _to_idx(src), _to_idx(target))
    self
  end
  alias movecolumn move_column

  def nearest(y)
    _from_idx(tk_send('nearest', y))
  end

  def nearest_cell(x, y)
    _from_idx(tk_send('nearestcell', x, y))
  end
  alias nearestcell nearest_cell

  def nearest_column(x)
    _from_idx(tk_send('nearestcolumn', x))
  end
  alias nearestcolumn nearest_column

  def reject_input
    tk_send('rejectinput')
    self
  end
  alias rejectinput reject_input

  def reset_sortinfo
    tk_send('resetsortinfo')
    self
  end
  alias resetsortinfo reset_sortinfo

  def get_rowattrib(name=nil)
    if name && name != None
      tk_send('rowattrib', name)
    else
      ret = []
      lst = simplelist(tk_send('rowattrib'))
      until lst.empty?
        ret << ( [lst.shift] << lst.shift )
      end
      ret
    end
  end
  def set_rowattrib(*args)
    tk_send('rowattrib', *(args.flatten))
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
    tk_send('see', _to_idx(idx))
    self
  end

  def see_cell(idx)
    tk_send('seecell', _to_idx(idx))
    self
  end
  alias seecell see_cell

  def see_column(idx)
    tk_send('seecolumn', _to_idx(idx))
    self
  end
  alias seecolumn see_column

  def selection_anchor(idx)
    tk_send('selection', 'anchor', _to_idx(idx))
    self
  end

  def selection_clear(first, last=nil)
    if first.kind_of?(Array)
      tk_send('selection', 'clear', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('selection', 'clear', first, last)
    end
    self
  end

  def selection_includes(idx)
    bool(tk_send('selection', 'includes', _to_idx(idx)))
  end

  def selection_set(first, last=nil)
    if first.kind_of?(Array)
      tk_send('selection', 'set', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('selection', 'set', first, last)
    end
    self
  end

  def separatorpath(idx=nil)
    if idx
      window(tk_send('separatorpath', _to_idx(idx)))
    else
      window(tk_send('separatorpath'))
    end
  end

  def separators
    simplelist(tk_send('separators')).collect!{|w| window(w)}
  end

  def size
    number(tk_send('size'))
  end

  def sort(order=nil)
    if order
      order = order.to_s
      order = '-' << order if order[0] != ?-
      if order.length < 2
        order = nil
      end
    end
    if order
      tk_send('sort', order)
    else
      tk_send('sort')
    end
    self
  end
  def sort_increasing
    tk_send('sort', '-increasing')
    self
  end
  def sort_decreasing
    tk_send('sort', '-decreasing')
    self
  end


  # default of 'labelcommand' option
  DEFAULT_labelcommand_value =
    DEFAULT_sortByColumn_cmd = '::tablelist::sortByColumn'

  # default of 'labelcommand2' option
  DEFAULT_labelcommand2_value =
    DEFAULT_addToSortColumns_cmd = '::tablelist::addToSortColumns'

  def sortByColumn_with_event_generate(idx)
    tk_call('::tablelist::sortByColumn', @path, _to_idx(idx))
  end

  def addToSortColumns_with_event_generate(idx)
    tk_call('::tablelist::addToSortColumns', @path, _to_idx(idx))
  end

  def sort_by_column(idx, order=nil)
    if order
      order = order.to_s
      order = '-' << order if order[0] != ?-
      if order.length < 2
        order = nil
      end
    end
    if order
      tk_send('sortbycolumn', _to_idx(idx), order)
    else
      tk_send('sortbycolumn', _to_idx(idx))
    end
    self
  end
  def sort_by_column_increasing(idx)
    tk_send('sortbycolumn', _to_idx(idx), '-increasing')
    self
  end
  def sort_by_column_decreasing(idx)
    tk_send('sortbycolumn', _to_idx(idx), '-decreasing')
    self
  end

  def sort_by_columnlist(idxlist, orderlist=None)
    # orderlist :: list of 'increasing' or 'decreasing'
    tk_send('sortbycolumnlist', idxlist.map{|idx| _to_idx(idx)}, orderlist)
    self
  end

  def sortcolumn
    idx = num_or_str(tk_send('sortcolum'))
    (idx.kind_of?(Fixnum) && idx < 0)?  nil: idx
  end

  def sortcolumnlist
    simplelist(tk_send('sortcolumlist')).map{|col| num_or_str(col)}
  end

  def sortorder
    tk_send('sortorder')
  end

  def sortorderlist
    simplelist(tk_send('sortorderlist'))
  end

  def toggle_columnhide(first, last=nil)
    if first.kind_of?(Array)
      tk_send('togglecolumnhide', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('togglecolumnhide', first, last)
    end
  end

  def toggle_rowhide(first, last=nil)
    if first.kind_of?(Array)
      tk_send('togglerowhide', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('togglerowhide', first, last)
    end
  end

  def toggle_visibility(first, last=nil)
    if first.kind_of?(Array)
      tk_send('togglevisibility', first.collect{|idx| _to_idx(idx)})
    else
      first = _to_idx(first)
      last = (last)? _to_idx(last): first
      tk_send('togglevisibility', first, last)
    end
    self
  end
  alias togglevisibility toggle_visibility

  def unset_attrib(name)
    tk_send('unsetattrib', name)
    self
  end

  def unset_cellattrib(idx, name)
    tk_send('unsetcellattrib', _to_idx(idx), name)
    self
  end

  def unset_columnattrib(idx, name)
    tk_send('unsetcolumnattrib', _to_idx(idx), name)
    self
  end

  def unset_rowattrib(idx, name)
    tk_send('unsetrowattrib', _to_idx(idx), name)
    self
  end

  def windowpath(idx)
    window(tk_send('windowpath', _to_idx(idx)))
  end
end

class << Tk::Tcllib::Tablelist
  ############################################################
  # helper commands
  def getTablelistPath(descendant)
    window(Tk.tk_call('::tablelist::getTablelistPath', descendant))
  end

  def getTablelistColumn(descendant)
    num_or_str(Tk.tk_call('::tablelist::getTablelistColumn', headerlabel))
  end


  def convEventFields(descendant, x, y)
    window(Tk.tk_call('::tablelist::convEventFields', descendant, x, y))
  end


  ############################################################
  # with the BWidget package
  def addBWidgetEntry(name=None)
    Tk.tk_call('::tablelist::addBWidgetEntry', name)
  end

  def addBWidgetSpinBox(name=None)
    Tk.tk_call('::tablelist::addBWidgetSpinBox', name)
  end

  def addBWidgetComboBox(name=None)
    Tk.tk_call('::tablelist::addBWidgetComboBox', name)
  end


  ############################################################
  # with the Iwidgets ([incr Widgets]) package
  def addIncrEntryfield(name=None)
    Tk.tk_call('::tablelist::addIncrEntry', name)
  end

  def addIncrDateTimeWidget(type, seconds=false, name=None)
    # type := 'datefield'|'dateentry'|timefield'|'timeentry'
    if seconds && seconds != None
      seconds = '-seconds'
    else
      seconds = None
    end
    Tk.tk_call('::tablelist::addDateTimeWidget', type, seconds, name)
  end

  def addIncrSpinner(name=None)
    Tk.tk_call('::tablelist::addIncrSpinner', name)
  end

  def addIncrSpinint(name=None)
    Tk.tk_call('::tablelist::addIncrSpinint', name)
  end

  def addIncrCombobox(name=None)
    Tk.tk_call('::tablelist::addIncrCombobox', name)
  end


  ############################################################
  # with Bryan Oakley's combobox package
  def addOakleyCombobox(name=None)
    Tk.tk_call('::tablelist::addOakleyCombobox', name)
  end

  ############################################################
  # with the multi-entry package Mentry is a library extension
  def addDateMentry(format, separator, gmt=false, name=None)
    if gmt && gmt != None
      gmt = '-gmt'
    else
      gmt = None
    end
    Tk.tk_call('::tablelist::addDateMentry', format, separator, gmt, name)
  end

  def addTimeMentry(format, separator, gmt=false, name=None)
    if gmt && gmt != None
      gmt = '-gmt'
    else
      gmt = None
    end
    Tk.tk_call('::tablelist::addTimeMentry', format, separator, gmt, name)
  end

  def addDateTimeMentry(format, date_sep, time_sep, gmt=false, name=None)
    if gmt && gmt != None
      gmt = '-gmt'
    else
      gmt = None
    end
    Tk.tk_call('::tablelist::addDateTimeMentry',
               format, date_sep, time_sep, gmt, name)
  end

  def addFixedPointMentry(count1, count2, comma=false, name=None)
    if comma && comma != None
      comma = '-comma'
    else
      comma = None
    end
    Tk.tk_call('::tablelist::addFixedPoingMentry', count1, count2, comma, name)
  end

  def addIPAddrMentry(name=None)
    Tk.tk_call('::tablelist::addIPAddrMentry', name)
  end
end
