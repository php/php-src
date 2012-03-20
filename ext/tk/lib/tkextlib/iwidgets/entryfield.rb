#
#  tkextlib/iwidgets/entryfield.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class  Entryfield < Tk::Iwidgets::Labeledwidget
    end
  end
end

class Tk::Iwidgets::Entryfield
  TkCommandNames = ['::iwidgets::entryfield'.freeze].freeze
  WidgetClassName = 'Entryfield'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __font_optkeys
    super() << 'textfont'
  end
  private :__font_optkeys

  ####################################

  include Tk::ValidateConfigure

  class EntryfieldValidate < TkValidateCommand
    #class CalCmdArgs < TkUtil::CallbackSubst
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL  = [
        [ ?c, ?s, :char ],
        [ ?P, ?s, :post ],
        [ ?S, ?s, :current ],
        [ ?W, ?w, :widget ],
        nil
      ]
      PROC_TBL = [
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
    end

    def self._config_keys
      ['validate', 'invalid']
    end
  end

  def __validation_class_list
    super() << EntryfieldValidate
  end

  Tk::ValidateConfigure.__def_validcmd(binding, EntryfieldValidate)
=begin
  def validate(cmd = Proc.new, args = nil)
    if cmd.kind_of?(ValidateCmd)
      configure('validate', cmd)
    elsif args
      configure('validate', [cmd, args])
    else
      configure('validate', cmd)
    end
  end

  def invalid(cmd = Proc.new, args = nil)
    if cmd.kind_of?(ValidateCmd)
      configure('invalid', cmd)
    elsif args
      configure('invalid', [cmd, args])
    else
      configure('invalid', cmd)
    end
  end
=end

  ####################################

  def clear
    tk_call(@path, 'clear')
    self
  end

  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end

  def value
    _fromUTF8(tk_send_without_enc('get'))
  end
  def value= (val)
    tk_send_without_enc('delete', 0, 'end')
    tk_send_without_enc('insert', 0, _get_eval_enc_str(val))
    val
  end
  alias get value
  alias set value=

  def cursor=(index)
    tk_send_without_enc('icursor', index)
    #self
    index
  end
  alias icursor cursor=

  def index(index)
    number(tk_send_without_enc('index', index))
  end

  def insert(pos,text)
    tk_send_without_enc('insert', pos, _get_eval_enc_str(text))
    self
  end

  def mark(pos)
    tk_send_without_enc('scan', 'mark', pos)
    self
  end
  def dragto(pos)
    tk_send_without_enc('scan', 'dragto', pos)
    self
  end
  def selection_adjust(index)
    tk_send_without_enc('selection', 'adjust', index)
    self
  end
  def selection_clear
    tk_send_without_enc('selection', 'clear')
    self
  end
  def selection_from(index)
    tk_send_without_enc('selection', 'from', index)
    self
  end
  def selection_present()
    bool(tk_send_without_enc('selection', 'present'))
  end
  def selection_range(s, e)
    tk_send_without_enc('selection', 'range', s, e)
    self
  end
  def selection_to(index)
    tk_send_without_enc('selection', 'to', index)
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
end
