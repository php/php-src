#
#  tkextlib/bwidget/entry.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/bwidget.rb'
require 'tkextlib/bwidget/arrowbutton'
require 'tkextlib/bwidget/entry'

module Tk
  module BWidget
    class SpinBox < Tk::Entry
    end
  end
end

class Tk::BWidget::SpinBox
  include Scrollable

  TkCommandNames = ['SpinBox'.freeze].freeze
  WidgetClassName = 'SpinBox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'helptext' << 'insertbackground' << 'entryfg' << 'entrybg'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'dragenabled' << 'dropenabled' << 'editable'
  end
  private :__boolval_optkeys

  def __listval_optkeys
    super() << 'values'
  end
  private :__listval_optkeys

  def __tkvariable_optkeys
    super() << 'helpvar'
  end
  private :__tkvariable_optkeys

  #def entrybind(*args)
  #  _bind([path, 'bind'], *args)
  #  self
  #end
  def entrybind(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind([path, 'bind'], context, cmd, *args)
    self
  end

  #def entrybind_append(*args)
  #  _bind_append([path, 'bind'], *args)
  #  self
  #end
  def entrybind_append(context, *args)
    #if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append([path, 'bind'], context, cmd, *args)
    self
  end

  def entrybind_remove(*args)
    _bind_remove([path, 'bind'], *args)
    self
  end

  def entrybindinfo(*args)
    _bindinfo([path, 'bind'], *args)
    self
  end

  def get_index_of_value
    number(tk_send_without_enc('getvalue'))
  end
  alias get_value get_index_of_value
  alias get_value_index get_index_of_value

  def set_value_by_index(idx)
    idx = "@#{idx}" if idx.kind_of?(Integer)
    tk_send_without_enc('setvalue', idx)
    self
  end
  alias set_value set_value_by_index
  alias set_index_value set_value_by_index
end
