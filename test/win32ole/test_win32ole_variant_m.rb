begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE::VARIANT)
  class TestWin32OLE_VARIANT_MODULE < Test::Unit::TestCase
    include WIN32OLE::VARIANT
    def test_variant
      assert_equal(0, VT_EMPTY)
      assert_equal(1, VT_NULL)
      assert_equal(2, VT_I2)
      assert_equal(3, VT_I4)
      assert_equal(4, VT_R4)
      assert_equal(5, VT_R8)
      assert_equal(6, VT_CY)
      assert_equal(7, VT_DATE)
      assert_equal(8, VT_BSTR)
      assert_equal(9, VT_DISPATCH)
      assert_equal(10, VT_ERROR)
      assert_equal(11, VT_BOOL)
      assert_equal(12, VT_VARIANT)
      assert_equal(13, VT_UNKNOWN)
      assert_equal(16, VT_I1)
      assert_equal(17, VT_UI1)
      assert_equal(18, VT_UI2)
      assert_equal(19, VT_UI4)
      assert_equal(22, VT_INT)
      assert_equal(23, VT_UINT)
      assert_equal(0x2000, VT_ARRAY)
      assert_equal(0x4000, VT_BYREF)
    end
  end
end
