require 'dl'
require 'dl/pack.rb'

module DL
  class CStruct
    def CStruct.entity_class()
      CStructEntity
    end
  end

  class CUnion
    def CUnion.entity_class()
      CUnionEntity
    end
  end

  module CStructBuilder
    def create(klass, types, members)
      new_class = Class.new(klass){
        define_method(:initialize){|addr|
          @entity = klass.entity_class.new(addr, types)
          @entity.assign_names(members)
        }
        define_method(:to_ptr){ @entity }
        define_method(:to_i){ @entity.to_i }
        members.each{|name|
          define_method(name){ @entity[name] }
          define_method(name + "="){|val| @entity[name] = val }
        }
      }
      size = klass.entity_class.size(types)
      new_class.module_eval(<<-EOS, __FILE__, __LINE__+1)
        def new_class.size()
          #{size}
        end
        def new_class.malloc()
          addr = DL.malloc(#{size})
          new(addr)
        end
      EOS
      return new_class
    end
    module_function :create
  end

  class CStructEntity < CPtr
    include PackInfo
    include ValueUtil

    def CStructEntity.malloc(types, func = nil)
      addr = DL.malloc(CStructEntity.size(types))
      CStructEntity.new(addr, types, func)
    end

    def CStructEntity.size(types)
      offset = 0
      max_align = 0
      types.each_with_index{|t,i|
        orig_offset = offset
        if( t.is_a?(Array) )
          align = PackInfo::ALIGN_MAP[t[0]]
          offset = PackInfo.align(orig_offset, align)
          size = offset - orig_offset
          offset += (PackInfo::SIZE_MAP[t[0]] * t[1])
        else
          align = PackInfo::ALIGN_MAP[t]
          offset = PackInfo.align(orig_offset, align)
          size = offset - orig_offset
          offset += PackInfo::SIZE_MAP[t]
        end
        if (max_align < align)
          max_align = align
        end
      }
      offset = PackInfo.align(offset, max_align)
      offset
    end

    def initialize(addr, types, func = nil)
      set_ctypes(types)
      super(addr, @size, func)
    end

    def assign_names(members)
      @members = members
    end

    def set_ctypes(types)
      @ctypes = types
      @offset = []
      offset = 0
      max_align = 0
      types.each_with_index{|t,i|
        orig_offset = offset
        if( t.is_a?(Array) )
          align = ALIGN_MAP[t[0]]
        else
          align = ALIGN_MAP[t]
        end
        offset = PackInfo.align(orig_offset, align)
        @offset[i] = offset
        if( t.is_a?(Array) )
          offset += (SIZE_MAP[t[0]] * t[1])
        else
          offset += SIZE_MAP[t]
        end
        if (max_align < align)
          max_align = align
        end
      }
      offset = PackInfo.align(offset, max_align)
      @size = offset
    end

    def [](name)
      idx = @members.index(name)
      if( idx.nil? )
        raise(ArgumentError, "no such member: #{name}")
      end
      ty = @ctypes[idx]
      if( ty.is_a?(Array) )
        r = super(@offset[idx], SIZE_MAP[ty[0]] * ty[1])
      else
        r = super(@offset[idx], SIZE_MAP[ty.abs])
      end
      packer = Packer.new([ty])
      val = packer.unpack([r])
      case ty
      when Array
        case ty[0]
        when TYPE_VOIDP
          val = val.collect{|v| CPtr.new(v)}
        end
      when TYPE_VOIDP
        val = CPtr.new(val[0])
      else
        val = val[0]
      end
      if( ty.is_a?(Integer) && (ty < 0) )
        return unsigned_value(val, ty)
      elsif( ty.is_a?(Array) && (ty[0] < 0) )
        return val.collect{|v| unsigned_value(v,ty[0])}
      else
        return val
      end
    end

    def []=(name, val)
      idx = @members.index(name)
      if( idx.nil? )
        raise(ArgumentError, "no such member: #{name}")
      end
      ty  = @ctypes[idx]
      packer = Packer.new([ty])
      val = wrap_arg(val, ty, [])
      buff = packer.pack([val].flatten())
      super(@offset[idx], buff.size, buff)
      if( ty.is_a?(Integer) && (ty < 0) )
        return unsigned_value(val, ty)
      elsif( ty.is_a?(Array) && (ty[0] < 0) )
        return val.collect{|v| unsigned_value(v,ty[0])}
      else
        return val
      end
    end

    def to_s()
      super(@size)
    end
  end

  class CUnionEntity < CStructEntity
    include PackInfo

    def CUnionEntity.malloc(types, func=nil)
      addr = DL.malloc(CUnionEntity.size(types))
      CUnionEntity.new(addr, types, func)
    end

    def CUnionEntity.size(types)
      size   = 0
      types.each_with_index{|t,i|
        if( t.is_a?(Array) )
          tsize = PackInfo::SIZE_MAP[t[0]] * t[1]
        else
          tsize = PackInfo::SIZE_MAP[t]
        end
        if( tsize > size )
          size = tsize
        end
      }
    end

    def set_ctypes(types)
      @ctypes = types
      @offset = []
      @size   = 0
      types.each_with_index{|t,i|
        @offset[i] = 0
        if( t.is_a?(Array) )
          size = SIZE_MAP[t[0]] * t[1]
        else
          size = SIZE_MAP[t]
        end
        if( size > @size )
          @size = size
        end
      }
    end
  end
end

