#-----------------------------
# olegen.rb
# $Revision$
#-----------------------------

require 'win32ole'

class WIN32COMGen
  def initialize(typelib)
    @typelib = typelib
    @reciever = ""
  end
  attr_reader :typelib

  def ole_classes(typelib)
    begin
      @ole = WIN32OLE.new(typelib)
      [@ole.ole_obj_help]
    rescue
      WIN32OLE_TYPE.ole_classes(typelib)
    end
  end

  def generate_args(method)
    args = []
    if method.size_opt_params >= 0
      size_required_params = method.size_params - method.size_opt_params
    else
      size_required_params = method.size_params - 1
    end
    size_required_params.times do |i|
      if method.params[i] && method.params[i].optional?
        args.push "arg#{i}=nil"
      else
        args.push "arg#{i}"
      end
    end
    if method.size_opt_params >= 0
      method.size_opt_params.times do |i|
        args.push "arg#{i + size_required_params}=nil"
      end
    else
      args.push "*arg"
    end
    args.join(", ")
  end

  def generate_argtype(typedetails)
    ts = ''
    typedetails.each do |t|
      case t
      when 'CARRAY', 'VOID', 'UINT', 'RESULT', 'DECIMAL', 'I8', 'UI8'
#	  raise "Sorry type\"" + t + "\" not supported"
      ts << "\"??? NOT SUPPORTED TYPE:`#{t}'\""
      when 'USERDEFINED', 'Unknown Type 9'
        ts << 'VT_DISPATCH'
        break;
      when 'SAFEARRAY'
        ts << 'VT_ARRAY|'
      when 'PTR'
        ts << 'VT_BYREF|'
      when 'INT'
        ts << 'VT_I4'
      else
        if String === t
          ts << 'VT_' + t
        end
      end
    end
    if ts.empty?
      ts = 'VT_VARIANT'
    elsif ts[-1] == ?|
	ts += 'VT_VARIANT'
    end
    ts
  end

  def generate_argtypes(method, proptypes)
    types = method.params.collect{|param|
      generate_argtype(param.ole_type_detail)
    }.join(", ")
    if proptypes
      types += ", " if types.size > 0
      types += generate_argtype(proptypes)
    end
    types
  end

  def generate_method_body(method, disptype, types=nil)
    "    ret = #{@reciever}#{disptype}(#{method.dispid}, [" +
    generate_args(method).gsub("=nil", "") +
    "], [" +
    generate_argtypes(method, types) +
    "])\n" +
    "    @lastargs = WIN32OLE::ARGV\n" +
    "    ret"
  end

  def generate_method_help(method, type = nil)
    str = "  # "
    if type
      str += type
    else
      str += method.return_type
    end
    str += " #{method.name}"
    if method.event?
      str += " EVENT"
      str += " in #{method.event_interface}"
    end
    if method.helpstring && method.helpstring != ""
      str += "\n  # "
      str += method.helpstring
    end
    args_help = generate_method_args_help(method)
    if args_help
      str += "\n"
      str += args_help
    end
    str
  end

  def generate_method_args_help(method)
    args = []
    method.params.each_with_index {|param, i|
      h = "  #   #{param.ole_type} arg#{i} --- #{param.name}"
      inout = []
      inout.push "IN" if param.input?
      inout.push "OUT" if param.output?
      h += " [#{inout.join('/')}]"
      h += " ( = #{param.default})" if param.default
      args.push h
    }
    if args.size > 0
      args.join("\n")
    else
      nil
    end
  end

  def generate_method(method, disptype, io = STDOUT, types = nil)
    io.puts "\n"
    io.puts  generate_method_help(method)
    if method.invoke_kind == 'PROPERTYPUT'
      io.print "  def #{method.name}=("
    else
      io.print "  def #{method.name}("
    end
    io.print generate_args(method)
    io.puts ")"
    io.puts generate_method_body(method, disptype, types)
    io.puts "  end"
  end

  def generate_propputref_methods(klass, io = STDOUT)
    klass.ole_methods.select {|method|
      method.invoke_kind == 'PROPERTYPUTREF' && method.visible?
    }.each do |method|
      generate_method(method, io)
    end
  end

  def generate_properties_with_args(klass, io = STDOUT)
    klass.ole_methods.select {|method|
      method.invoke_kind == 'PROPERTYGET' &&
      method.visible? &&
      method.size_params > 0
    }.each do |method|
      types = method.return_type_detail
      io.puts "\n"
      io.puts  generate_method_help(method, types[0])
      io.puts  "  def #{method.name}"
      if klass.ole_type == "Class"
        io.print "    OLEProperty.new(@dispatch, #{method.dispid}, ["
      else
        io.print "    OLEProperty.new(self, #{method.dispid}, ["
      end
      io.print generate_argtypes(method, nil)
      io.print "], ["
      io.print generate_argtypes(method, types)
      io.puts "])"
      io.puts  "  end"
    end
  end

  def generate_propput_methods(klass, io = STDOUT)
    klass.ole_methods.select {|method|
      method.invoke_kind == 'PROPERTYPUT' && method.visible? &&
      method.size_params == 1
    }.each do |method|
      ms = klass.ole_methods.select {|m|
        m.invoke_kind == 'PROPERTYGET' &&
        m.dispid == method.dispid
      }
      types = []
      if ms.size == 1
        types = ms[0].return_type_detail
      end
      generate_method(method, '_setproperty', io, types)
    end
  end

  def generate_propget_methods(klass, io = STDOUT)
    klass.ole_methods.select {|method|
      method.invoke_kind == 'PROPERTYGET' && method.visible? &&
      method.size_params == 0
    }.each do |method|
      generate_method(method, '_getproperty', io)
    end
  end

  def generate_func_methods(klass, io = STDOUT)
    klass.ole_methods.select {|method|
      method.invoke_kind == "FUNC" && method.visible?
    }.each do |method|
      generate_method(method, '_invoke', io)
    end
  end

  def generate_methods(klass, io = STDOUT)
    generate_propget_methods(klass, io)
    generate_propput_methods(klass, io)
    generate_properties_with_args(klass, io)
    generate_func_methods(klass, io)
