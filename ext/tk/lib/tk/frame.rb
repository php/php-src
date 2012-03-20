#
# tk/frame.rb : treat frame widget
#
require 'tk'

class Tk::Frame<TkWindow
  TkCommandNames = ['frame'.freeze].freeze
  WidgetClassName = 'Frame'.freeze
  WidgetClassNames[WidgetClassName] ||= self

################# old version
#  def initialize(parent=nil, keys=nil)
#    if keys.kind_of? Hash
#      keys = keys.dup
#      @classname = keys.delete('classname') if keys.key?('classname')
#      @colormap  = keys.delete('colormap')  if keys.key?('colormap')
#      @container = keys.delete('container') if keys.key?('container')
#      @visual    = keys.delete('visual')    if keys.key?('visual')
#    end
#    super(parent, keys)
#  end
#
#  def create_self
#    s = []
#    s << "-class"     << @classname if @classname
#    s << "-colormap"  << @colormap  if @colormap
#    s << "-container" << @container if @container
#    s << "-visual"    << @visual    if @visual
#    tk_call 'frame', @path, *s
#  end
#################

  def __boolval_optkeys
    super() << 'container'
  end
  private :__boolval_optkeys

  def initialize(parent=nil, keys=nil)
    my_class_name = nil
    if self.class < WidgetClassNames[self.class::WidgetClassName]
      my_class_name = self.class.name
      my_class_name = nil if my_class_name == ''
    end
    if parent.kind_of? Hash
      keys = _symbolkey2str(parent)
    else
      if keys
        keys = _symbolkey2str(keys)
        keys['parent'] = parent
      else
        keys = {'parent'=>parent}
      end
    end
    if keys.key?('classname')
       keys['class'] = keys.delete('classname')
    end
    @classname = keys['class']
    @colormap  = keys['colormap']
    @container = keys['container']
    @visual    = keys['visual']
    if !@classname && my_class_name
      keys['class'] = @classname = my_class_name
    end
    if @classname.kind_of? TkBindTag
      @db_class = @classname
      @classname = @classname.id
    elsif @classname
      @db_class = TkDatabaseClass.new(@classname)
    else
      @db_class = self.class
      @classname = @db_class::WidgetClassName
    end
    super(keys)
  end

  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('frame', @path, *hash_kv(keys))
  #  else
  #    tk_call_without_enc( 'frame', @path)
  #  end
  #end
  #private :create_self

  def database_classname
    @classname
  end

  def self.database_class
    if self == WidgetClassNames[WidgetClassName] || self.name == ''
      self
    else
      TkDatabaseClass.new(self.name)
    end
  end
  def self.database_classname
    self.database_class.name
  end

  def self.bind(*args, &b)
    if self == WidgetClassNames[WidgetClassName] || self.name == ''
      super(*args, &b)
    else
      TkDatabaseClass.new(self.name).bind(*args, &b)
    end
  end
  def self.bind_append(*args, &b)
    if self == WidgetClassNames[WidgetClassName] || self.name == ''
      super(*args, &b)
    else
      TkDatabaseClass.new(self.name).bind_append(*args, &b)
    end
  end
  def self.bind_remove(*args)
    if self == WidgetClassNames[WidgetClassName] || self.name == ''
      super(*args)
    else
      TkDatabaseClass.new(self.name).bind_remove(*args)
    end
  end
  def self.bindinfo(*args)
    if self == WidgetClassNames[WidgetClassName] || self.name == ''
      super(*args)
    else
      TkDatabaseClass.new(self.name).bindinfo(*args)
    end
  end
end

#TkFrame = Tk::Frame unless Object.const_defined? :TkFrame
#Tk.__set_toplevel_aliases__(:Tk, Tk::Frame, :TkFrame)
Tk.__set_loaded_toplevel_aliases__('tk/frame.rb', :Tk, Tk::Frame, :TkFrame)
