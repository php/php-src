#
# tk/checkbutton.rb : treat checkbutton widget
#
require 'tk'
require 'tk/radiobutton'

class Tk::CheckButton<Tk::RadioButton
  TkCommandNames = ['checkbutton'.freeze].freeze
  WidgetClassName = 'Checkbutton'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('checkbutton', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('checkbutton', @path)
  #  end
  #end
  #private :create_self

  def toggle
    tk_send_without_enc('toggle')
    self
  end
end

Tk::Checkbutton = Tk::CheckButton
#TkCheckButton = Tk::CheckButton unless Object.const_defined? :TkCheckButton
#TkCheckbutton = Tk::Checkbutton unless Object.const_defined? :TkCheckbutton
#Tk.__set_toplevel_aliases__(:Tk, Tk::CheckButton,
#                            :TkCheckButton, :TkCheckbutton)
Tk.__set_loaded_toplevel_aliases__('tk/checkbutton.rb', :Tk, Tk::CheckButton,
                                   :TkCheckButton, :TkCheckbutton)
