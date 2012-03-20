begin
  require_relative 'helper'
rescue LoadError
end

module Fiddle
  class TestClosure < Fiddle::TestCase
    def test_argument_errors
      assert_raises(TypeError) do
        Closure.new(TYPE_INT, TYPE_INT)
      end

      assert_raises(TypeError) do
        Closure.new('foo', [TYPE_INT])
      end

      assert_raises(TypeError) do
        Closure.new(TYPE_INT, ['meow!'])
      end
    end

    def test_call
      closure = Class.new(Closure) {
        def call
          10
        end
      }.new(TYPE_INT, [])

      func = Function.new(closure, [], TYPE_INT)
      assert_equal 10, func.call
    end

    def test_returner
      closure = Class.new(Closure) {
        def call thing
          thing
        end
      }.new(TYPE_INT, [TYPE_INT])

      func = Function.new(closure, [TYPE_INT], TYPE_INT)
      assert_equal 10, func.call(10)
    end

    def test_block_caller
      cb = Closure::BlockCaller.new(TYPE_INT, [TYPE_INT]) do |one|
        one
      end
      func = Function.new(cb, [TYPE_INT], TYPE_INT)
      assert_equal 11, func.call(11)
    end

    def test_memsize
      require 'objspace'
      bug = '[ruby-dev:42480]'
      n = 10000
      assert_equal(n, n.times {ObjectSpace.memsize_of(Closure.allocate)}, bug)
    end

    %w[INT SHORT CHAR LONG LONG_LONG].each do |name|
      type = DL.const_get("TYPE_#{name}") rescue next
      size = DL.const_get("SIZEOF_#{name}")
      [[type, size-1, name], [-type, size, "unsigned_"+name]].each do |t, s, n|
        define_method("test_conversion_#{n.downcase}") do
          arg = nil

          clos = Class.new(Closure) do
            define_method(:call) {|x| arg = x}
          end.new(t, [t])

          v = ~(~0 << (8*s))

          arg = nil
          assert_equal(v, clos.call(v))
          assert_equal(arg, v, n)

          arg = nil
          func = Function.new(clos, [t], t)
          assert_equal(v, func.call(v))
          assert_equal(arg, v, n)
        end
      end
    end
  end
end if defined?(Fiddle)
