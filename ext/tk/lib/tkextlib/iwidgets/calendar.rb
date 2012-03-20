#
#  tkextlib/iwidgets/calendar.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Calendar < Tk::Itk::Widget
    end
  end
end

class Tk::Iwidgets::Calendar
  TkCommandNames = ['::iwidgets::calendar'.freeze].freeze
  WidgetClassName = 'Calendar'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() + [
      'buttonforeground', 'outline', 'selectcolor',
      'weekdaybackground', 'weekendbackground'
    ]
  end
  private :__strval_optkeys

  def __listval_optkeys
    super() << 'days'
  end
  private :__listval_optkeys

  def __font_optkeys
    super() + ['currentdatefont', 'datefont', 'dayfont', 'titlefont']
  end
  private :__font_optkeys

  ####################################

  include Tk::ValidateConfigure

  class CalendarCommand < TkValidateCommand
    #class CalCmdArgs < TkUtil::CallbackSubst
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL  = [ [?d, ?s, :date], nil ]
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
      ['command']
    end

    #def initialize(cmd = Proc.new, *args)
    #  _initialize_for_cb_class(CalCmdArgs, cmd, *args)
    #end
  end

  def __validation_class_list
    super() << CalendarCommand
  end

  Tk::ValidateConfigure.__def_validcmd(binding, CalendarCommand)
=begin
  def command(cmd = Proc.new, args = nil)
    if cmd.kind_of?(CalendarCommand)
      configure('command', cmd)
    elsif args
      configure('command', [cmd, args])
    else
      configure('command', cmd)
    end
  end
=end

  ####################################

  def get_string
    tk_call(@path, 'get', '-string')
  end
  alias get get_string

  def get_clicks
    number(tk_call(@path, 'get', '-clicks'))
  end

  def select(date)
    tk_call(@path, 'select', date)
    self
  end

  def show(date)
    tk_call(@path, 'show', date)
    self
  end
  def show_now
    tk_call(@path, 'show', 'now')
    self
  end
end