#   generate_propputref_methods(klass, io)
  end

  def generate_constants(klass, io = STDOUT)
    klass.variables.select {|v|
      v.visible? && v.variable_kind == 'CONSTANT'
    }.each do |v|
      io.print "  "
      io.print v.name.sub(/^./){$&.upcase}
      io.print " = "
      io.puts  v.value
    end
  end

  def class_name(klass)
    klass_name = klass.name
    if klass.ole_type == "Class" &&
       klass.guid &&
       klass.progid
       klass_name = klass.progid.gsub(/\./, '_')
    end
    if /^[A-Z]/ !~ klass_name || Module.constants.include?(klass_name)
      klass_name = 'OLE' + klass_name
    end
    klass_name
  end

  def define_initialize(klass)
    <<STR

  def initialize(obj = nil)
    @clsid = "#{klass.guid}"
    @progid = "#{klass.progid}"
    if obj.nil?
      @dispatch = WIN32OLE.new @progid
    else
      @dispatch = obj
    end
  end
STR
  end

  def define_include
    "  include WIN32OLE::VARIANT"
  end

  def define_instance_variables
    "  attr_reader :lastargs"
  end

  def define_method_missing
    <<STR

  def method_missing(cmd, *arg)
    @dispatch.method_missing(cmd, *arg)
  end
STR
  end

  def define_class(klass, io = STDOUT)
    io.puts "class #{class_name(klass)} # #{klass.name}"
    io.puts define_include
    io.puts define_instance_variables
    io.puts "  attr_reader :dispatch"
    io.puts "  attr_reader :clsid"
    io.puts "  attr_reader :progid"
    io.puts define_initialize(klass)
    io.puts define_method_missing
  end

  def define_module(klass, io = STDOUT)
    io.puts "module #{class_name(klass)}"
    io.puts define_include
    io.puts define_instance_variables
  end

  def generate_class(klass, io = STDOUT)
    io.puts "\n# #{klass.helpstring}"
    if klass.ole_type == "Class" &&
       klass.guid &&
       klass.progid
      @reciever = "@dispatch."
      define_class(klass, io)
    else
      @reciever = ""
      define_module(klass, io)
    end
    generate_constants(klass, io)
    generate_methods(klass, io)
    io.puts "end"
  end

  def generate(io = STDOUT)
    io.puts "require 'win32ole'"
    io.puts "require 'win32ole/property'"

    ole_classes(typelib).select{|klass|
      klass.visible? &&
      (klass.ole_type == "Class" ||
       klass.ole_type == "Interface" ||
       klass.ole_type == "Dispatch" ||
       klass.ole_type == "Enum")
    }.each do |klass|
      generate_class(klass, io)
    end
    begin
      @ole.quit if @ole
    rescue
    end
  end
end

require 'win32ole'
if __FILE__ == $0
  if ARGV.size == 0
    $stderr.puts "usage: #{$0} Type Library [...]"
    exit 1
  end
  ARGV.each do |typelib|
    comgen = WIN32COMGen.new(typelib)
    comgen.generate
  end
end
