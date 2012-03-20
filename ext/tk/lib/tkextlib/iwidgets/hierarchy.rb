#
#  tkextlib/iwidgets/hierarchy.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/text'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Hierarchy < Tk::Iwidgets::Scrolledwidget
    end
  end
end

class Tk::Iwidgets::Hierarchy
  ItemConfCMD = ['tag'.freeze, 'configure'.freeze].freeze
  include TkTextTagConfig

  TkCommandNames = ['::iwidgets::hierarchy'.freeze].freeze
  WidgetClassName = 'Hierarchy'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  ####################################

  include Tk::ValidateConfigure

  class QueryCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL  = [ [?n, ?s, :node], nil ]
      PROC_TBL = [ [?s, TkComm.method(:string) ], nil ]

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
      # array of config-option key (string or symbol)
      ['querycommand']
    end
  end

  class IndicatorCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL  = [
        [ ?n, ?s, :node ],
        [ ?s, ?b, :status ],
        nil
      ]

      PROC_TBL = [
        [ ?s, TkComm.method(:string) ],
        [ ?b, TkComm.method(:bool) ],
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
      # array of config-option key (string or symbol)
      ['iconcommand', 'icondblcommand', 'imagedblcommand']
    end
  end

  class IconCommand < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL  = [
        [ ?n, ?s, :node ],
        [ ?i, ?s, :icon ],
        nil
      ]
      PROC_TBL = [ [ ?s, TkComm.method(:string) ], nil ]

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
      # array of config-option key (string or symbol)
      ['dblclickcommand', 'imagecommand', 'selectcommand']
    end
  end

  def __validation_class_list
    super() << QueryCommand << IndicatorCommand << IconCommand
  end

  Tk::ValidateConfigure.__def_validcmd(binding, QueryCommand)
  Tk::ValidateConfigure.__def_validcmd(binding, IndicatorCommand)
  Tk::ValidateConfigure.__def_validcmd(binding, IconCommand)

  ####################################

  def __boolval_optkeys
    super() << 'alwaysquery' << 'expanded' << 'filter'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'markbackground' << 'markforeground' << 'textbackground'
  end
  private :__strval_optkeys

  def __font_optkeys
    super() << 'textfont'
  end
  private :__font_optkeys

  def clear
    tk_call(@path, 'clear')
    self
  end

  def collapse(node)
    tk_call(@path, 'collapse')
    self
  end

  def current
    tk_call(@path, 'current')
  end

  def draw(mode=None)
    case mode
    when None
      # do nothing
    when 'now', :now
      mode = '-now'
    when 'eventually', :eventually
      mode = '-eventually'
    when String, Symbol
      mode = mode.to_s
      mode = '-' << mode if mode[0] != ?-
    end
    tk_call(@path, 'draw', mode)
  end

  def expand(node)
    tk_call(@path, 'expand', node)
    self
  end

  def expanded?(node)
    bool(tk_call(@path, 'expanded', node))
  end

  def exp_state
    list(tk_call(@path, 'expState'))
  end
  alias expand_state  exp_state
  alias expanded_list exp_state

  def mark_clear
    tk_call(@path, 'mark', 'clear')
    self
  end
  def mark_add(*nodes)
    tk_call(@path, 'mark', 'add', *nodes)
    self
  end
  def mark_remove(*nodes)
    tk_call(@path, 'mark', 'remove', *nodes)
    self
  end
  def mark_get
    list(tk_call(@path, 'mark', 'get'))
  end

  def refresh(node)
    tk_call(@path, 'refresh', node)
    self
  end

  def prune(node)
    tk_call(@path, 'prune', node)
    self
  end

  def selection_clear
    tk_call(@path, 'selection', 'clear')
    self
  end
  def selection_add(*nodes)
    tk_call(@path, 'selection', 'add', *nodes)
    self
  end
  def selection_remove(*nodes)
    tk_call(@path, 'selection', 'remove', *nodes)
    self
  end
  def selection_get
    list(tk_call(@path, 'selection', 'get'))
  end

  def toggle(node)
    tk_call(@path, 'toggle', node)
    self
  end

  # based on Tk::Text widget

  def bbox(index)
    list(tk_send_without_enc('bbox', _get_eval_enc_str(index)))
  end

  def compare(idx1, op, idx2)
    bool(tk_send_without_enc('compare', _get_eval_enc_str(idx1),
                             op, _get_eval_enc_str(idx2)))
  end

  def debug
    bool(tk_send_without_enc('debug'))
  end
  def debug=(boolean)
    tk_send_without_enc('debug', boolean)
    #self
    boolean
  end

  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end

  def dlineinfo(index)
    list(tk_send_without_enc('dlineinfo', _get_eval_enc_str(index)))
  end

  def get(*index)
    _fromUTF8(tk_send_without_enc('get', *index))
  end

  def index(index)
    tk_send_without_enc('index', _get_eval_enc_str(index))
  end

  def insert(index, chars, *tags)
    if tags[0].kind_of? Array
      # multiple chars-taglist argument :: str, [tag,...], str, [tag,...], ...
      args = [chars]
      while tags.size > 0
        args << tags.shift.collect{|x|_get_eval_string(x)}.join(' ')  # taglist
        args << tags.shift if tags.size > 0                           # chars
      end
      super(index, *args)
    else
      # single chars-taglist argument :: str, tag, tag, ...
      if tags.size == 0
        super(index, chars)
      else
        super(index, chars, tags.collect{|x|_get_eval_string(x)}.join(' '))
      end
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
    tk_send_without_enc('see', index)
    self
  end

  # based on tk/scrollable.rb
  def xview(*index)
    if index.size == 0
      list(tk_send_without_enc('xview'))
    else
      tk_send_without_enc('xview', *index)
      self
    end
  end
  def xview_moveto(*index)
    xview('moveto', *index)
  end
  def xview_scroll(*index)
    xview('scroll', *index)
  end

  def yview(*index)
    if index.size == 0
      list(tk_send_without_enc('yview'))
    else
      tk_send_without_enc('yview', *index)
      self
    end
  end
  def yview_moveto(*index)
    yview('moveto', *index)
  end
  def yview_scroll(*index)
    yview('scroll', *index)
  end
end
