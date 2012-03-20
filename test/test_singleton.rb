require 'test/unit'
require 'singleton'

class TestSingleton < Test::Unit::TestCase
  class SingletonTest
    include Singleton
  end

  def test_marshal
    o1 = SingletonTest.instance
    m = Marshal.dump(o1)
    o2 = Marshal.load(m)
    assert_same(o1, o2)
  end

  def test_instance_never_changes
    a = SingletonTest.instance
    b = SingletonTest.instance
    assert_same a, b
  end

  def test_initialize_raises_exception
    assert_raises NoMethodError do
      SingletonTest.new
    end
  end

  def test_allocate_raises_exception
    assert_raises NoMethodError do
      SingletonTest.allocate
    end
  end

  def test_clone_raises_exception
    exception = assert_raises TypeError do
      SingletonTest.instance.clone
    end

    expected = "can't clone instance of singleton TestSingleton::SingletonTest"

    assert_equal expected, exception.message
  end

  def test_dup_raises_exception
    exception = assert_raises TypeError do
      SingletonTest.instance.dup
    end

    expected = "can't dup instance of singleton TestSingleton::SingletonTest"

    assert_equal expected, exception.message
  end

  def test_include_in_module_raises_exception
    mod = Module.new

    exception = assert_raises TypeError do
      mod.class_eval do
        include Singleton
      end
    end

    expected = "Inclusion of the OO-Singleton module in module #{mod}"

    assert_equal expected, exception.message
  end

  def test_extending_singleton_raises_exception
    assert_raises NoMethodError do
      'foo'.extend Singleton
    end
  end

  def test_inheritance_works_with_overridden_inherited_method
    super_super_called = false

    outer = Class.new do
      define_singleton_method :inherited do |sub|
        super_super_called = true
      end
    end

    inner = Class.new(outer) do
      include Singleton
    end

    tester = Class.new(inner)

    assert super_super_called

    a = tester.instance
    b = tester.instance
    assert_same a, b
  end

  def test_class_level_cloning_preserves_singleton_behavior
    klass = SingletonTest.clone

    a = klass.instance
    b = klass.instance
    assert_same a, b
  end
end
