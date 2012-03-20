#
# tk/message.rb : treat message widget
#
require 'tk'
require 'tk/label'

class Tk::Message<Tk::Label
  TkCommandNames = ['message'.freeze].freeze
  WidgetClassName = 'Message'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('message', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('message', @path)
  #  end
  #end
  private :create_self
end

#TkMessage = Tk::Message unless Object.const_defined? :TkMessage
#Tk.__set_toplevel_aliases__(:Tk, Tk::Message, :TkMessage)
Tk.__set_loaded_toplevel_aliases__('tk/message.rb', :Tk, Tk::Message,
                                   :TkMessage)
