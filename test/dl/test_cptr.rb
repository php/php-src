require_relative 'test_base'
require_relative '../ruby/envutil'

module DL
  class TestCPtr < TestBase
    def test_cptr_to_int
      null = DL::NULL
      assert_equal(null.to_i, null.to_int)
    end

    def test_malloc_free_func_int
      free = CFunc.new(DL::RUBY_FREE, TYPE_VOID, 'free')

      ptr  = CPtr.malloc(10, free.to_i)
      assert_equal 10, ptr.size
      assert_equal free.to_i, ptr.free.to_i
    end

    def test_malloc_free_func
      free = CFunc.new(DL::RUBY_FREE, TYPE_VOID, 'free')

      ptr  = CPtr.malloc(10, free)
      assert_equal 10, ptr.size
      assert_equal free.to_i, ptr.free.to_i
    end

    def test_to_str
      str = "hello world"
      ptr = CPtr[str]

      assert_equal 3, ptr.to_str(3).length
      assert_equal str, ptr.to_str

      ptr[5] = 0
      assert_equal "hello\0world", ptr.to_str
    end

    def test_to_s
      str = "hello world"
      ptr = CPtr[str]

      assert_equal 3, ptr.to_s(3).length
      assert_equal str, ptr.to_s

      ptr[5] = 0
      assert_equal 'hello', ptr.to_s
    end

    def test_minus
      str = "hello world"
      ptr = CPtr[str]
      assert_equal ptr.to_s, (ptr + 3 - 3).to_s
    end

    # TODO: what if the pointer size is 0?  raise an exception? do we care?
    def test_plus
      str = "hello world"
      ptr = CPtr[str]
      new_str = ptr + 3
      assert_equal 'lo world', new_str.to_s
    end

    def test_inspect
      ptr = CPtr.new(0)
      inspect = ptr.inspect
      assert_match(/size=#{ptr.size}/, inspect)
      assert_match(/free=#{sprintf("%#x", ptr.free.to_i)}/, inspect)
      assert_match(/ptr=#{sprintf("%#x", ptr.to_i)}/, inspect)
    end

    def test_to_ptr_string
      str = "hello world"
      ptr = CPtr[str]
      assert ptr.tainted?, 'pointer should be tainted'
      assert_equal str.length, ptr.size
      assert_equal 'hello', ptr[0,5]
    end

    def test_to_ptr_io
      buf = CPtr.malloc(10)
      File.open(__FILE__, 'r') do |f|
        ptr = CPtr.to_ptr f
        fread = CFunc.new(@libc['fread'], TYPE_VOID, 'fread')
        fread.call([buf.to_i, DL::SIZEOF_CHAR, buf.size - 1, ptr.to_i])
      end

      File.open(__FILE__, 'r') do |f|
        assert_equal f.read(9), buf.to_s
      end
    end

    def test_to_ptr_with_ptr
      ptr = CPtr.new 0
      ptr2 = CPtr.to_ptr Struct.new(:to_ptr).new(ptr)
      assert_equal ptr, ptr2

      assert_raises(DL::DLError) do
        CPtr.to_ptr Struct.new(:to_ptr).new(nil)
      end
    end

    def test_to_ptr_with_num
      ptr = CPtr.new 0
      assert_equal ptr, CPtr[0]
    end

    def test_equals
      ptr   = CPtr.new 0
      ptr2  = CPtr.new 0
      assert_equal ptr2, ptr
    end

    def test_not_equals
      ptr = CPtr.new 0
      assert_not_equal 10, ptr, '10 should not equal the pointer'
    end

    def test_cmp
      ptr = CPtr.new 0
      assert_nil(ptr <=> 10, '10 should not be comparable')
    end

    def test_ref_ptr
      ary = [0,1,2,4,5]
      addr = CPtr.new(dlwrap(ary))
      assert_equal addr.to_i, addr.ref.ptr.to_i

      assert_equal addr.to_i, (+ (- addr)).to_i
    end

    def test_to_value
      ary = [0,1,2,4,5]
      addr = CPtr.new(dlwrap(ary))
      assert_equal ary, addr.to_value
    end

    def test_free
      ptr = CPtr.malloc(4)
      assert_nil ptr.free
    end

    def test_free=
      assert_normal_exit(<<-"End", '[ruby-dev:39269]')
        require 'dl'
        DL::LIBC_SO = #{DL::LIBC_SO.dump}
        DL::LIBM_SO = #{DL::LIBM_SO.dump}
        include DL
        @libc = dlopen(LIBC_SO)
        @libm = dlopen(LIBM_SO)
        free = CFunc.new(DL::RUBY_FREE, TYPE_VOID, 'free')
        ptr = CPtr.malloc(4)
        ptr.free = free
        free.ptr
        ptr.free.ptr
      End

      free = CFunc.new(DL::RUBY_FREE, TYPE_VOID, 'free')
      ptr = CPtr.malloc(4)
      ptr.free = free

      assert_equal free.ptr, ptr.free.ptr
    end

    def test_null?
      ptr = CPtr.new(0)
      assert ptr.null?
    end

    def test_size
      ptr = CPtr.malloc(4)
      assert_equal 4, ptr.size
      DL.free ptr.to_i
    end

    def test_size=
      ptr = CPtr.malloc(4)
      ptr.size = 10
      assert_equal 10, ptr.size
      DL.free ptr.to_i
    end

    def test_aref_aset
      check = Proc.new{|str,ptr|
        assert_equal(str.size(), ptr.size())
        assert_equal(str, ptr.to_s())
        assert_equal(str[0,2], ptr.to_s(2))
        assert_equal(str[0,2], ptr[0,2])
        assert_equal(str[1,2], ptr[1,2])
        assert_equal(str[1,0], ptr[1,0])
        assert_equal(str[0].ord, ptr[0])
        assert_equal(str[1].ord, ptr[1])
      }
      str = 'abc'
      ptr = CPtr[str]
      check.call(str, ptr)

      str[0] = "c"
      assert_equal 'c'.ord, ptr[0] = "c".ord
      check.call(str, ptr)

      str[0,2] = "aa"
      assert_equal 'aa', ptr[0,2] = "aa"
      check.call(str, ptr)

      ptr2 = CPtr['cdeeee']
      str[0,2] = "cd"
      assert_equal ptr2, ptr[0,2] = ptr2
      check.call(str, ptr)

      ptr3 = CPtr['vvvv']
      str[0,2] = "vv"
      assert_equal ptr3.to_i, ptr[0,2] = ptr3.to_i
      check.call(str, ptr)
    end

    def test_null_pointer
      nullpo = CPtr.new(0)
      assert_raise(DLError) {nullpo[0]}
      assert_raise(DLError) {nullpo[0] = 1}
    end
  end
end
