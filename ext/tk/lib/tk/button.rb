#
# tk/button.rb : treat button widget
#
require 'tk'
require 'tk/label'

class Tk::Button<Tk::Label
  TkCommandNames = ['button'.freeze].freeze
  WidgetClassName = 'Button'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('button', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('button', @path)
  #  end
  #end
  #private :create_self

  def invoke
    _fromUTF8(tk_send_without_enc('invoke'))
  end
  def flash
    tk_send_without_enc('flash')
    self
  end
end

#TkButton = Tk::Button unless Object.const_defined? :TkButton
#Tk.__set_toplevel_aliases__(:Tk, Tk::Button, :TkButton)
Tk.__set_loaded_toplevel_aliases__('tk/button.rb', :Tk, Tk::Button, :TkButton)
