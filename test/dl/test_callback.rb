require_relative 'test_base'
require_relative '../ruby/envutil'
require 'dl/callback'
require 'dl/func'

module DL
  class TestCallback < TestBase
    include DL

    def test_remove_callback_failed
      assert_equal(false, remove_callback(0, TYPE_VOIDP))
    end

    def test_remove_callback
      addr = set_callback(TYPE_VOIDP, 1) do |str|
        str
      end
      assert remove_callback(addr, TYPE_VOIDP), 'callback removed'
    end

    def test_callback_return_value
      addr = set_callback(TYPE_VOIDP, 1) do |str|
        str
      end
      func = CFunc.new(addr, TYPE_VOIDP, 'test')
      f = Function.new(func, [TYPE_VOIDP])
      ptr = CPtr['blah']
      assert_equal ptr, f.call(ptr)
    end

    def test_callback_return_arbitrary
      foo = 'foo'
      addr = set_callback(TYPE_VOIDP, 1) do |ptr|
        CPtr[foo].to_i
      end
      func = CFunc.new(addr, TYPE_VOIDP, 'test')
      f = Function.new(func, [TYPE_VOIDP])

      ptr = CPtr['foo']
      assert_equal 'foo', f.call(ptr).to_s
    end

    def test_callback_with_string
      called_with = nil
      addr = set_callback(TYPE_VOID, 1) do |str|
        called_with = dlunwrap(str)
      end
      func = CFunc.new(addr, TYPE_VOID, 'test')
      f = Function.new(func, [TYPE_VOIDP])

      # Don't remove local variable arg.
      # This necessary to protect objects from GC.
      arg = 'foo'
      f.call(dlwrap(arg))
      assert_equal arg, called_with
    end

    def test_call_callback
      called = false

      addr = set_callback(TYPE_VOID, 1) do |foo|
        called = true
      end

      func = CFunc.new(addr, TYPE_VOID, 'test')
      f = Function.new(func, [TYPE_VOIDP])
      f.call(nil)

      assert called, 'function should be called'
    end
  end
end
