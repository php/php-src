#
# tk/labelframe.rb : treat labelframe widget
#
require 'tk'
require 'tk/frame'

class Tk::LabelFrame<Tk::Frame
  TkCommandNames = ['labelframe'.freeze].freeze
  WidgetClassName = 'Labelframe'.freeze
  WidgetClassNames[WidgetClassName] ||= self
  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('labelframe', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('labelframe', @path)
  #  end
  #end
  #private :create_self

  def __val2ruby_optkeys  # { key=>proc, ... }
    super().update('labelwidget'=>proc{|v| window(v)})
  end
  private :__val2ruby_optkeys
end

Tk::Labelframe = Tk::LabelFrame
#TkLabelFrame = Tk::LabelFrame unless Object.const_defined? :TkLabelFrame
#TkLabelframe = Tk::Labelframe unless Object.const_defined? :TkLabelframe
#Tk.__set_toplevel_aliases__(:Tk, Tk::LabelFrame, :TkLabelFrame, :TkLabelframe)
Tk.__set_loaded_toplevel_aliases__('tk/labelframe.rb', :Tk, Tk::LabelFrame,
                                   :TkLabelFrame, :TkLabelframe)
