begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_VARIANT)

  class TestWIN32OLE_VARIANT < Test::Unit::TestCase

    def test_s_new
      obj = WIN32OLE_VARIANT.new('foo')
      assert_instance_of(WIN32OLE_VARIANT, obj)
    end

    def test_s_new_exc
      assert_raise(TypeError) {
        WIN32OLE_VARIANT.new(/foo/)
      }
    end

    def test_s_new_no_argument
      ex = nil
      begin
        obj = WIN32OLE_VARIANT.new
      rescue ArgumentError
        ex = $!
      end
      assert_instance_of(ArgumentError, ex)
      assert_equal("wrong number of arguments (0 for 1..3)", ex.message);
    end

    def test_s_new_one_argument
      ex = nil
      begin
        obj = WIN32OLE_VARIANT.new('foo')
      rescue
        ex = $!
      end
      assert_equal(nil, ex);
    end

    def test_s_new_with_nil
      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_I2)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I2, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_I4)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_R4)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R4, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_R8)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R8, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_CY)
      assert_equal("0", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_CY, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1899,12,30), obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_DATE, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_BSTR)
      assert_equal("", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BSTR, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_DISPATCH)
      assert_nil(obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_DISPATCH, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_BOOL)
      assert_equal(false, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BOOL, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_VARIANT)
      assert_nil(obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_VARIANT, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_I1)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I1, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_UI1)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI1, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_UI2)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI2, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_UI4)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI4, obj.vartype)


      if defined?(WIN32OLE::VARIANT::VT_I8)
        obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_I8)
        assert_equal(0, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_I8, obj.vartype)
      end

      if defined?(WIN32OLE::VARIANT::VT_UI8)
        obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_UI8)
        assert_equal(0, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_UI8, obj.vartype)
      end

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_INT)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_INT, obj.vartype)

      obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_UINT)
      assert_equal(0, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UINT, obj.vartype)
    end

    def test_s_new_with_non_nil
      obj = WIN32OLE_VARIANT.new(2, WIN32OLE::VARIANT::VT_I2)
      assert_equal(2, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I2, obj.vartype)

      obj = WIN32OLE_VARIANT.new(3, WIN32OLE::VARIANT::VT_I4)
      assert_equal(3, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4, obj.vartype)

      obj = WIN32OLE_VARIANT.new(4.5, WIN32OLE::VARIANT::VT_R4)
      assert_equal(4.5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R4, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5.5, WIN32OLE::VARIANT::VT_R8)
      assert_equal(5.5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R8, obj.vartype)

      obj = WIN32OLE_VARIANT.new(600, WIN32OLE::VARIANT::VT_CY)
      assert_equal("600", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_CY, obj.vartype)

      obj = WIN32OLE_VARIANT.new("2001-06-15 12:17:34", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(2001,06,15,12,17,34), obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_DATE, obj.vartype)

      obj = WIN32OLE_VARIANT.new("foo", WIN32OLE::VARIANT::VT_BSTR)
      assert_equal("foo", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BSTR, obj.vartype)

      obj = WIN32OLE_VARIANT.new(true, WIN32OLE::VARIANT::VT_BOOL)
      assert_equal(true, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BOOL, obj.vartype)

      obj = WIN32OLE_VARIANT.new(2, WIN32OLE::VARIANT::VT_I1)
      assert_equal(2, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I1, obj.vartype)

      obj = WIN32OLE_VARIANT.new(3, WIN32OLE::VARIANT::VT_UI1)
      assert_equal(3, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI1, obj.vartype)

      obj = WIN32OLE_VARIANT.new(4, WIN32OLE::VARIANT::VT_UI2)
      assert_equal(4, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI2, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5, WIN32OLE::VARIANT::VT_UI4)
      assert_equal(5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI4, obj.vartype)

      if defined?(WIN32OLE::VARIANT::VT_I8)
        obj = WIN32OLE_VARIANT.new(-123456789012345, WIN32OLE::VARIANT::VT_I8)
        assert_equal(-123456789012345, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_I8, obj.vartype)
      end

      if defined?(WIN32OLE::VARIANT::VT_UI8)
        obj = WIN32OLE_VARIANT.new(123456789012345, WIN32OLE::VARIANT::VT_UI8)
        assert_equal(123456789012345, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_UI8, obj.vartype)
      end

      obj = WIN32OLE_VARIANT.new(4, WIN32OLE::VARIANT::VT_INT)
      assert_equal(4, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_INT, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5, WIN32OLE::VARIANT::VT_UINT)
      assert_equal(5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UINT, obj.vartype)
    end

    def test_s_new_with_non_nil_byref
      obj = WIN32OLE_VARIANT.new(2, WIN32OLE::VARIANT::VT_I2|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(2, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I2|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(3, WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(3, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(4.5, WIN32OLE::VARIANT::VT_R4|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(4.5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R4|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5.5, WIN32OLE::VARIANT::VT_R8|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(5.5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_R8|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(600, WIN32OLE::VARIANT::VT_CY|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal("600", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_CY|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new("2001-06-15 12:17:34", WIN32OLE::VARIANT::VT_DATE|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(Time.new(2001,06,15,12,17,34), obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_DATE|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new("foo", WIN32OLE::VARIANT::VT_BSTR|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal("foo", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BSTR|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(true, WIN32OLE::VARIANT::VT_BOOL|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(true, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_BOOL|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(2, WIN32OLE::VARIANT::VT_I1|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(2, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I1|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(3, WIN32OLE::VARIANT::VT_UI1|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(3, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI1|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(4, WIN32OLE::VARIANT::VT_UI2|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(4, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI2|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5, WIN32OLE::VARIANT::VT_UI4|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UI4|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(4, WIN32OLE::VARIANT::VT_INT|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(4, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_INT|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)

      obj = WIN32OLE_VARIANT.new(5, WIN32OLE::VARIANT::VT_UINT|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(5, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_UINT|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)
    end

    # This test is failed in cygwin.
    # The tagVARIANT definition has no union member pllVal in cygwin.
    def test_s_new_with_i8_byref
      if defined?(WIN32OLE::VARIANT::VT_I8) && /mswin/ =~ RUBY_PLATFORM
        obj = WIN32OLE_VARIANT.new(-123456789012345, WIN32OLE::VARIANT::VT_I8|WIN32OLE::VARIANT::VT_BYREF)
        assert_equal(-123456789012345, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_I8|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)
      else
        skip("WIN32OLE::VARIANT::VT_I8 is not available")
      end
    end

    # This test is failed in cygwin.
    # The tagVARIANT definition has no union member pullVal in cygwin.
    def test_s_new_with_ui8_byref
      if defined?(WIN32OLE::VARIANT::VT_UI8) && /mswin/ =~ RUBY_PLATFORM
        obj = WIN32OLE_VARIANT.new(123456789012345, WIN32OLE::VARIANT::VT_UI8|WIN32OLE::VARIANT::VT_BYREF)
        assert_equal(123456789012345, obj.value)
        assert_equal(WIN32OLE::VARIANT::VT_UI8|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)
      else
        skip("WIN32OLE::VARIANT::VT_UI8 is not available")
      end
    end

    def test_value
      obj = WIN32OLE_VARIANT.new('foo')
      assert_equal('foo', obj.value)
    end

    def test_s_new_2_argument
      obj = WIN32OLE_VARIANT.new('foo', WIN32OLE::VARIANT::VT_BSTR|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal('foo', obj.value);
    end

    def test_s_new_2_argument2
      obj = WIN32OLE_VARIANT.new('foo', WIN32OLE::VARIANT::VT_BSTR)
      assert_equal('foo', obj.value);
    end

    def test_s_new_dispatch_array
      vt = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_DISPATCH
      obj = WIN32OLE_VARIANT.new(nil, vt)
      assert_equal(vt, obj.vartype)
      assert_nil(obj.value)

      vt = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_DISPATCH|WIN32OLE::VARIANT::VT_BYREF
      obj = WIN32OLE_VARIANT.new(nil, vt)
      assert_equal(vt, obj.vartype)
      assert_nil(obj.value)
    end

    def test_s_new_array
      # should not occur stack over flow
      ar = (1..500000).to_a.map{|i| [i]}
      ar2 = WIN32OLE_VARIANT.new(ar)
      assert_equal(ar, ar2.value)
    end

    def test_s_array
      obj = WIN32OLE_VARIANT.array([2,3], WIN32OLE::VARIANT::VT_I4)
      assert_instance_of(WIN32OLE_VARIANT, obj)
      assert_equal(WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_ARRAY, obj.vartype)
      assert_equal([[0, 0, 0],[0, 0, 0]], obj.value)

      obj = WIN32OLE_VARIANT.array([2,3], WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_BYREF|WIN32OLE::VARIANT::VT_ARRAY, obj.vartype)
      assert_equal([[0, 0, 0],[0, 0, 0]], obj.value)

      obj = WIN32OLE_VARIANT.array([2,3], WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_ARRAY)
      assert_instance_of(WIN32OLE_VARIANT, obj)
      assert_equal(WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_ARRAY, obj.vartype)
      assert_equal([[0, 0, 0],[0, 0, 0]], obj.value)

      assert_equal(0, obj[0,0])
      obj[0,0] = 10
      assert_equal([[10, 0, 0],[0, 0, 0]], obj.value)
      obj[0,1] = "13.2"
      assert_equal([[10, 13, 0],[0, 0, 0]], obj.value)

      obj = WIN32OLE_VARIANT.array([3, 2], WIN32OLE::VARIANT::VT_VARIANT)
      obj[0,0] = 10
      obj[0,1] = "string"
      obj[1,0] = 12.735
      assert_equal([[10, "string"],[12.735, nil],[nil,nil]], obj.value)

      obj = WIN32OLE_VARIANT.array([2,3], WIN32OLE::VARIANT::VT_DISPATCH)
      assert_equal([[nil, nil, nil],[nil,nil,nil]], obj.value)

    end

    def test_s_array_exc
      assert_raise(TypeError) {
        obj = WIN32OLE_VARIANT.array(2, WIN32OLE::VARIANT::VT_I4)
      }
    end

    def test_conversion_num2str
      obj = WIN32OLE_VARIANT.new(124, WIN32OLE::VARIANT::VT_BSTR)
      assert_equal("124", obj.value);
    end

    def test_conversion_float2int
      obj = WIN32OLE_VARIANT.new(12.345, WIN32OLE::VARIANT::VT_I4)
      assert_equal(12, obj.value)
      obj = WIN32OLE_VARIANT.new(12.345, WIN32OLE::VARIANT::VT_I4|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal(12, obj.value)
    end

    def test_conversion_str2num
      obj = WIN32OLE_VARIANT.new("12.345", WIN32OLE::VARIANT::VT_R8)
      assert_equal(12.345, obj.value)
    end

    def test_conversion_ole_variant2ole_variant
      obj = WIN32OLE_VARIANT.new("12.345", WIN32OLE::VARIANT::VT_R4)
      obj = WIN32OLE_VARIANT.new(obj, WIN32OLE::VARIANT::VT_I4)
      assert_equal(12, obj.value)
    end

    def test_conversion_str2date
      obj = WIN32OLE_VARIANT.new("2004-12-24 12:24:45", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(2004,12,24,12,24,45), obj.value)
    end

    def test_conversion_time2date
      dt = Time.mktime(2004, 12, 24, 12, 24, 45)
      obj = WIN32OLE_VARIANT.new(dt, WIN32OLE::VARIANT::VT_DATE)
      assert_equal(dt, obj.value)
    end

    # this test failed because of VariantTimeToSystemTime
    # and SystemTimeToVariantTime API ignores wMilliseconds
    # member of SYSTEMTIME  struct.
    #
    # def test_conversion_time_nsec2date
    #   dt = Time.new(2004, 12,24, 12, 24, 45)
    #   dt += 0.1
    #   obj = WIN32OLE_VARIANT.new(dt, WIN32OLE::VARIANT::VT_DATE)
    #   assert_equal(dt, obj.value)
    # end

    def test_conversion_str2cy
      begin
        begin
          WIN32OLE.locale = 0x0411 # set locale Japanese
        rescue WIN32OLERuntimeError
          skip("Japanese locale is not installed")
        end
        if WIN32OLE.locale == 0x0411
          obj = WIN32OLE_VARIANT.new("\\10,000", WIN32OLE::VARIANT::VT_CY)
          assert_equal("10000", obj.value)
        end
      ensure
        WIN32OLE.locale = WIN32OLE::LOCALE_SYSTEM_DEFAULT
      end
    end

    def test_create_vt_array
      obj = WIN32OLE_VARIANT.new([1.2, 2.3], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_R8)
      assert_equal([1.2, 2.3], obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_R8, obj.vartype)

      obj = WIN32OLE_VARIANT.new([1.2, 2.3], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_R8|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal([1.2, 2.3], obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_R8|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)
    end

    def test_create_vt_array2
      obj = WIN32OLE_VARIANT.new([1.2, "a"], WIN32OLE::VARIANT::VT_ARRAY)
      assert_equal([1.2, "a"], obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_VARIANT, obj.vartype)

      obj = WIN32OLE_VARIANT.new([1.2, "a"])
      assert_equal([1.2, "a"], obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_VARIANT, obj.vartype)
    end


    def test_create_vt_nested_array
      obj = WIN32OLE_VARIANT.new([[1.2, "a", "b"], [3.4, "C", "D"]], WIN32OLE::VARIANT::VT_ARRAY)
      assert_equal([[1.2, "a", "b"], [3.4, "C", "D"]], obj.value)

      obj = WIN32OLE_VARIANT.new([[1.2, "a", "b"], [3.4, "C", "D"]])
      assert_equal([[1.2, "a", "b"], [3.4, "C", "D"]], obj.value)

      obj = WIN32OLE_VARIANT.new([[1.2, "a", "b"], [3.4, "C", "D"], [5.6, "E", "F"]])
      assert_equal([[1.2, "a", "b"], [3.4, "C", "D"], [5.6, "E", "F"]], obj.value)

      obj = WIN32OLE_VARIANT.new([[[1.2], [3.4]], [[5.6], [7.8]], [[9.1],[9.2]]])
      assert_equal([[[1.2], [3.4]], [[5.6], [7.8]], [[9.1],[9.2]]], obj.value)
    end

    def test_create_vt_array3
      obj = WIN32OLE_VARIANT.new([])
      assert_equal([], obj.value)

      obj = WIN32OLE_VARIANT.new([[]])
      assert_equal([[]], obj.value)

      obj = WIN32OLE_VARIANT.new([[],[]])
      assert_equal([[],[]], obj.value)

      obj = WIN32OLE_VARIANT.new([], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal([], obj.value)

      obj = WIN32OLE_VARIANT.new([[]], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal([[]], obj.value)

      obj = WIN32OLE_VARIANT.new([[],[]], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal([[],[]], obj.value)
    end

    def test_create_vt_array_nil
      vartype = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_DISPATCH|WIN32OLE::VARIANT::VT_BYREF
      obj = WIN32OLE_VARIANT.new(nil, vartype)
      assert_nil(obj.value)
      assert_equal(vartype, obj.vartype)

      vartype = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_DISPATCH
      obj = WIN32OLE_VARIANT.new(nil, vartype)
      assert_nil(obj.value)
      assert_equal(vartype, obj.vartype)
    end

    def test_create_vt_array_str
      vartype = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_BSTR
      obj = WIN32OLE_VARIANT.new(["abc", "123"], vartype)
      assert_equal(vartype, obj.vartype)
      assert_equal(["abc", "123"], obj.value)

      vartype = WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_BYREF|WIN32OLE::VARIANT::VT_BSTR
      obj = WIN32OLE_VARIANT.new(["abc", "123"], vartype)
      assert_equal(vartype, obj.vartype)
      assert_equal(["abc", "123"], obj.value)
    end

    def test_create_vt_array_exc
      exc = assert_raise(TypeError) {
        obj = WIN32OLE_VARIANT.new(1, WIN32OLE::VARIANT::VT_ARRAY);
      }
      assert_match(/wrong argument type Fixnum \(expected Array\)/, exc.message)
    end

    def test_create_vt_array_str2ui1array
      obj = WIN32OLE_VARIANT.new("ABC", WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_UI1)
      assert_equal("ABC", obj.value)

      obj.value = "DEF"
      assert_equal("DEF", obj.value)
      obj[0] = 71
      assert_equal("GEF", obj.value)

      obj = WIN32OLE_VARIANT.new([65, 0].pack("C*"), WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_UI1)
      assert_equal([65, 0].pack("C*"), obj.value)

      obj = WIN32OLE_VARIANT.new("abc", WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_UI1|WIN32OLE::VARIANT::VT_BYREF)
      assert_equal("abc", obj.value)
      obj.value = "DEF"
      assert_equal("DEF", obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_UI1|WIN32OLE::VARIANT::VT_BYREF, obj.vartype)
      obj[1] = 71
      assert_equal("DGF", obj.value)

    end

    def test_create_vt_array_int
      obj = WIN32OLE_VARIANT.new([65, 0], WIN32OLE::VARIANT::VT_ARRAY|WIN32OLE::VARIANT::VT_UI1)
      assert_equal([65, 0].pack("C*"), obj.value)

      obj = WIN32OLE_VARIANT.new([65, 0])
      assert_equal([65, 0], obj.value)

      obj = WIN32OLE_VARIANT.new([65, 0], WIN32OLE::VARIANT::VT_I2|WIN32OLE::VARIANT::VT_ARRAY)
      assert_equal([65, 0], obj.value)

    end

    def test_vt_array_bracket
      obj = WIN32OLE_VARIANT.new([[1,2,3],[4,5,6]])
      assert_equal(1, obj[0,0])
      assert_equal(2, obj[0,1])
      assert_equal(3, obj[0,2])
      assert_equal(4, obj[1,0])
      assert_equal(5, obj[1,1])
      assert_equal(6, obj[1,2])

      assert_raise(WIN32OLERuntimeError) {
        obj[0,4]
      }
      assert_raise(WIN32OLERuntimeError) {
        obj[0,-1]
      }
      assert_raise(ArgumentError) {
        obj[0]
      }

      obj[0,0] = 7
      obj[1,2] = 8
      assert_equal([[7,2,3], [4,5,8]], obj.value)

      assert_raise(WIN32OLERuntimeError) {
        obj[0,4] = 9
      }
      assert_raise(WIN32OLERuntimeError) {
        obj[0,-1] = 10
      }
      assert_raise(ArgumentError) {
        obj[0] = 11
      }
    end

    def test_conversion_vt_date
      obj = WIN32OLE_VARIANT.new(-657434, WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(100,1,1), obj.value)

      obj = WIN32OLE_VARIANT.new("1500/12/29 23:59:59", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1500,12,29,23,59,59), obj.value)

      obj = WIN32OLE_VARIANT.new("1500/12/30 00:00:00", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1500,12,30), obj.value)

      obj = WIN32OLE_VARIANT.new("1500/12/30 00:00:01", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1500,12,30,0,0,1), obj.value)

      obj = WIN32OLE_VARIANT.new("1899/12/29 23:59:59", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1899,12,29,23,59,59), obj.value)

      obj = WIN32OLE_VARIANT.new("1899/12/30 00:00:00", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1899,12,30), obj.value)

      obj = WIN32OLE_VARIANT.new("1899/12/30 00:00:01", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1899,12,30,0,0,1), obj.value)

      obj = WIN32OLE_VARIANT.new(0, WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(1899,12,30), obj.value)

      obj = WIN32OLE_VARIANT.new("2008/12/29 23:59:59", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(2008,12,29,23,59,59), obj.value)

      obj = WIN32OLE_VARIANT.new("2008/12/30 00:00:00", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(2008,12,30,0,0,0), obj.value)

      obj = WIN32OLE_VARIANT.new("2008/12/30 00:00:01", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(2008,12,30,0,0,1), obj.value)

      obj = WIN32OLE_VARIANT.new("9999/12/31 23:59:59", WIN32OLE::VARIANT::VT_DATE)
      assert_equal(Time.new(9999,12,31,23,59,59), obj.value)
    end

    def test_create_nil_dispatch
      var = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_DISPATCH)
      assert_nil(var.value)
    end

    def test_create_variant_byref
      obj = WIN32OLE_VARIANT.new("Str", WIN32OLE::VARIANT::VT_VARIANT|WIN32OLE::VARIANT::VT_BYREF);
      assert_equal("Str", obj.value);
    end

    def test_vartype
      obj = WIN32OLE_VARIANT.new("Str")
      assert_equal(WIN32OLE::VARIANT::VT_BSTR, obj.vartype)
    end

    def test_set_value
      obj = WIN32OLE_VARIANT.new(10)
      obj.value = 12
      assert_equal(12, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4, obj.vartype)
      obj.value = "14"
      assert_equal(14, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4, obj.vartype)
      obj.value = 11.2
      assert_equal(11, obj.value)
      assert_equal(WIN32OLE::VARIANT::VT_I4, obj.vartype)

      obj = WIN32OLE_VARIANT.new([1,2])
      assert_raise(WIN32OLERuntimeError) {
        obj.value = [3,4]
      }

      obj = WIN32OLE_VARIANT.new("2007/01/01", WIN32OLE::VARIANT::VT_DATE)
      assert_raise(WIN32OLERuntimeError) {
        obj.value = "hogehoge"
      }
      assert_equal(Time.new(2007,1,1), obj.value)

      obj2 = WIN32OLE_VARIANT.new("2006/01/01", WIN32OLE::VARIANT::VT_DATE)
      obj.value = obj2
      assert_equal(Time.new(2006,01,01), obj.value)
    end

    def test_c_nothing
      assert_nil(WIN32OLE_VARIANT::Nothing.value)
    end

    def test_c_empty
      assert_nil(WIN32OLE_VARIANT::Empty.value)
    end

    def test_c_null
      assert_nil(WIN32OLE_VARIANT::Null.value)
    end

  end
end

