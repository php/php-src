#
# tk/radiobutton.rb : treat radiobutton widget
#
require 'tk'
require 'tk/button'

class Tk::RadioButton<Tk::Button
  TkCommandNames = ['radiobutton'.freeze].freeze
  WidgetClassName = 'Radiobutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('radiobutton', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('radiobutton', @path)
  #  end
  #end
  #private :create_self

  def __boolval_optkeys
    super() << 'indicatoron'
  end
  private :__boolval_optkeys

  def __strval_optkeys
    super() << 'selectcolor'
  end
  private :__strval_optkeys

  def __ruby2val_optkeys  # { key=>proc, ... }
    {
      'variable'=>proc{|v| tk_trace_variable(v)}  # for backward compatibility
    }
  end
  private :__ruby2val_optkeys


  def deselect
    tk_send_without_enc('deselect')
    self
  end
  def select
    tk_send_without_enc('select')
    self
  end

  def get_value
    var = tk_send_without_enc('cget', '-variable')
    if TkVariable::USE_TCLs_SET_VARIABLE_FUNCTIONS
      _fromUTF8(INTERP._get_global_var(var))
    else
      INTERP._eval(Kernel.format('global %s; set %s', var, var))
    end
  end

  def set_value(val)
    var = tk_send_without_enc('cget', '-variable')
    if TkVariable::USE_TCLs_SET_VARIABLE_FUNCTIONS
      _fromUTF8(INTERP._set_global_var(var, _get_eval_string(val, true)))
    else
      s = '"' + _get_eval_string(val).gsub(/[\[\]$"\\]/, '\\\\\&') + '"'
      INTERP._eval(Kernel.format('global %s; set %s %s', var, var, s))
    end
  end
end

Tk::Radiobutton = Tk::RadioButton
#TkRadioButton = Tk::RadioButton unless Object.const_defined? :TkRadioButton
#TkRadiobutton = Tk::Radiobutton unless Object.const_defined? :TkRadiobutton
#Tk.__set_toplevel_aliases__(:Tk, Tk::RadioButton,
#                            :TkRadioButton, :TkRadiobutton)
Tk.__set_loaded_toplevel_aliases__('tk/radiobutton.rb', :Tk, Tk::RadioButton,
                                   :TkRadioButton, :TkRadiobutton)
