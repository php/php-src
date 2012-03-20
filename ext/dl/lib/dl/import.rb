require 'dl'
require 'dl/func.rb'
require 'dl/struct.rb'
require 'dl/cparser.rb'

module DL
  class CompositeHandler
    def initialize(handlers)
      @handlers = handlers
    end

    def handlers()
      @handlers
    end

    def sym(symbol)
      @handlers.each{|handle|
        if( handle )
          begin
            addr = handle.sym(symbol)
            return addr
          rescue DLError
          end
        end
      }
      return nil
    end

    def [](symbol)
      sym(symbol)
    end
  end

  # DL::Importer includes the means to dynamically load libraries and build
  # modules around them including calling extern functions within the C
  # library that has been loaded.
  #
  # == Example
  #
  #   require 'dl'
  #   require 'dl/import'
  #
  #   module LibSum
  #   	extend DL::Importer
  #   	dlload './libsum.so'
  #   	extern 'double sum(double*, int)'
  #   	extern 'double split(double)'
  #   end
	#
  module Importer
    include DL
    include CParser
    extend Importer

    def dlload(*libs)
      handles = libs.collect{|lib|
        case lib
        when nil
          nil
        when Handle
          lib
        when Importer
          lib.handlers
        else
          begin
            DL.dlopen(lib)
          rescue DLError
            raise(DLError, "can't load #{lib}")
          end
        end
      }.flatten()
      @handler = CompositeHandler.new(handles)
      @func_map = {}
      @type_alias = {}
    end

    def typealias(alias_type, orig_type)
      @type_alias[alias_type] = orig_type
    end

    def sizeof(ty)
      case ty
      when String
        ty = parse_ctype(ty, @type_alias).abs()
        case ty
        when TYPE_CHAR
          return SIZEOF_CHAR
        when TYPE_SHORT
          return SIZEOF_SHORT
        when TYPE_INT
          return SIZEOF_INT
        when TYPE_LONG
          return SIZEOF_LONG
        when TYPE_LONG_LONG
          return SIZEOF_LONG_LON
        when TYPE_FLOAT
          return SIZEOF_FLOAT
        when TYPE_DOUBLE
          return SIZEOF_DOUBLE
        when TYPE_VOIDP
          return SIZEOF_VOIDP
        else
          raise(DLError, "unknown type: #{ty}")
        end
      when Class
        if( ty.instance_methods().include?(:to_ptr) )
          return ty.size()
        end
      end
      return CPtr[ty].size()
    end

    def parse_bind_options(opts)
      h = {}
      while( opt = opts.shift() )
        case opt
        when :stdcall, :cdecl
          h[:call_type] = opt
        when :carried, :temp, :temporal, :bind
          h[:callback_type] = opt
          h[:carrier] = opts.shift()
        else
          h[opt] = true
        end
      end
      h
    end
    private :parse_bind_options

    def extern(signature, *opts)
      symname, ctype, argtype = parse_signature(signature, @type_alias)
      opt = parse_bind_options(opts)
      f = import_function(symname, ctype, argtype, opt[:call_type])
      name = symname.gsub(/@.+/,'')
      @func_map[name] = f
      # define_method(name){|*args,&block| f.call(*args,&block)}
      begin
        /^(.+?):(\d+)/ =~ caller.first
        file, line = $1, $2.to_i
      rescue
        file, line = __FILE__, __LINE__+3
      end
      module_eval(<<-EOS, file, line)
        def #{name}(*args, &block)
          @func_map['#{name}'].call(*args,&block)
        end
      EOS
      module_function(name)
      f
    end

    def bind(signature, *opts, &blk)
      name, ctype, argtype = parse_signature(signature, @type_alias)
      h = parse_bind_options(opts)
      case h[:callback_type]
      when :bind, nil
        f = bind_function(name, ctype, argtype, h[:call_type], &blk)
      when :temp, :temporal
        f = create_temp_function(name, ctype, argtype, h[:call_type])
      when :carried
        f = create_carried_function(name, ctype, argtype, h[:call_type], h[:carrier])
      else
        raise(RuntimeError, "unknown callback type: #{h[:callback_type]}")
      end
      @func_map[name] = f
      #define_method(name){|*args,&block| f.call(*args,&block)}
      begin
        /^(.+?):(\d+)/ =~ caller.first
        file, line = $1, $2.to_i
      rescue
        file, line = __FILE__, __LINE__+3
      end
      module_eval(<<-EOS, file, line)
        def #{name}(*args,&block)
          @func_map['#{name}'].call(*args,&block)
        end
      EOS
      module_function(name)
      f
    end

    def struct(signature)
      tys, mems = parse_struct_signature(signature, @type_alias)
      DL::CStructBuilder.create(CStruct, tys, mems)
    end

    def union(signature)
      tys, mems = parse_struct_signature(signature, @type_alias)
      DL::CStructBuilder.create(CUnion, tys, mems)
    end

    def [](name)
      @func_map[name]
    end

    def create_value(ty, val=nil)
      s = struct([ty + " value"])
      ptr = s.malloc()
      if( val )
        ptr.value = val
      end
      return ptr
    end
    alias value create_value

    def import_value(ty, addr)
      s = struct([ty + " value"])
      ptr = s.new(addr)
      return ptr
    end

    def handler
      @handler or raise "call dlload before importing symbols and functions"
    end

    def import_symbol(name)
      addr = handler.sym(name)
      if( !addr )
        raise(DLError, "cannot find the symbol: #{name}")
      end
      CPtr.new(addr)
    end

    def import_function(name, ctype, argtype, call_type = nil)
      addr = handler.sym(name)
      if( !addr )
        raise(DLError, "cannot find the function: #{name}()")
      end
      Function.new(CFunc.new(addr, ctype, name, call_type || :cdecl), argtype)
    end

    def bind_function(name, ctype, argtype, call_type = nil, &block)
      if DL.fiddle?
        closure = Class.new(Fiddle::Closure) {
          define_method(:call, block)
        }.new(ctype, argtype)

        Function.new(closure, argtype)
      else
        f = Function.new(CFunc.new(0, ctype, name, call_type || :cdecl), argtype)
        f.bind(&block)
        f
      end
    end

    def create_temp_function(name, ctype, argtype, call_type = nil)
      TempFunction.new(CFunc.new(0, ctype, name, call_type || :cdecl), argtype)
    end

    def create_carried_function(name, ctype, argtype, call_type = nil, n = 0)
      CarriedFunction.new(CFunc.new(0, ctype, name, call_type || :cdecl), argtype, n)
    end
  end
end
