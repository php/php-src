#
# tk/label.rb : treat label widget
#
require 'tk'

class Tk::Label<TkWindow
  TkCommandNames = ['label'.freeze].freeze
  WidgetClassName = 'Label'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('label', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('label', @path)
  #  end
  #end
  #private :create_self
end

#TkLabel = Tk::Label unless Object.const_defined? :TkLabel
#Tk.__set_toplevel_aliases__(:Tk, Tk::Label, :TkLabel)
Tk.__set_loaded_toplevel_aliases__('tk/label.rb', :Tk, Tk::Label, :TkLabel)
