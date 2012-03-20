begin
  require_relative 'helper'
rescue LoadError
end

class TestFiddle < Fiddle::TestCase
  def test_constants_match
    [
      :TYPE_VOID,
      :TYPE_VOIDP,
      :TYPE_CHAR,
      :TYPE_SHORT,
      :TYPE_INT,
      :TYPE_LONG,
      :TYPE_LONG_LONG,
      :TYPE_FLOAT,
      :TYPE_DOUBLE,
      :TYPE_SIZE_T,
      :TYPE_SSIZE_T,
      :TYPE_PTRDIFF_T,
      :TYPE_INTPTR_T,
      :TYPE_UINTPTR_T,
    ].each do |name|
      assert_equal(DL.const_get(name), Fiddle.const_get(name), "Fiddle::#{name}")
    end
  end

  def test_windows_constant
    require 'rbconfig'
    if RbConfig::CONFIG['host_os'] =~ /mswin|mingw/
      assert Fiddle::WINDOWS, "Fiddle::WINDOWS should be 'true' on Windows platforms"
    else
      refute Fiddle::WINDOWS, "Fiddle::WINDOWS should be 'false' on non-Windows platforms"
    end
  end

end if defined?(Fiddle)
