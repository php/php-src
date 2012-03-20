#
#   irb/inspector.rb - inspect methods
#   	$Release Version: 0.9.6$
#   	$Revision: 1.19 $
#   	$Date: 2002/06/11 07:51:31 $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

module IRB

  def IRB::Inspector(inspect, init = nil)
    Inspector.new(inspect, init)
  end

  class Inspector
    def initialize(inspect_proc, init_proc = nil)
      @init = init_proc
      @inspect = inspect_proc
    end

    def init
      @init.call if @init
    end

    def inspect_value(v)
      @inspect.call(v)
    end
  end

  INSPECTORS = {}

  def INSPECTORS.keys_with_inspector(inspector)
    select{|k,v| v == inspector}.collect{|k, v| k}
  end

  # ex)
  # INSPECTORS.def_inspector(key, init_p=nil){|v| v.inspect}
  # INSPECTORS.def_inspector([key1,..], init_p=nil){|v| v.inspect}
  # INSPECTORS.def_inspector(key, inspector)
  # INSPECTORS.def_inspector([key1,...], inspector)

  def INSPECTORS.def_inspector(key, arg=nil, &block)
#     if !block_given?
#       case arg
#       when nil, Proc
# 	inspector = IRB::Inspector(init_p)
#       when Inspector
# 	inspector = init_p
#       else
# 	IRB.Raise IllegalParameter, init_p
#       end
#       init_p = nil
#     else
#       inspector = IRB::Inspector(block, init_p)
#     end

    if block_given?
      inspector = IRB::Inspector(block, arg)
    else
      inspector = arg
    end

    case key
    when Array
      for k in key
	def_inspector(k, inspector)
      end
    when Symbol
      self[key] = inspector
      self[key.to_s] = inspector
    when String
      self[key] = inspector
      self[key.intern] = inspector
    else
      self[key] = inspector
    end
  end

  INSPECTORS.def_inspector([false, :to_s, :raw]){|v| v.to_s}
  INSPECTORS.def_inspector([true, :p, :inspect]){|v|
    begin
      v.inspect
    rescue NoMethodError
      puts "(Object doesn't support #inspect)"
    end
  }
  INSPECTORS.def_inspector([:pp, :pretty_inspect], proc{require "pp"}){|v| v.pretty_inspect.chomp}
  INSPECTORS.def_inspector([:yaml, :YAML], proc{require "yaml"}){|v|
    begin
      YAML.dump(v)
    rescue
      puts "(can't dump yaml. use inspect)"
      v.inspect
    end
  }

  INSPECTORS.def_inspector([:marshal, :Marshal, :MARSHAL, Marshal]){|v|
    Marshal.dump(v)
  }
end





