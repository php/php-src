#
#  tkextlib/blt/dragdrop.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/itemconfig'
require 'tkextlib/blt.rb'

module Tk::BLT
  module DragDrop
    extend TkCore

    TkCommandNames = ['::blt::drag&drop'.freeze].freeze

    class Token < TkWindow
      WidgetClassName = 'DragDropToken'.freeze
      WidgetClassNames[WidgetClassName] ||= self

      def initialize(arg)
        if arg.kind_of?(Hash) # arg is a hash includes the widgetpath of token
          arg = _symbolkey2str(arg)
          install_win(nil, arg['widgetname'])
        else  # arg is a drag&drop source
          tk_call('::blt::drag&drop', 'source', arg)
          install_win(nil, tk_call('::blt::drag&drop', 'token', arg))
        end
      end
    end

    ###################################

    extend TkItemConfigMethod
    extend Tk::ValidateConfigure

    class << self
      def __item_config_cmd(id)  # id := ['source'|'target', win]
        ['::blt::drag&drop', id[0], id[1]]
      end
      private :__item_config_cmd

      def __item_boolval_optkeys(id)
        super(id) << 'selftarget'
      end
      private :__item_boolval_optkeys

      def __item_listval_optkeys(id)
        super(id) << 'send'
      end
      private :__item_listval_optkeys

      def __item_strval_optkeys(id)
        super(id) << 'rejectbg' << 'rejectfg' << 'tokenbg'
      end
      private :__item_strval_optkeys

      undef itemcget
      undef itemcget_tkstring
      private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

      def source_configure(win, slot, value=None)
        itemconfigure(['source', win], slot, value)
      end
      def source_configinfo(win, slot=nil)
        itemconfiginfo(['source', win], slot)
      end
      def current_source_configinfo(win, slot=nil)
        current_itemconfiginfo(['source', win], slot)
      end
    end

    class PackageCommand < TkValidateCommand
      class ValidateArgs < TkUtil::CallbackSubst
        KEY_TBL = [
          [ ?t, ?w, :token ],
          [ ?W, ?w, :widget ],
          nil
        ]

        PROC_TBL = [
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

        _setup_subst_table(KEY_TBL, PROC_TBL)

        def self.ret_val(val)
          val
        end
      end

      def self._config_keys
        ['packagecmd']
      end
    end

    class SiteCommand < TkValidateCommand
      class ValidateArgs < TkUtil::CallbackSubst
        KEY_TBL = [
          [ ?s, ?b, :compatible ],
          [ ?t, ?w, :token ],
          nil
        ]

        PROC_TBL = [
          [ ?b, TkComm.method(:bool) ],
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

        _setup_subst_table(KEY_TBL, PROC_TBL)

        def self.ret_val(val)
          val
        end
      end

      def self._config_keys
        ['sitecmd']
      end
    end

    def self.__validation_class_list
      super() << PackageCommand << SiteCommand
    end

    class << self
      Tk::ValidateConfigure.__def_validcmd(binding, PackageCommand)
      Tk::ValidateConfigure.__def_validcmd(binding, SiteCommand)
    end

    ###################################

    class DnD_Handle < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?i, ?s, :ip_name ],
        [ ?v, ?v, :value ],
        [ ?W, ?w, :widget ],
        nil
      ]

      PROC_TBL = [
        [ ?i, TkComm.method(:string) ],
        [ ?v, TkComm.method(:tk_tcl2ruby) ],
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

      _setup_subst_table(KEY_TBL, PROC_TBL)
    end

    def self.source_handler(win, datatype, cmd=Proc.new, *args)
      _bind_for_event_class(DnD_Handle,
                            ['::blt::drag&drop', 'source', win, 'handler'],
                            cmd, *args)
    end

    def self.target_handler(win, datatype, cmd=Proc.new, *args)
      _bind_for_event_class(DnD_Handle,
                            ['::blt::drag&drop', 'target', win, 'handler'],
                            cmd, *args)
    end

    ###################################

    def self.init_source(win)
      tk_call('::blt::drag&drop', 'source', win)
    end

    def self.source()
      list(tk_call('::blt::drag&drop', 'source'))
    end

    def self.source_handler_list(win)
      simplelist(tk_call('::blt::drag&drop', 'source', win, 'handler'))
    end
    def self.source_handler_info(win, type)
      tk_tcl2ruby(tk_call('::blt::drag&drop', 'source', win, 'handler', type))
    end

    def self.target
      list(tk_call('::blt::drag&drop', 'target'))
    end
    def self.target_handler_list(win)
      simplelist(tk_call('::blt::drag&drop', 'target', win, 'handler'))
    end

    def self.handle_target(win, type, val=None)
      tk_call('::blt::drag&drop', 'target', win, 'handle', type, val)
    end

    def self.token(win)
      window(tk_call('::blt::drag&drop', 'token', win))
    end

    def self.drag(win, x, y)
      tk_call('::blt::drag&drop', 'drag', win, x, y)
    end
    def self.drop(win, x, y)
      tk_call('::blt::drag&drop', 'drop', win, x, y)
    end

    def self.errors(cmd=Proc.new)
      tk_call('::blt::drag&drop', 'errors', cmd)
    end

    def self.active
      bool(tk_call('::blt::drag&drop', 'active'))
    end

    def self.location(x=None, y=None)
      list(tk_call('::blt::drag&drop', 'location', x, y))
    end
  end
end
