#
#  tkextlib/blt/table.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/itemconfig.rb'
require 'tkextlib/blt.rb'

module Tk::BLT
  module Table
    include Tk
    extend Tk
    extend TkItemConfigMethod

    TkCommandNames = ['::blt::table'.freeze].freeze

    module TableContainer
      def blt_table_add(*args)
        Tk::BLT::Table.add(self, *args)
        self
      end

      def blt_table_arrange()
        Tk::BLT::Table.arrange(self)
        self
      end

      def blt_table_cget_tkstring(*args)
        Tk::BLT::Table.cget_tkstring(self, *args)
      end
      def blt_table_cget(*args)
        Tk::BLT::Table.cget(self, *args)
      end
      def blt_table_cget_strict(*args)
        Tk::BLT::Table.cget_strict(self, *args)
      end

      def blt_table_configure(*args)
        Tk::BLT::Table.configure(self, *args)
        self
      end

      def blt_table_configinfo(*args)
        Tk::BLT::Table.configinfo(self, *args)
      end

      def blt_table_current_configinfo(*args)
        Tk::BLT::Table.current_configinfo(self, *args)
      end

      def blt_table_locate(x, y)
        Tk::BLT::Table.locate(self, x, y)
      end

      def blt_table_delete(*args)
        Tk::BLT::Table.delete(self, *args)
        self
      end

      def blt_table_extents(item)
        Tk::BLT::Table.extents(self, item)
      end

      def blt_table_insert(*args)
        Tk::BLT::Table.insert(self, *args)
        self
      end

      def blt_table_insert_before(*args)
        Tk::BLT::Table.insert_before(self, *args)
        self
      end

      def blt_table_insert_after(*args)
        Tk::BLT::Table.insert_after(self, *args)
        self
      end

      def blt_table_join(first, last)
        Tk::BLT::Table.join(self, first, last)
        self
      end

      def blt_table_save()
        Tk::BLT::Table.save(self)
      end

      def blt_table_search(*args)
        Tk::BLT::Table.search(self, *args)
      end

      def blt_table_split(*args)
        Tk::BLT::Table.split(self, *args)
        self
      end

      def blt_table_itemcget_tkstring(*args)
        Tk::BLT::Table.itemcget_tkstring(self, *args)
      end
      def blt_table_itemcget(*args)
        Tk::BLT::Table.itemcget(self, *args)
      end
      def blt_table_itemcget_strict(*args)
        Tk::BLT::Table.itemcget_strict(self, *args)
      end

      def blt_table_itemconfigure(*args)
        Tk::BLT::Table.itemconfigure(self, *args)
        self
      end

      def blt_table_itemconfiginfo(*args)
        Tk::BLT::Table.itemconfiginfo(self, *args)
      end

      def blt_table_current_itemconfiginfo(*args)
        Tk::BLT::Table.current_itemconfiginfo(self, *args)
      end

      def blt_table_iteminfo(item)
        Tk::BLT::Table.iteminfo(self, item)
      end
    end
  end
end


