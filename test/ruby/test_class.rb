require 'test/unit'
require_relative 'envutil'

class TestClass < Test::Unit::TestCase
  # ------------------
  # Various test classes
  # ------------------

  class ClassOne
    attr :num_args
    @@subs = []
    def initialize(*args)
      @num_args = args.size
      @args = args
    end
    def [](n)
      @args[n]
    end
    def ClassOne.inherited(klass)
      @@subs.push klass
    end
    def subs
      @@subs
    end
  end

  class ClassTwo < ClassOne
  end

  class ClassThree < ClassOne
  end

  class ClassFour < ClassThree
  end

  # ------------------
  # Start of tests
  # ------------------

  def test_s_inherited
    assert_equal([ClassTwo, ClassThree, ClassFour], ClassOne.new.subs)
  end

  def test_s_new
    c = Class.new
    assert_same(Class, c.class)
    assert_same(Object, c.superclass)

    c = Class.new(Fixnum)
    assert_same(Class, c.class)
    assert_same(Fixnum, c.superclass)
  end

  def test_00_new_basic
    a = ClassOne.new
    assert_equal(ClassOne, a.class)
    assert_equal(0, a.num_args)

    a = ClassOne.new(1, 2, 3)
    assert_equal(3, a.num_args)
    assert_equal(1, a[0])
  end

  def test_01_new_inherited
    a = ClassTwo.new
    assert_equal(ClassTwo, a.class)
    assert_equal(0, a.num_args)

    a = ClassTwo.new(1, 2, 3)
    assert_equal(3, a.num_args)
    assert_equal(1, a[0])
  end

  def test_superclass
    assert_equal(ClassOne, ClassTwo.superclass)
    assert_equal(Object,   ClassTwo.superclass.superclass)
    assert_equal(BasicObject, ClassTwo.superclass.superclass.superclass)
  end

  def test_class_cmp
    assert_raise(TypeError) { Class.new <= 1 }
    assert_raise(TypeError) { Class.new >= 1 }
    assert_nil(Class.new <=> 1)
  end

  def test_class_initialize
    assert_raise(TypeError) do
      Class.new.instance_eval { initialize }
    end
  end

  def test_instanciate_singleton_class
    c = class << Object.new; self; end
    assert_raise(TypeError) { c.new }
  end

  def test_superclass_of_basicobject
    assert_equal(nil, BasicObject.superclass)
  end

  def test_module_function
    c = Class.new
    assert_raise(TypeError) do
      Module.instance_method(:module_function).bind(c).call(:foo)
    end
  end

  def test_method_redefinition
    feature2155 = '[ruby-dev:39400]'

    line = __LINE__+4
    stderr = EnvUtil.verbose_warning do
      Class.new do
        def foo; end
        def foo; end
      end
    end
    assert_match(/:#{line}: warning: method redefined; discarding old foo/, stderr)
    assert_match(/:#{line-1}: warning: previous definition of foo/, stderr, feature2155)

    stderr = EnvUtil.verbose_warning do
      Class.new do
        def foo; end
        alias bar foo
        def foo; end
      end
    end
    assert_equal("", stderr)

    stderr = EnvUtil.verbose_warning do
      Class.new do
        def foo; end
        alias bar foo
        alias bar foo
      end
    end
    assert_equal("", stderr)

    line = __LINE__+4
    stderr = EnvUtil.verbose_warning do
      Class.new do
        define_method(:foo) do end
        def foo; end
      end
    end
    assert_match(/:#{line}: warning: method redefined; discarding old foo/, stderr)
    assert_match(/:#{line-1}: warning: previous definition of foo/, stderr, feature2155)

    stderr = EnvUtil.verbose_warning do
      Class.new do
        define_method(:foo) do end
        alias bar foo
        alias bar foo
      end
    end
    assert_equal("", stderr)

    stderr = EnvUtil.verbose_warning do
      Class.new do
        def foo; end
        undef foo
      end
    end
    assert_equal("", stderr)
  end

  def test_check_inheritable
    assert_raise(TypeError) { Class.new(Object.new) }

    o = Object.new
    c = class << o; self; end
    assert_raise(TypeError) { Class.new(c) }
    assert_raise(TypeError) { Class.new(Class) }
    assert_raise(TypeError) { eval("class Foo < Class; end") }
  end

  def test_initialize_copy
    c = Class.new
    assert_raise(TypeError) { c.instance_eval { initialize_copy(1) } }

    o = Object.new
    c = class << o; self; end
    assert_raise(TypeError) { c.dup }

    assert_raise(TypeError) { BasicObject.dup }
  end

  def test_singleton_class
    assert_raise(TypeError) { 1.extend(Module.new) }
    assert_raise(TypeError) { :foo.extend(Module.new) }

    assert_in_out_err([], <<-INPUT, %w(:foo :foo true true), [])
      module Foo; def foo; :foo; end; end
      false.extend(Foo)
      true.extend(Foo)
      p false.foo
      p true.foo
      p FalseClass.include?(Foo)
      p TrueClass.include?(Foo)
    INPUT
  end

  def test_uninitialized
    assert_raise(TypeError) { Class.allocate.new }
    assert_raise(TypeError) { Class.allocate.superclass }
  end

  def test_nonascii_name
    c = eval("class ::C\u{df}; self; end")
    assert_equal("C\u{df}", c.name, '[ruby-core:24600]')
    c = eval("class C\u{df}; self; end")
    assert_equal("TestClass::C\u{df}", c.name, '[ruby-core:24600]')
  end

  def test_invalid_jump_from_class_definition
    assert_raise(SyntaxError) { eval("class C; next; end") }
    assert_raise(SyntaxError) { eval("class C; break; end") }
    assert_raise(SyntaxError) { eval("class C; redo; end") }
    assert_raise(SyntaxError) { eval("class C; retry; end") }
    assert_raise(SyntaxError) { eval("class C; return; end") }
    assert_raise(SyntaxError) { eval("class C; yield; end") }
  end

  def test_clone
    original = Class.new {
      def foo
        return super()
      end
    }
    mod = Module.new {
      def foo
        return "mod#foo"
      end
    }
    copy = original.clone
    copy.send(:include, mod)
    assert_equal("mod#foo", copy.new.foo)
  end

  def test_nested_class_removal
    assert_normal_exit('File.__send__(:remove_const, :Stat); at_exit{File.stat(".")}; GC.start')
  end

  class PrivateClass
  end
  private_constant :PrivateClass

  def test_redefine_private_class
    assert_raise(NameError) do
      eval("class ::TestClass::PrivateClass; end")
    end
    eval <<-END
      class ::TestClass
        class PrivateClass
          def foo; 42; end
        end
      end
    END
    assert_equal(42, PrivateClass.new.foo)
  end

  StrClone = String.clone
  Class.new(StrClone)

  def test_cloned_class
    bug5274 = StrClone.new("[ruby-dev:44460]")
    assert_equal(bug5274, Marshal.load(Marshal.dump(bug5274)))
  end
end
