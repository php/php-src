#
#  tkextlib/blt/tabset.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Tabset < TkWindow
    class Tab < TkObject
      include TkTreatItemFont

      TabID_TBL = TkCore::INTERP.create_table

      (TabsetTab_ID = ['blt_tabset_tab'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      TkCore::INTERP.init_ip_env{
        TabID_TBL.mutex.synchronize{ TabID_TBL.clear }
      }

      def self.id2obj(tabset, id)
        tpath = tabset.path
        TabID_TBL.mutex.synchronize{
          if TabID_TBL[tpath]
            TabID_TBL[tpath][id]? TabID_TBL[tpath][id]: id
          else
            id
          end
        }
      end

      def self.new(parent, pos=nil, name=nil, keys={})
        if pos.kind_of?(Hash)
          keys = pos
          name = nil
          pos  = nil
        end
        if name.kind_of?(Hash)
          keys = name
          name = nil
        end
        obj = nil
        TabID_TBL.mutex.synchronize{
          if name && TabID_TBL[parent.path] && TabID_TBL[parent.path][name]
            obj = TabID_TBL[parent.path][name]
            if pos
              if pos.to_s == 'end'
                obj.move_after('end')
              else
                obj.move_before(pos)
              end
            end
            obj.configure if keys && ! keys.empty?
          else
            (obj = self.allocate).instance_eval{
              initialize(parent, pos, name, keys)
              TabID_TBL[@tpath] = {} unless TabID_TBL[@tpath]
              TabID_TBL[@tpath][@id] = self
            }
          end
        }
        obj
      end

      def initialize(parent, pos, name, keys)
        @t = parent
        @tpath = parent.path
        if name
          @path = @id = name
          unless (list(tk_call(@tpath, 'tab', 'names', @id)).empty?)
            if pos
              idx = tk_call(@tpath, 'index', '-name', @id)
              if pos.to_s == 'end'
                tk_call(@tpath, 'move', idx, 'after', 'end')
              else
                tk_call(@tpath, 'move', idx, 'before', pos)
              end
            end
            tk_call(@tpath, 'tab', 'configure', @id, keys)
          else
            pos = 'end' unless pos
            tk_call(@tpath, 'insert', pos, @id, keys)
          end
        else
          pos = 'end' unless pos
          TabsetTab_ID.mutex.synchronize{
            @path = @id = TabsetTab_ID.join(TkCore::INTERP._ip_id_)
            TabsetTab_ID[1].succ!
          }
          tk_call(@tpath, 'insert', pos, @id, keys)
        end
      end

      #def bind(context, cmd=Proc.new, *args)
      #  @t.tab_bind(@id, context, cmd, *args)
      #  self
      #end
      def bind(context, *args)
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end
        @t.tab_bind(@id, context, cmd, *args)
        self
      end
      #def bind_append(context, cmd=Proc.new, *args)
      #  @t.tab_bind_append(@id, context, cmd, *args)
      #  self
      #end
      def bind_append(context, *args)
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end
        @t.tab_bind_append(@id, context, cmd, *args)
        self
      end
      def bind_remove(context)
        @t.tab_bind_remove(@id, context)
        self
      end
      def bindinfo(context=nil)
        @t.tab_bindinfo(@id, context)
      end

      def cget_tkstring(*args)
        @t.tab_cget_tkstring(@id, *args)
      end
      def cget(*args)
        @t.tab_cget(@id, *args)
      end
      def cget_strict(*args)
        @t.tab_cget_strict(@id, *args)
      end
      def configure(*args)
        @t.tab_configure(@id, *args)
      end
      def configinfo(*args)
        @t.tab_configinfo(@id, *args)
      end
      def current_configinfo(*args)
        @t.current_tab_configinfo(@id, *args)
      end

      def delete()
        @t.delete(@id)
        TabID_TBL.mutex.synchronize{
          TabID_TBL[@tpath].delete(@id)
        }
        self
      end

      def get_name()
        @id.dup
      end

      def focus()
        @t.focus(self.index)
      end

      def index()
        @t.index_name(@id)
      end

      def invoke()
        @t.invoke(self.index)
      end

      def move_before(idx)
        @t.move_before(self.index, idx)
      end
      def move_after(idx)
        @t.move_after(self.index, idx)
      end

      def perforation_highlight(mode)
        @t.perforation_highlight(self.index, mode)
      end
      def perforation_invoke()
        @t.perforation_invoke(self.index)
      end

      def see()
        @t.see(self.index)
      end

      def tearoff(name=None)
        @t.tab_tearoff(self.index, *args)
      end
    end

    ########################################

    class NamedTab < Tab
      def self.new(parent, name)
        super(parent, nil, name, {})
      end
    end

    ########################################

    include X_Scrollable
    include TkItemConfigMethod

    TkCommandNames = ['::blt::tabset'.freeze].freeze
    WidgetClassName = 'Tabset'.freeze
    WidgetClassNames[WidgetClassName] ||= self

    def __destroy_hook__
      Tk::BLT::Tabset::Tab::TabID_TBL.mutex.synchronize{
        Tk::BLT::Tabset::Tab::TabID_TBL.delete(@path)
      }
    end

    ########################################

    def __boolval_optkeys
      super() << 'samewidth' << 'tearoff'
    end
    private :__strval_optkeys

    def __strval_optkeys
      super() << 'tabbackground' << 'tabforeground'
    end
    private :__strval_optkeys

    def __item_cget_cmd(id)
      [self.path, 'tab', 'cget', id]
    end
    private :__item_cget_cmd

    def __item_config_cmd(id)
      [self.path, 'tab', 'configure', id]
    end
    private :__item_config_cmd

    def __item_pathname(tagOrId)
      if tagOrId.kind_of?(Tk::BLT::Tabset::Tab)
        self.path + ';' + tagOrId.id.to_s
      else
        self.path + ';' + tagOrId.to_s
      end
    end
    private :__item_pathname

    alias tab_cget_tkstring itemcget_tkstring
    alias tab_cget itemcget
    alias tab_cget_strict itemcget_strict
    alias tab_configure itemconfigure
    alias tab_configinfo itemconfiginfo
    alias current_tab_configinfo current_itemconfiginfo

    def __item_strval_optkeys(id)
      super(id) << 'shadow'
    end
    private :__item_strval_optkeys

    def tagid(tab)
      if tab.kind_of?(Tk::BLT::Tabset::Tab)
        tab.id
      else
        tab
      end
    end

    def tagindex(tab)
      if tab.kind_of?(Tk::BLT::Tabset::Tab)
        tab.index
      else
        tab
      end
    end

    ########################################

    def activate(index)
      tk_send('activate', tagindex(index))
      self
    end
    alias highlight activate

    #def tabbind(tag, context, cmd=Proc.new, *args)
    #  _bind([path, "bind", tagid(tag)], context, cmd, *args)
    #  self
    #end
    def tabbind(tag, context, *args)
      # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, "bind", tagid(tag)], context, cmd, *args)
      self
    end
    #def tabbind_append(tag, context, cmd=Proc.new, *args)
    #  _bind_append([path, "bind", tagid(tag)], context, cmd, *args)
    #  self
    #end
    def tabbind_append(tag, context, *args)
      # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, "bind", tagid(tag)], context, cmd, *args)
      self
    end
    def tabbind_remove(tag, context)
      _bind_remove([path, "bind", tagid(tag)], context)
      self
    end
    def tabbindinfo(tag, context=nil)
      _bindinfo([path, "bind", tagid(tag)], context)
    end

    def delete(first, last=None)
      tk_send('delete', tagindex(first), tagindex(last))
      if first.kind_of?(Tk::BLT::Tabset::Tab)
        TabID_TBL.mutex.synchronize{
          TabID_TBL[@path].delete(first.id)
        }
      end
      # middle tabs of the range are unknown
      if last.kind_of?(Tk::BLT::Tabset::Tab)
        TabID_TBL.mutex.synchronize{
          TabID_TBL[@path].delete(last.id)
        }
      end
      self
    end

    def focus(index)
      tk_send('focus', tagindex(index))
      self
    end

    def get_tab(index)
      if (idx = tk_send_without_enc('get', tagindex(index))).empty?
        nil
      else
        Tk::BLT::Tabset::Tab.id2obj(self, idx)
      end
    end
    def get_tabobj(index)
      if (idx = tk_send_without_enc('get', tagindex(index))).empty?
        nil
      else
       Tk::BLT::Tabset::Tab.new(self, nil, name, {})
      end
    end

    def index(str)
      num_or_str(tk_send('index', str))
    end
    def index_name(tab)
      num_or_str(tk_send('index', '-name', tagid(tab)))
    end

    def insert(pos, tab, keys={})
      pos = 'end' if pos.nil?
      Tk::BLT::Tabset::Tab.new(self, tagindex(pos), tagid(tab), keys)
    end
    def insert_tabs(pos, *tabs)
      pos = 'end' if pos.nil?
      if tabs[-1].kind_of?(Hash)
        keys = tabs.pop
      else
        keys = {}
      end
      fail ArgumentError, 'no tabs is given' if tabs.empty?
      tabs.map!{|tab| tagid(tab)}
      tk_send('insert', tagindex(pos), *(tabs + [keys]))
      tabs.collect{|tab| Tk::BLT::Tabset::Tab.new(self, nil, tagid(tab))}
    end

    def invoke(index)
      tk_send('invoke', tagindex(index))
    end

    def move_before(index, base_idx)
      tk_send('move', tagindex(index), 'before', tagindex(base_idx))
      self
    end
    def move_after(index, base_idx)
      tk_send('move', tagindex(index), 'after', tagindex(base_idx))
      self
    end

    def nearest(x, y)
      Tk::BLT::Tabset::Tab.id2obj(self, num_or_str(tk_send_without_enc('nearest', x, y)))
    end

    def perforation_activate(mode)
      tk_send('perforation', 'activate', mode)
      self
    end
    def perforation_highlight(index, *args)
      if args.empty?
        # index --> mode
        tk_send('perforation', 'highlight', index)
      elsif args.size == 1
        # args[0] --> mode
        tk_send('perforation', 'highlight', tagindex(index), args[0])
      else # Error: call to get Tcl's error message
        tk_send('perforation', 'highlight', tagindex(index), *args)
      end
      self
    end
    def perforation_invoke(index=nil)
      if index
        tk_send('perforation', 'invoke', tagindex(index))
      else
        tk_send('perforation', 'invoke')
      end
    end

    def scan_mark(x, y)
      tk_send_without_enc('scan', 'mark', x, y)
      self
    end
    def scan_dragto(x, y)
      tk_send_without_enc('scan', 'dragto', x, y)
      self
    end

    def see(index)
      tk_send('see', tagindex(index))
      self
    end

    def size()
      number(tk_send_without_enc('size'))
    end

    def select(index)
      tk_send('select', tagindex(index))
      self
    end

    def tab_dockall
      tk_send('tab', 'dockall')
      self
    end

    def tab_names(pat=None)
      simplelist(tk_send('tab', 'names', pat)).collect{|name|
        Tk::BLT::Tabset::Tab.id2obj(self, name)
      }
    end

    def tab_objs(pat=None)
      simplelist(tk_send('tab', 'names', pat)).collect{|name|
        Tk::BLT::Tabset::Tab.new(self, nil, name, {})
      }
    end

    def tab_ids(pat=None)
      simplelist(tk_send('tab', 'names', pat))
    end

    def tab_pageheight
      number(tk_send('tab', 'pageheight'))
    end

    def tab_pagewidth
      number(tk_send('tab', 'pagewidth'))
    end

    def tab_tearoff(index, parent=None)
      window(tk_send('tab', 'tearoff', tagindex(index), parent))
    end

    def xscrollcommand(cmd=Proc.new)
      configure_cmd 'scrollcommand', cmd
      self
    end
    alias scrollcommand xscrollcommand

    def xview(*index)
      if index.empty?
        list(tk_send_without_enc('view'))
      else
        tk_send_without_enc('view', *index)
        self
      end
    end
    alias view xview
    alias view_moveto xview_moveto
    alias view_scroll xview_scroll

    alias scrollbar xscrollbar
  end
end