############################################
class << Tk::BLT::Table
  def __item_cget_cmd(id) # id := [ container, item ]
    win = (id[0].kind_of?(TkWindow))? id[0].path: id[0].to_s
    ['::blt::table', 'cget', win, id[1]]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id) # id := [ container, item, ... ]
    container, *items = id
    win = (container.kind_of?(TkWindow))? container.path: container.to_s
    ['::blt::table', 'configure', win, *items]
  end
  private :__item_config_cmd

  def __item_pathname(id)
    win = (id[0].kind_of?(TkWindow))? id[0].path: id[0].to_s
    win + ';'
  end
  private :__item_pathname

  alias __itemcget_tkstring itemcget_tkstring
  alias __itemcget itemcget
  alias __itemcget_strict itemcget_strict
  alias __itemconfigure itemconfigure
  alias __itemconfiginfo itemconfiginfo
  alias __current_itemconfiginfo current_itemconfiginfo

  private :__itemcget_tkstring, :__itemcget, :__itemcget_strict
  private :__itemconfigure, :__itemconfiginfo, :__current_itemconfiginfo

  def __boolval_optkeys
    super() << 'propagate'
  end
  private :__boolval_optkeys

  def tagid(tag)
    if tag.kind_of?(Array)
      case tag[0]
      when Integer
        # [row, col]
        tag.join(',')
      when :c, :C, 'c', 'C', :r, :R, 'r', 'R'
        # c0 or r1 or C*, and so on
        tag.collect{|elem| elem.to_s}.join('')
      else
        tag
      end
    elsif tag.kind_of?(TkWindow)
      _epath(tag)
    else
      tag
    end
  end

  def tagid2obj(tagid)
    tagid
  end

  ############################################

  def cget_tkstring(container, option)
    __itemcget_tkstring([container], option)
  end
  def cget(container, option)
    __itemcget([container], option)
  end
  def cget_strict(container, option)
    __itemcget_strict([container], option)
  end

  def configure(container, *args)
    __itemconfigure([container], *args)
  end

  def configinfo(container, *args)
    __itemconfiginfo([container], *args)
  end

  def current_configinfo(container, *args)
    __current_itemconfiginfo([container], *args)
  end

  def itemcget_tkstring(container, item, option)
    __itemcget_tkstring([container, tagid(item)], option)
  end
  def itemcget(container, item, option)
    __itemcget([container, tagid(item)], option)
  end
  def itemcget_strict(container, item, option)
    __itemcget_strict([container, tagid(item)], option)
  end

  def itemconfigure(container, *args)
    if args[-1].kind_of?(Hash)
      # container, item, item, ... , hash_optkeys
      keys = args.pop
      fail ArgumentError, 'no item is given' if args.empty?
      id = [container]
      args.each{|item| id << tagid(item)}
      __itemconfigure(id, keys)
    else
      # container, item, item, ... , option, value
      val = args.pop
      opt = args.pop
      fail ArgumentError, 'no item is given' if args.empty?
      id = [container]
      args.each{|item| id << tagid(item)}
      __itemconfigure(id, opt, val)
    end
    container
  end

  def itemconfiginfo(container, *args)
    slot = args[-1]
    if slot.kind_of?(String) || slot.kind_of?(Symbol)
      slot = slot.to_s
      if slot[0] == ?. || slot =~ /^\d+,\d+$/ || slot =~ /^(c|C|r|R)(\*|\d+)/
        #   widget     ||    row,col          ||    Ci or Ri
        slot = nil
      else
        # option
        slot = args.pop
      end
    else
      slot = nil
    end

    fail ArgumentError, 'no item is given' if args.empty?

    id = [container]
    args.each{|item| id << tagid(item)}
    __itemconfiginfo(id, slot)
  end

  def current_itemconfiginfo(container, *args)
    slot = args[-1]
    if slot.kind_of?(String) || slot.kind_of?(Symbol)
      slot = slot.to_s
      if slot[0] == ?. || slot =~ /^\d+,\d+$/ || slot =~ /^(c|C|r|R)(\*|\d+)/
        #   widget     ||    row,col          ||    Ci or Ri
        slot = nil
      else
        # option
        slot = args.pop
      end
    else
      slot = nil
    end

    fail ArgumentError, 'no item is given' if args.empty?

    id = [container]
    args.each{|item| id << tagid(item)}
    __current_itemconfiginfo(id, slot)
  end

  def info(container)
    ret = {}
    inf = list(tk_call('::blt::table', 'info', container))
    until inf.empty?
      opt = inf.slice!(0..1)
      ret[opt[1..-1]] = opt[1]
    end
    ret
  end

  def iteminfo(container, item)
    inf = list(tk_call('::blt::table', 'info', container, tagid(item)).chomp)

    ret = []
    until inf.empty? || (inf[0].kind_of?(String) && inf[0] =~ /^-/)
      ret << inf.shift
    end

    if inf.length > 1
      keys = {}
      while inf.length > 1
        opt = inf.slice!(0..1)
        keys[opt[0][1..-1]] = opt[1]
      end
      ret << keys
    end

    ret
  end

  ############################################

  def create_container(container)
    tk_call('::blt::table', container)
    begin
      class << container
        include Tk::BLT::Table::TableContainer
      end
    rescue
      warn('fail to include TableContainer methods (frozen object?)')
    end
    container
  end

  def add(container, *args)
    if args.empty?
      tk_call('::blt::table', container)
    else
      args = args.collect{|arg|
        if arg.kind_of?(TkWindow)
          _epath(arg)
        elsif arg.kind_of?(Array)  # index
          arg.join(',')
        else
          arg
        end
      }
      tk_call('::blt::table', container, *args)
    end
    container
  end

  def arrange(container)
    tk_call('::blt::table', 'arrange', container)
    container
  end

  def delete(container, *args)
    tk_call('::blt::table', 'delete', container, *args)
  end

  def extents(container, item)
    ret = []
    inf = list(tk_call('::blt::table', 'extents', container, item))
    ret << inf.slice!(0..4) until inf.empty?
    ret
  end

  def forget(*wins)
    wins = wins.collect{|win| _epath(win)}
    tk_call('::blt::table', 'forget', *wins)
  end

  def insert(container, *args)
    tk_call('::blt::table', 'insert', container, *args)
  end

  def insert_before(container, *args)
    tk_call('::blt::table', 'insert', container, '-before', *args)
  end

  def insert_after(container, *args)
    tk_call('::blt::table', 'insert', container, '-after', *args)
  end

  def join(container, first, last)
    tk_call('::blt::table', 'join', container, first, last)
  end

  def locate(container, x, y)
    tk_call('::blt::table', 'locate', container, x, y)
  end

  def containers(arg={})
    list(tk_call('::blt::table', 'containers', *hash_kv(arg)))
  end

  def containers_pattern(pat)
    list(tk_call('::blt::table', 'containers', '-pattern', pat))
  end

  def containers_slave(win)
    list(tk_call('::blt::table', 'containers', '-slave', win))
  end

  def save(container)
    tk_call('::blt::table', 'save', container)
  end

  def search(container, keys={})
    list(tk_call('::blt::table', 'containers', *hash_kv(keys)))
  end

  def split(container, *args)
    tk_call('::blt::table', 'split', container, *args)
  end
end
