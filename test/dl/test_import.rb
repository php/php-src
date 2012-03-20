require_relative 'test_base'
require 'dl/import'

module DL
  module LIBC
    extend Importer
    dlload LIBC_SO, LIBM_SO

    typealias 'string', 'char*'
    typealias 'FILE*', 'void*'

    extern "void *strcpy(char*, char*)"
    extern "int isdigit(int)"
    extern "double atof(string)"
    extern "unsigned long strtoul(char*, char **, int)"
    extern "int qsort(void*, unsigned long, unsigned long, void*)"
    extern "int fprintf(FILE*, char*)"
    extern "int gettimeofday(timeval*, timezone*)" rescue nil

    QsortCallback = bind("void *qsort_callback(void*, void*)", :temp)
    BoundQsortCallback = bind("void *bound_qsort_callback(void*, void*)"){|ptr1,ptr2| ptr1[0] <=> ptr2[0]}
    Timeval = struct [
      "long tv_sec",
      "long tv_usec",
    ]
    Timezone = struct [
      "int tz_minuteswest",
      "int tz_dsttime",
    ]
    MyStruct = struct [
      "short num[5]",
      "char c",
      "unsigned char buff[7]",
    ]

    CallCallback = bind("void call_callback(void*, void*)"){|ptr1, ptr2|
      f = Function.new(CFunc.new(ptr1.to_i, DL::TYPE_VOID, "<anonymous>"), [TYPE_VOIDP])
      f.call(ptr2)
    }
    CarriedFunction = bind("void callback_function(void*)", :carried, 0)
  end

  class TestImport < TestBase
    def test_ensure_call_dlload
      err = assert_raises(RuntimeError) do
        Class.new do
          extend DL::Importer
          extern "void *strcpy(char*, char*)"
        end
      end
      assert_match(/call dlload before/, err.message)
    end

    def test_malloc()
      s1 = LIBC::Timeval.malloc()
      s2 = LIBC::Timeval.malloc()
      assert_not_equal(s1.to_ptr.to_i, s2.to_ptr.to_i)
    end

    def test_sizeof()
      assert_equal(DL::SIZEOF_VOIDP, LIBC.sizeof("FILE*"))
      assert_equal(LIBC::MyStruct.size(), LIBC.sizeof(LIBC::MyStruct))
    end

    def test_unsigned_result()
      d = (2 ** 31) + 1

      r = LIBC.strtoul(d.to_s, 0, 0)
      assert_equal(d, r)
    end

    def test_io()
      if( RUBY_PLATFORM != DL::BUILD_RUBY_PLATFORM )
        return
      end
      io_in,io_out = IO.pipe()
      LIBC.fprintf(io_out, "hello")
      io_out.flush()
      io_out.close()
      str = io_in.read()
      io_in.close()
      assert_equal("hello", str)
    end

    def test_value()
      i = LIBC.value('int', 2)
      assert_equal(2, i.value)

      d = LIBC.value('double', 2.0)
      assert_equal(2.0, d.value)

      ary = LIBC.value('int[3]', [0,1,2])
      assert_equal([0,1,2], ary.value)
    end

    def test_carried_function()
      data1 = "data"
      data2 = nil
      LIBC.call_callback(LIBC::CarriedFunction, LIBC::CarriedFunction.create_carrier(data1)){|d|
        data2 = d
      }
      assert_equal(data1, data2)
    end

    def test_struct()
      s = LIBC::MyStruct.malloc()
      s.num = [0,1,2,3,4]
      s.c = ?a.ord
      s.buff = "012345\377"
      assert_equal([0,1,2,3,4], s.num)
      assert_equal(?a.ord, s.c)
      assert_equal([?0.ord,?1.ord,?2.ord,?3.ord,?4.ord,?5.ord,?\377.ord], s.buff)
    end

    def test_gettimeofday()
      if( defined?(LIBC.gettimeofday) )
        timeval = LIBC::Timeval.malloc()
        timezone = LIBC::Timezone.malloc()
        LIBC.gettimeofday(timeval, timezone)
        cur = Time.now()
        assert(cur.to_i - 2 <= timeval.tv_sec && timeval.tv_sec <= cur.to_i)
      end
    end

    def test_strcpy()
      buff = "000"
      str = LIBC.strcpy(buff, "123")
      assert_equal("123", buff)
      assert_equal("123", str.to_s)
    end

    def test_isdigit()
      r1 = LIBC.isdigit(?1.ord)
      r2 = LIBC.isdigit(?2.ord)
      rr = LIBC.isdigit(?r.ord)
      assert_positive(r1)
      assert_positive(r2)
      assert_zero(rr)
    end

    def test_atof()
      r = LIBC.atof("12.34")
      assert_match(12.00..13.00, r)
    end

    def test_strtod()
      f = Function.new(CFunc.new(@libc['strtod'], TYPE_DOUBLE, 'strtod'),
                       [TYPE_VOIDP, TYPE_VOIDP])
      buff1 = "12.34"
      buff2 = "     "
      r = f.call(buff1, buff2)
      assert_match(12.00..13.00, r)
    end

    def test_qsort()
      buff = "9341"
      LIBC.qsort(buff, buff.size, 1, LIBC::QsortCallback){|ptr1,ptr2| ptr1[0] <=> ptr2[0]}
      assert_equal("1349", buff)
      buff = "9341"
      LIBC.qsort(buff, buff.size, 1, LIBC::BoundQsortCallback)
      assert_equal("1349", buff)
    end
  end
end
