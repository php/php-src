require_relative 'test_base.rb'
require 'dl/callback'
require 'dl/func'
require 'dl/pack'

module DL
class TestDL < TestBase
  def ptr2num(*list)
    list.pack("p*").unpack(PackInfo::PACK_MAP[TYPE_VOIDP] + "*")
  end

  # TODO: refactor test repetition

  def test_free_secure
    assert_raises(SecurityError) do
      Thread.new do
        $SAFE = 4
        DL.free(0)
      end.join
    end
  end

  def test_realloc
    str = "abc"
    ptr_id = DL.realloc(0, 4)
    ptr    = CPtr.new(ptr_id, 4)

    assert_equal ptr_id, ptr.to_i

    cfunc  = CFunc.new(@libc['strcpy'], TYPE_VOIDP, 'strcpy')
    x      = cfunc.call([ptr_id,str].pack("l!p").unpack("l!*"))
    assert_equal("abc\0", ptr[0,4])
    DL.free ptr_id
  end

  def test_realloc_secure
    assert_raises(SecurityError) do
      Thread.new do
        $SAFE = 4
        DL.realloc(0, 4)
      end.join
    end
  end

  def test_malloc
    str = "abc"

    ptr_id = DL.malloc(4)
    ptr    = CPtr.new(ptr_id, 4)

    assert_equal ptr_id, ptr.to_i

    cfunc  = CFunc.new(@libc['strcpy'], TYPE_VOIDP, 'strcpy')
    x      = cfunc.call([ptr_id,str].pack("l!p").unpack("l!*"))
    assert_equal("abc\0", ptr[0,4])
    DL.free ptr_id
  end

  def test_malloc_security
    assert_raises(SecurityError) do
      Thread.new do
        $SAFE = 4
        DL.malloc(4)
      end.join
    end
  end

  def test_call_int()
    cfunc = CFunc.new(@libc['atoi'], TYPE_INT, 'atoi')
    x = cfunc.call(["100"].pack("p").unpack("l!*"))
    assert_equal(100, x)

    cfunc = CFunc.new(@libc['atoi'], TYPE_INT, 'atoi')
    x = cfunc.call(["-100"].pack("p").unpack("l!*"))
    assert_equal(-100, x)
  end

  def test_call_long()
    cfunc = CFunc.new(@libc['atol'], TYPE_LONG, 'atol')
    x = cfunc.call(["100"].pack("p").unpack("l!*"))
    assert_equal(100, x)
    cfunc = CFunc.new(@libc['atol'], TYPE_LONG, 'atol')
    x = cfunc.call(["-100"].pack("p").unpack("l!*"))
    assert_equal(-100, x)
  end

  def test_call_double()
    cfunc = CFunc.new(@libc['atof'], TYPE_DOUBLE, 'atof')
    x = cfunc.call(["0.1"].pack("p").unpack("l!*"))
    assert_in_delta(0.1, x)

    cfunc = CFunc.new(@libc['atof'], TYPE_DOUBLE, 'atof')
    x = cfunc.call(["-0.1"].pack("p").unpack("l!*"))
    assert_in_delta(-0.1, x)
  end

  def test_sin
    return if /x86_64/ =~ RUBY_PLATFORM
    pi_2 = Math::PI/2
    cfunc = Function.new(CFunc.new(@libm['sin'], TYPE_DOUBLE, 'sin'),
                        [TYPE_DOUBLE])
    x = cfunc.call(pi_2)
    assert_equal(Math.sin(pi_2), x)

    cfunc = Function.new(CFunc.new(@libm['sin'], TYPE_DOUBLE, 'sin'),
                        [TYPE_DOUBLE])
    x = cfunc.call(-pi_2)
    assert_equal(Math.sin(-pi_2), x)
  end

  def test_strlen()
    cfunc = CFunc.new(@libc['strlen'], TYPE_INT, 'strlen')
    x = cfunc.call(["abc"].pack("p").unpack("l!*"))
    assert_equal("abc".size, x)
  end

  def test_strcpy()
    buff = "xxxx"
    str  = "abc"
    cfunc = CFunc.new(@libc['strcpy'], TYPE_VOIDP, 'strcpy')
    x = cfunc.call(ptr2num(buff,str))
    assert_equal("abc\0", buff)
    assert_equal("abc\0", CPtr.new(x).to_s(4))

    buff = "xxxx"
    str  = "abc"
    cfunc = CFunc.new(@libc['strncpy'], TYPE_VOIDP, 'strncpy')
    x = cfunc.call(ptr2num(buff,str) + [3])
    assert_equal("abcx", buff)
    assert_equal("abcx", CPtr.new(x).to_s(4))

    ptr = CPtr.malloc(4)
    str = "abc"
    cfunc = CFunc.new(@libc['strcpy'], TYPE_VOIDP, 'strcpy')
    x = cfunc.call([ptr.to_i, *ptr2num(str)])
    assert_equal("abc\0", ptr[0,4])
    assert_equal("abc\0", CPtr.new(x).to_s(4))
  end

  def test_callback()
    buff = "foobarbaz"
    cb = set_callback(TYPE_INT,2){|x,y| CPtr.new(x)[0] <=> CPtr.new(y)[0]}
    cfunc = CFunc.new(@libc['qsort'], TYPE_VOID, 'qsort')
    cfunc.call(ptr2num(buff) + [buff.size, 1, cb])
    assert_equal('aabbfoorz', buff)
  end

  def test_dlwrap()
    ary = [0,1,2,4,5]
    addr = dlwrap(ary)
    ary2 = dlunwrap(addr)
    assert_equal(ary, ary2)
  end

  def test_type_size_t
    assert_equal(DL::TYPE_SSIZE_T, DL::TYPE_SIZE_T.abs)
  end

  def test_type_uintptr_t
    assert_equal(-DL::TYPE_INTPTR_T, DL::TYPE_UINTPTR_T)
  end

  def test_sizeof_uintptr_t
    assert_equal(DL::SIZEOF_VOIDP, DL::SIZEOF_INTPTR_T)
  end
end
end # module DL
