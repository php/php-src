#
# This is test for [ruby-talk:196897]
#
begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE)
  class TestWIN32OLE_FOR_PROPERTYPUTREF < Test::Unit::TestCase

    def setup
      @obj = WIN32OLE.new('Scripting.Dictionary')
    end

    def test_ole_methods
      x = @obj.ole_methods.select {|m|
        m.invoke_kind == 'PROPERTYPUTREF'
      }
      assert(x.size > 0)
      assert_equal(1, x.size)
      assert_equal('Item', x[0].name)
    end

    def test_ole_put_methods
      x = @obj.ole_put_methods.select {|m|
        m.invoke_kind == 'PROPERTYPUTREF'
      }
      assert(x.size > 0)
      assert_equal(1, x.size)
      assert_equal('Item', x[0].name)
    end

  end
end
