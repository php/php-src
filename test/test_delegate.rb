require 'test/unit'
require 'delegate'

class TestDelegateClass < Test::Unit::TestCase
  module M
    attr_reader :m
  end

  def test_extend
    obj = DelegateClass(Array).new([])
    obj.instance_eval { @m = :m }
    obj.extend M
    assert_equal(:m, obj.m, "[ruby-dev:33116]")
  end

  def test_systemcallerror_eq
    e = SystemCallError.new(0)
    assert((SimpleDelegator.new(e) == e) == (e == SimpleDelegator.new(e)), "[ruby-dev:34808]")
  end

  class Myclass < DelegateClass(Array);end

  def test_delegateclass_class
    myclass=Myclass.new([])
    assert_equal(Myclass,myclass.class)
    assert_equal(Myclass,myclass.dup.class,'[ruby-dev:40313]')
    assert_equal(Myclass,myclass.clone.class,'[ruby-dev:40313]')
  end

  def test_simpledelegator_class
    simple=SimpleDelegator.new([])
    assert_equal(SimpleDelegator,simple.class)
    assert_equal(SimpleDelegator,simple.dup.class)
    assert_equal(SimpleDelegator,simple.clone.class)
  end

  class Object
    def m
      :o
    end
    private
    def delegate_test_m
      :o
    end
  end

  class Foo
    def m
      :m
    end
    def delegate_test_m
      :m
    end
  end

  class Bar < DelegateClass(Foo)
  end

  def test_override
    foo = Foo.new
    foo2 = SimpleDelegator.new(foo)
    bar = Bar.new(foo)
    assert_equal(:o, Object.new.m)
    assert_equal(:m, foo.m)
    assert_equal(:m, foo2.m)
    assert_equal(:m, bar.m)
    bug = '[ruby-dev:39154]'
    assert_equal(:m, foo2.send(:delegate_test_m), bug)
    assert_equal(:m, bar.send(:delegate_test_m), bug)
  end

  class IV < DelegateClass(Integer)
    attr_accessor :var

    def initialize
      @var = 1
      super(0)
    end
  end

  def test_marshal
    bug1744 = '[ruby-core:24211]'
    c = IV.new
    assert_equal(1, c.var)
    d = Marshal.load(Marshal.dump(c))
    assert_equal(1, d.var, bug1744)
  end

  def test_copy_frozen
    bug2679 = '[ruby-dev:40242]'
    a = [42, :hello].freeze
    d = SimpleDelegator.new(a)
    assert_nothing_raised(bug2679) {d.dup[0] += 1}
    assert_raise(RuntimeError) {d.clone[0] += 1}
    d.freeze
    assert(d.clone.frozen?)
    assert(!d.dup.frozen?)
  end

  def test_frozen
    d = SimpleDelegator.new([1, :foo])
    d.freeze
    assert_raise(RuntimeError, '[ruby-dev:40314]#1') {d.__setobj__("foo")}
    assert_equal([1, :foo], d)
  end

  def test_instance_method
    s = SimpleDelegator.new("foo")
    m = s.method("upcase")
    s.__setobj__([1,2,3])
    assert_raise(NoMethodError, '[ruby-dev:40314]#3') {m.call}
  end

  def test_methods
    s = SimpleDelegator.new("foo")
    assert_equal([], s.methods(false))
    def s.bar; end
    assert_equal([:bar], s.methods(false))
  end

  class Foo
    private
    def delegate_test_private
      :m
    end
  end

  def test_private_method
    foo = Foo.new
    d = SimpleDelegator.new(foo)
    assert_raise(NoMethodError) {foo.delegate_test_private}
    assert_equal(:m, foo.send(:delegate_test_private))
    assert_raise(NoMethodError, '[ruby-dev:40314]#4') {d.delegate_test_private}
    assert_raise(NoMethodError, '[ruby-dev:40314]#5') {d.send(:delegate_test_private)}
  end
end
