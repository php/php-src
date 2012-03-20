require 'test/unit'
require 'pp'
require_relative 'envutil'

$m0 = Module.nesting

class TestModule < Test::Unit::TestCase
  def _wrap_assertion
    yield
  end

  def assert_method_defined?(klass, mid, message="")
    message = build_message(message, "#{klass}\##{mid} expected to be defined.")
    _wrap_assertion do
      klass.method_defined?(mid) or
        raise Test::Unit::AssertionFailedError, message, caller(3)
    end
  end

  def assert_method_not_defined?(klass, mid, message="")
    message = build_message(message, "#{klass}\##{mid} expected to not be defined.")
    _wrap_assertion do
      klass.method_defined?(mid) and
        raise Test::Unit::AssertionFailedError, message, caller(3)
    end
  end

  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_LT_0
    assert_equal true, String < Object
    assert_equal false, Object < String
    assert_nil String < Array
    assert_equal true, Array < Enumerable
    assert_equal false, Enumerable < Array
    assert_nil Proc < Comparable
    assert_nil Comparable < Proc
  end

  def test_GT_0
    assert_equal false, String > Object
    assert_equal true, Object > String
    assert_nil String > Array
    assert_equal false, Array > Enumerable
    assert_equal true, Enumerable > Array
    assert_nil Comparable > Proc
    assert_nil Proc > Comparable
  end

  def test_CMP_0
    assert_equal(-1, (String <=> Object))
    assert_equal 1, (Object <=> String)
    assert_nil(Array <=> String)
  end

  ExpectedException = NoMethodError

  # Support stuff

  module Mixin
    MIXIN = 1
    def mixin
    end
  end

  module User
    USER = 2
    include Mixin
    def user
    end
  end

  module Other
    def other
    end
  end

  class AClass
    def AClass.cm1
      "cm1"
    end
    def AClass.cm2
      cm1 + "cm2" + cm3
    end
    def AClass.cm3
      "cm3"
    end

    private_class_method :cm1, "cm3"

    def aClass
      :aClass
    end

    def aClass1
      :aClass1
    end

    def aClass2
      :aClass2
    end

    private :aClass1
    protected :aClass2
  end

  class BClass < AClass
    def bClass1
      :bClass1
    end

    private

    def bClass2
      :bClass2
    end

    protected
    def bClass3
      :bClass3
    end
  end

  class CClass < BClass
    def self.cClass
    end
  end

  MyClass = AClass.clone
  class MyClass
    public_class_method :cm1
  end

  # -----------------------------------------------------------

  def test_CMP # '<=>'
    assert_equal( 0, Mixin <=> Mixin)
    assert_equal(-1, User <=> Mixin)
    assert_equal( 1, Mixin <=> User)

    assert_equal( 0, Object <=> Object)
    assert_equal(-1, String <=> Object)
    assert_equal( 1, Object <=> String)
  end

  def test_GE # '>='
    assert(Mixin >= User)
    assert(Mixin >= Mixin)
    assert(!(User >= Mixin))

    assert(Object >= String)
    assert(String >= String)
    assert(!(String >= Object))
  end

  def test_GT # '>'
    assert(Mixin   > User)
    assert(!(Mixin > Mixin))
    assert(!(User  > Mixin))

    assert(Object > String)
    assert(!(String > String))
    assert(!(String > Object))
  end

  def test_LE # '<='
    assert(User <= Mixin)
    assert(Mixin <= Mixin)
    assert(!(Mixin <= User))

    assert(String <= Object)
    assert(String <= String)
    assert(!(Object <= String))
  end

  def test_LT # '<'
    assert(User < Mixin)
    assert(!(Mixin < Mixin))
    assert(!(Mixin < User))

    assert(String < Object)
    assert(!(String < String))
    assert(!(Object < String))
  end

  def test_VERY_EQUAL # '==='
    assert(Object === self)
    assert(Test::Unit::TestCase === self)
    assert(TestModule === self)
    assert(!(String === self))
  end

  def test_ancestors
    assert_equal([User, Mixin],      User.ancestors)
    assert_equal([Mixin],            Mixin.ancestors)

    ancestors = Object.ancestors
    mixins = ancestors - [Object, Kernel, BasicObject]
    mixins << JSON::Ext::Generator::GeneratorMethods::String if defined?(JSON::Ext::Generator::GeneratorMethods::String)
    assert_equal([Object, Kernel, BasicObject], ancestors - mixins)
    assert_equal([String, Comparable, Object, Kernel, BasicObject], String.ancestors - mixins)
  end

  CLASS_EVAL = 2
  @@class_eval = 'b'

  def test_class_eval
    Other.class_eval("CLASS_EVAL = 1")
    assert_equal(1, Other::CLASS_EVAL)
    assert(Other.constants.include?(:CLASS_EVAL))
    assert_equal(2, Other.class_eval { CLASS_EVAL })

    Other.class_eval("@@class_eval = 'a'")
    assert_equal('a', Other.class_variable_get(:@@class_eval))
    assert_equal('b', Other.class_eval { @@class_eval })

    Other.class_eval do
      module_function

      def class_eval_test
        "foo"
      end
    end
    assert_equal("foo", Other.class_eval_test)

    assert_equal([Other], Other.class_eval { |*args| args })
  end

  def test_const_defined?
    assert(Math.const_defined?(:PI))
    assert(Math.const_defined?("PI"))
    assert(!Math.const_defined?(:IP))
    assert(!Math.const_defined?("IP"))
  end

  def test_const_get
    assert_equal(Math::PI, Math.const_get("PI"))
    assert_equal(Math::PI, Math.const_get(:PI))
  end

  def test_const_set
    assert(!Other.const_defined?(:KOALA))
    Other.const_set(:KOALA, 99)
    assert(Other.const_defined?(:KOALA))
    assert_equal(99, Other::KOALA)
    Other.const_set("WOMBAT", "Hi")
    assert_equal("Hi", Other::WOMBAT)
  end

  def test_constants
    assert_equal([:MIXIN], Mixin.constants)
    assert_equal([:MIXIN, :USER], User.constants.sort)
  end

  def test_included_modules
    assert_equal([], Mixin.included_modules)
    assert_equal([Mixin], User.included_modules)

    mixins = Object.included_modules - [Kernel]
    mixins << JSON::Ext::Generator::GeneratorMethods::String if defined?(JSON::Ext::Generator::GeneratorMethods::String)
    assert_equal([Kernel], Object.included_modules - mixins)
    assert_equal([Comparable, Kernel], String.included_modules - mixins)
  end

  def test_instance_methods
    assert_equal([:user], User.instance_methods(false))
    assert_equal([:user, :mixin].sort, User.instance_methods(true).sort)
    assert_equal([:mixin], Mixin.instance_methods)
    assert_equal([:mixin], Mixin.instance_methods(true))
    assert_equal([:cClass], (class << CClass; self; end).instance_methods(false))
    assert_equal([], (class << BClass; self; end).instance_methods(false))
    assert_equal([:cm2], (class << AClass; self; end).instance_methods(false))
    # Ruby 1.8 feature change:
    # #instance_methods includes protected methods.
    #assert_equal([:aClass], AClass.instance_methods(false))
    assert_equal([:aClass, :aClass2], AClass.instance_methods(false).sort)
    assert_equal([:aClass, :aClass2],
        (AClass.instance_methods(true) - Object.instance_methods(true)).sort)
  end

  def test_method_defined?
    assert_method_not_defined?(User, :wombat)
    assert_method_defined?(User, :user)
    assert_method_defined?(User, :mixin)
    assert_method_not_defined?(User, :wombat)
    assert_method_defined?(User, :user)
    assert_method_defined?(User, :mixin)
  end

  def module_exec_aux
    Proc.new do
      def dynamically_added_method_3; end
    end
  end
  def module_exec_aux_2(&block)
    User.module_exec(&block)
  end

  def test_module_exec
    User.module_exec do
      def dynamically_added_method_1; end
    end
    assert_method_defined?(User, :dynamically_added_method_1)

    block = Proc.new do
      def dynamically_added_method_2; end
    end
    User.module_exec(&block)
    assert_method_defined?(User, :dynamically_added_method_2)

    User.module_exec(&module_exec_aux)
    assert_method_defined?(User, :dynamically_added_method_3)

    module_exec_aux_2 do
      def dynamically_added_method_4; end
    end
    assert_method_defined?(User, :dynamically_added_method_4)
  end

  def test_module_eval
    User.module_eval("MODULE_EVAL = 1")
    assert_equal(1, User::MODULE_EVAL)
    assert(User.constants.include?(:MODULE_EVAL))
    User.instance_eval("remove_const(:MODULE_EVAL)")
    assert(!User.constants.include?(:MODULE_EVAL))
  end

  def test_name
    assert_equal("Fixnum", Fixnum.name)
    assert_equal("TestModule::Mixin",  Mixin.name)
    assert_equal("TestModule::User",   User.name)
  end

  def test_private_class_method
    assert_raise(ExpectedException) { AClass.cm1 }
    assert_raise(ExpectedException) { AClass.cm3 }
    assert_equal("cm1cm2cm3", AClass.cm2)
  end

  def test_private_instance_methods
    assert_equal([:aClass1], AClass.private_instance_methods(false))
    assert_equal([:bClass2], BClass.private_instance_methods(false))
    assert_equal([:aClass1, :bClass2],
        (BClass.private_instance_methods(true) -
         Object.private_instance_methods(true)).sort)
  end

  def test_protected_instance_methods
    assert_equal([:aClass2], AClass.protected_instance_methods)
    assert_equal([:bClass3], BClass.protected_instance_methods(false))
    assert_equal([:bClass3, :aClass2].sort,
                 (BClass.protected_instance_methods(true) -
                  Object.protected_instance_methods(true)).sort)
  end

  def test_public_class_method
    assert_equal("cm1",       MyClass.cm1)
    assert_equal("cm1cm2cm3", MyClass.cm2)
    assert_raise(ExpectedException) { eval "MyClass.cm3" }
  end

  def test_public_instance_methods
    assert_equal([:aClass],  AClass.public_instance_methods(false))
    assert_equal([:bClass1], BClass.public_instance_methods(false))
  end

  def test_s_constants
    c1 = Module.constants
    Object.module_eval "WALTER = 99"
    c2 = Module.constants
    assert_equal([:WALTER], c2 - c1)

    assert_equal([], Module.constants(true))
    assert_equal([], Module.constants(false))

    src = <<-INPUT
      ary = Module.constants
      module M
        WALTER = 99
      end
      class Module
        include M
      end
      p Module.constants - ary, Module.constants(true), Module.constants(false)
    INPUT
    assert_in_out_err([], src, %w([:M] [:WALTER] []), [])

    klass = Class.new do
      const_set(:X, 123)
    end
    assert_equal(false, klass.class_eval { Module.constants }.include?(:X))
  end

  module M1
    $m1 = Module.nesting
    module M2
      $m2 = Module.nesting
    end
  end

  def test_s_nesting
    assert_equal([],                               $m0)
    assert_equal([TestModule::M1, TestModule],     $m1)
    assert_equal([TestModule::M1::M2,
                  TestModule::M1, TestModule],     $m2)
  end

  def test_s_new
    m = Module.new
    assert_instance_of(Module, m)
  end

  def test_freeze
    m = Module.new
    m.freeze
    assert_raise(RuntimeError) do
      m.module_eval do
        def foo; end
      end
    end
  end

  def test_attr_obsoleted_flag
    c = Class.new
    c.class_eval do
      def initialize
        @foo = :foo
        @bar = :bar
      end
      attr :foo, true
      attr :bar, false
    end
    o = c.new
    assert_equal(true, o.respond_to?(:foo))
    assert_equal(true, o.respond_to?(:foo=))
    assert_equal(true, o.respond_to?(:bar))
    assert_equal(false, o.respond_to?(:bar=))
  end

  def test_const_get_evaled
    c1 = Class.new
    c2 = Class.new(c1)

    eval("c1::Foo = :foo")
    assert_equal(:foo, c1::Foo)
    assert_equal(:foo, c2::Foo)
    assert_equal(:foo, c2.const_get(:Foo))
    assert_raise(NameError) { c2.const_get(:Foo, false) }

    eval("c1::Foo = :foo")
    assert_raise(NameError) { c1::Bar }
    assert_raise(NameError) { c2::Bar }
    assert_raise(NameError) { c2.const_get(:Bar) }
    assert_raise(NameError) { c2.const_get(:Bar, false) }
    assert_raise(NameError) { c2.const_get("Bar", false) }
    assert_raise(NameError) { c2.const_get("BaR11", false) }
    assert_raise(NameError) { Object.const_get("BaR11", false) }

    c1.instance_eval do
      def const_missing(x)
        x
      end
    end

    assert_equal(:Bar, c1::Bar)
    assert_equal(:Bar, c2::Bar)
    assert_equal(:Bar, c2.const_get(:Bar))
    assert_equal(:Bar, c2.const_get(:Bar, false))
    assert_equal(:Bar, c2.const_get("Bar"))
    assert_equal(:Bar, c2.const_get("Bar", false))

    v = c2.const_get("Bar11", false)
    assert_equal("Bar11".to_sym, v)

    assert_raise(NameError) { c1.const_get(:foo) }
  end

  def test_const_set_invalid_name
    c1 = Class.new
    assert_raise(NameError) { c1.const_set(:foo, :foo) }
  end

  def test_const_get_invalid_name
    c1 = Class.new
    assert_raise(NameError) { c1.const_defined?(:foo) }
    bug5084 = '[ruby-dev:44200]'
    assert_raise(TypeError, bug5084) { c1.const_defined?(1) }
  end

  def test_const_get_no_inherited
    bug3422 = '[ruby-core:30719]'
    assert_in_out_err([], <<-INPUT, %w[1 NameError A], [], bug3422)
    BasicObject::A = 1
    puts [true, false].map {|inh|
      begin
        Object.const_get(:A, inh)
      rescue NameError => e
        [e.class, e.name]
      end
    }
    INPUT
  end

  def test_const_get_inherited
    bug3423 = '[ruby-core:30720]'
    assert_in_out_err([], <<-INPUT, %w[NameError A NameError A], [], bug3423)
    module Foo; A = 1; end
    class Object; include Foo; end
    class Bar; include Foo; end

    puts [Object, Bar].map {|klass|
      begin
        klass.const_get(:A, false)
      rescue NameError => e
        [e.class, e.name]
      end
    }
    INPUT
  end

  def test_const_in_module
    bug3423 = '[ruby-core:37698]'
    assert_in_out_err([], <<-INPUT, %w[ok], [], bug3423)
    module LangModuleSpecInObject
      module LangModuleTop
      end
    end
    include LangModuleSpecInObject
    module LangModuleTop
    end
    puts "ok" if LangModuleSpecInObject::LangModuleTop == LangModuleTop
    INPUT

    bug5264 = '[ruby-core:39227]'
    assert_in_out_err([], <<-'INPUT', [], [], bug5264)
    class A
      class X; end
    end
    class B < A
      module X; end
    end
    INPUT
  end

  def test_class_variable_get
    c = Class.new
    c.class_eval('@@foo = :foo')
    assert_equal(:foo, c.class_variable_get(:@@foo))
    assert_raise(NameError) { c.class_variable_get(:@@bar) } # c.f. instance_variable_get
    assert_raise(NameError) { c.class_variable_get(:foo) }
  end

  def test_class_variable_set
    c = Class.new
    c.class_variable_set(:@@foo, :foo)
    assert_equal(:foo, c.class_eval('@@foo'))
    assert_raise(NameError) { c.class_variable_set(:foo, 1) }
  end

  def test_class_variable_defined
    c = Class.new
    c.class_eval('@@foo = :foo')
    assert_equal(true, c.class_variable_defined?(:@@foo))
    assert_equal(false, c.class_variable_defined?(:@@bar))
    assert_raise(NameError) { c.class_variable_defined?(:foo) }
  end

  def test_remove_class_variable
    c = Class.new
    c.class_eval('@@foo = :foo')
    c.class_eval { remove_class_variable(:@@foo) }
    assert_equal(false, c.class_variable_defined?(:@@foo))
  end

  def test_export_method
    m = Module.new
    assert_raise(NameError) do
      m.instance_eval { public(:foo) }
    end
  end

  def test_attr
    assert_in_out_err([], <<-INPUT, %w(:ok nil), /warning: private attribute\?$/)
      $VERBOSE = true
      c = Class.new
      c.instance_eval do
        private
        attr_reader :foo
      end
      o = c.new
      o.foo rescue p(:ok)
      p(o.instance_eval { foo })
    INPUT

    c = Class.new
    assert_raise(NameError) do
      c.instance_eval { attr_reader :"." }
    end
  end

  def test_undef
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        Class.instance_eval { undef_method(:foo) }
      end.join
    end

    c = Class.new
    assert_raise(NameError) do
      c.instance_eval { undef_method(:foo) }
    end

    m = Module.new
    assert_raise(NameError) do
      m.instance_eval { undef_method(:foo) }
    end

    o = Object.new
    assert_raise(NameError) do
      class << o; self; end.instance_eval { undef_method(:foo) }
    end

    %w(object_id __send__ initialize).each do |n|
      assert_in_out_err([], <<-INPUT, [], /warning: undefining `#{n}' may cause serious problems$/)
        $VERBOSE = false
        Class.new.instance_eval { undef_method(:#{n}) }
      INPUT
    end
  end

  def test_alias
    m = Module.new
    assert_raise(NameError) do
      m.class_eval { alias foo bar }
    end

    assert_in_out_err([], <<-INPUT, %w(2), /discarding old foo$/)
      $VERBOSE = true
      c = Class.new
      c.class_eval do
        def foo; 1; end
        def bar; 2; end
      end
      c.class_eval { alias foo bar }
      p c.new.foo
    INPUT
  end

  def test_mod_constants
    m = Module.new
    m.const_set(:Foo, :foo)
    assert_equal([:Foo], m.constants(true))
    assert_equal([:Foo], m.constants(false))
    m.instance_eval { remove_const(:Foo) }
  end

  def test_frozen_class
    m = Module.new
    m.freeze
    assert_raise(RuntimeError) do
      m.instance_eval { undef_method(:foo) }
    end

    c = Class.new
    c.freeze
    assert_raise(RuntimeError) do
      c.instance_eval { undef_method(:foo) }
    end

    o = Object.new
    c = class << o; self; end
    c.freeze
    assert_raise(RuntimeError) do
      c.instance_eval { undef_method(:foo) }
    end
  end

  def test_method_defined
    c = Class.new
    c.class_eval do
      def foo; end
      def bar; end
      def baz; end
      public :foo
      protected :bar
      private :baz
    end

    assert_equal(true, c.public_method_defined?(:foo))
    assert_equal(false, c.public_method_defined?(:bar))
    assert_equal(false, c.public_method_defined?(:baz))

    assert_equal(false, c.protected_method_defined?(:foo))
    assert_equal(true, c.protected_method_defined?(:bar))
    assert_equal(false, c.protected_method_defined?(:baz))

    assert_equal(false, c.private_method_defined?(:foo))
    assert_equal(false, c.private_method_defined?(:bar))
    assert_equal(true, c.private_method_defined?(:baz))
  end

  def test_change_visibility_under_safe4
    c = Class.new
    c.class_eval do
      def foo; end
    end
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        c.class_eval { private :foo }
      end.join
    end
  end

  def test_top_public_private
    assert_in_out_err([], <<-INPUT, %w([:foo] [:bar]), [])
      private
      def foo; :foo; end
      public
      def bar; :bar; end
      p self.private_methods.grep(/^foo$|^bar$/)
      p self.methods.grep(/^foo$|^bar$/)
    INPUT
  end

  def test_append_features
    t = nil
    m = Module.new
    m.module_eval do
      def foo; :foo; end
    end
    class << m; self; end.class_eval do
      define_method(:append_features) do |mod|
        t = mod
        super(mod)
      end
    end

    m2 = Module.new
    m2.module_eval { include(m) }
    assert_equal(m2, t)

    o = Object.new
    o.extend(m2)
    assert_equal(true, o.respond_to?(:foo))
  end

  def test_append_features_raise
    m = Module.new
    m.module_eval do
      def foo; :foo; end
    end
    class << m; self; end.class_eval do
      define_method(:append_features) {|mod| raise }
    end

    m2 = Module.new
    assert_raise(RuntimeError) do
      m2.module_eval { include(m) }
    end

    o = Object.new
    o.extend(m2)
    assert_equal(false, o.respond_to?(:foo))
  end

  def test_append_features_type_error
    assert_raise(TypeError) do
      Module.new.instance_eval { append_features(1) }
    end
  end

  def test_included
    m = Module.new
    m.module_eval do
      def foo; :foo; end
    end
    class << m; self; end.class_eval do
      define_method(:included) {|mod| raise }
    end

    m2 = Module.new
    assert_raise(RuntimeError) do
      m2.module_eval { include(m) }
    end

    o = Object.new
    o.extend(m2)
    assert_equal(true, o.respond_to?(:foo))
  end

  def test_cyclic_include
    m1 = Module.new
    m2 = Module.new
    m1.instance_eval { include(m2) }
    assert_raise(ArgumentError) do
      m2.instance_eval { include(m1) }
    end
  end

  def test_include_p
    m = Module.new
    c1 = Class.new
    c1.instance_eval { include(m) }
    c2 = Class.new(c1)
    assert_equal(true, c1.include?(m))
    assert_equal(true, c2.include?(m))
    assert_equal(false, m.include?(m))
  end

  def test_include_under_safe4
    m = Module.new
    c1 = Class.new
    assert_raise(SecurityError) do
      lambda {
        $SAFE = 4
        c1.instance_eval { include(m) }
      }.call
    end
    assert_nothing_raised do
      lambda {
        $SAFE = 4
        c2 = Class.new
        c2.instance_eval { include(m) }
      }.call
    end
  end

  def test_send
    a = AClass.new
    assert_equal(:aClass, a.__send__(:aClass))
    assert_equal(:aClass1, a.__send__(:aClass1))
    assert_equal(:aClass2, a.__send__(:aClass2))
    b = BClass.new
    assert_equal(:aClass, b.__send__(:aClass))
    assert_equal(:aClass1, b.__send__(:aClass1))
    assert_equal(:aClass2, b.__send__(:aClass2))
    assert_equal(:bClass1, b.__send__(:bClass1))
    assert_equal(:bClass2, b.__send__(:bClass2))
    assert_equal(:bClass3, b.__send__(:bClass3))
  end


  def test_nonascii_name
    c = eval("class ::C\u{df}; self; end")
    assert_equal("C\u{df}", c.name, '[ruby-core:24600]')
    c = eval("class C\u{df}; self; end")
    assert_equal("TestModule::C\u{df}", c.name, '[ruby-core:24600]')
  end

  def test_method_added
    memo = []
    mod = Module.new do
      mod = self
      (class << self ; self ; end).class_eval do
        define_method :method_added do |sym|
          memo << sym
          memo << mod.instance_methods(false)
          memo << (mod.instance_method(sym) rescue nil)
        end
      end
      def f
      end
      alias g f
      attr_reader :a
      attr_writer :a
    end
    assert_equal :f, memo.shift
    assert_equal [:f], memo.shift, '[ruby-core:25536]'
    assert_equal mod.instance_method(:f), memo.shift
    assert_equal :g, memo.shift
    assert_equal [:f, :g], memo.shift
    assert_equal mod.instance_method(:f), memo.shift
    assert_equal :a, memo.shift
    assert_equal [:f, :g, :a], memo.shift
    assert_equal mod.instance_method(:a), memo.shift
    assert_equal :a=, memo.shift
    assert_equal [:f, :g, :a, :a=], memo.shift
    assert_equal mod.instance_method(:a=), memo.shift
  end

  def test_method_undefined
    added = []
    undefed = []
    removed = []
    mod = Module.new do
      mod = self
      def f
      end
      (class << self ; self ; end).class_eval do
        define_method :method_added do |sym|
          added << sym
        end
        define_method :method_undefined do |sym|
          undefed << sym
        end
        define_method :method_removed do |sym|
          removed << sym
        end
      end
    end
    assert_method_defined?(mod, :f)
    mod.module_eval do
      undef :f
    end
    assert_equal [], added
    assert_equal [:f], undefed
    assert_equal [], removed
  end

  def test_method_removed
    added = []
    undefed = []
    removed = []
    mod = Module.new do
      mod = self
      def f
      end
      (class << self ; self ; end).class_eval do
        define_method :method_added do |sym|
          added << sym
        end
        define_method :method_undefined do |sym|
          undefed << sym
        end
        define_method :method_removed do |sym|
          removed << sym
        end
      end
    end
    assert_method_defined?(mod, :f)
    mod.module_eval do
      remove_method :f
    end
    assert_equal [], added
    assert_equal [], undefed
    assert_equal [:f], removed
  end

  def test_method_redefinition
    feature2155 = '[ruby-dev:39400]'

    line = __LINE__+4
    stderr = EnvUtil.verbose_warning do
      Module.new do
        def foo; end
        def foo; end
      end
    end
    assert_match(/:#{line}: warning: method redefined; discarding old foo/, stderr)
    assert_match(/:#{line-1}: warning: previous definition of foo/, stderr, feature2155)

    stderr = EnvUtil.verbose_warning do
      Module.new do
        def foo; end
        alias bar foo
        def foo; end
      end
    end
    assert_equal("", stderr)

    stderr = EnvUtil.verbose_warning do
      Module.new do
        def foo; end
        alias bar foo
        alias bar foo
      end
    end
    assert_equal("", stderr)

    line = __LINE__+4
    stderr = EnvUtil.verbose_warning do
      Module.new do
        define_method(:foo) do end
        def foo; end
      end
    end
    assert_match(/:#{line}: warning: method redefined; discarding old foo/, stderr)
    assert_match(/:#{line-1}: warning: previous definition of foo/, stderr, feature2155)

    stderr = EnvUtil.verbose_warning do
      Module.new do
        define_method(:foo) do end
        alias bar foo
        alias bar foo
      end
    end
    assert_equal("", stderr)

    stderr = EnvUtil.verbose_warning do
      Module.new do
        module_function
        def foo; end
        module_function :foo
      end
    end
    assert_equal("", stderr, '[ruby-dev:39397]')

    stderr = EnvUtil.verbose_warning do
      Module.new do
        def foo; end
        undef foo
      end
    end
    assert_equal("", stderr)
  end

  def test_protected_singleton_method
    klass = Class.new
    x = klass.new
    class << x
      protected

      def foo
      end
    end
    assert_raise(NoMethodError) do
      x.foo
    end
    klass.send(:define_method, :bar) do
      x.foo
    end
    assert_nothing_raised do
      x.bar
    end
    y = klass.new
    assert_raise(NoMethodError) do
      y.bar
    end
  end

  def test_uninitialized_toplevel_constant
    bug3123 = '[ruby-dev:40951]'
    e = assert_raise(NameError) {eval("Bug3123", TOPLEVEL_BINDING)}
    assert_not_match(/Object::/, e.message, bug3123)
  end

  def test_attr_inherited_visibility
    bug3406 = '[ruby-core:30638]'
    c = Class.new do
      class << self
        private
        def attr_accessor(*); super; end
      end
      attr_accessor :x
    end.new
    assert_nothing_raised(bug3406) {c.x = 1}
    assert_equal(1, c.x, bug3406)
  end

  def test_private_constant
    c = Class.new
    c.const_set(:FOO, "foo")
    assert_equal("foo", c::FOO)
    c.private_constant(:FOO)
    assert_raise(NameError) { c::FOO }
    assert_equal("foo", c.class_eval("FOO"))
    assert_equal("foo", c.const_get("FOO"))
    $VERBOSE, verbose = nil, $VERBOSE
    c.const_set(:FOO, "foo")
    $VERBOSE = verbose
    assert_raise(NameError) { c::FOO }
  end

  def test_private_constant2
    c = Class.new
    c.const_set(:FOO, "foo")
    c.const_set(:BAR, "bar")
    assert_equal("foo", c::FOO)
    assert_equal("bar", c::BAR)
    c.private_constant(:FOO, :BAR)
    assert_raise(NameError) { c::FOO }
    assert_raise(NameError) { c::BAR }
    assert_equal("foo", c.class_eval("FOO"))
    assert_equal("bar", c.class_eval("BAR"))
  end

  class PrivateClass
  end
  private_constant :PrivateClass

  def test_define_module_under_private_constant
    assert_raise(NameError) do
      eval %q{class TestModule::PrivateClass; end}
    end
    assert_raise(NameError) do
      eval %q{module TestModule::PrivateClass::TestModule; end}
    end
    eval %q{class PrivateClass; end}
    eval %q{module PrivateClass::TestModule; end}
    assert_instance_of(Module, PrivateClass::TestModule)
    PrivateClass.class_eval { remove_const(:TestModule) }
  end

  def test_public_constant
    c = Class.new
    c.const_set(:FOO, "foo")
    assert_equal("foo", c::FOO)
    c.private_constant(:FOO)
    assert_raise(NameError) { c::FOO }
    assert_equal("foo", c.class_eval("FOO"))
    c.public_constant(:FOO)
    assert_equal("foo", c::FOO)
  end

  def test_constants_with_private_constant
    assert(!(::TestModule).constants.include?(:PrivateClass))
  end

  def test_toplevel_private_constant
    src = <<-INPUT
      class Object
        private_constant :Object
      end
      p Object
      begin
        p ::Object
      rescue
        p :ok
      end
    INPUT
    assert_in_out_err([], src, %w(Object :ok), [])
  end

  def test_private_constants_clear_inlinecache
    bug5702 = '[ruby-dev:44929]'
    src = <<-INPUT
    class A
      C = :Const
      def self.get_C
        A::C
      end
      # fill cache
      A.get_C
      private_constant :C, :D rescue nil
      begin
        A.get_C
      rescue NameError
        puts "A.get_C"
      end
    end
    INPUT
    assert_in_out_err([], src, %w(A.get_C), [], bug5702)
  end

  def test_constant_lookup_in_method_defined_by_class_eval
    src = <<-INPUT
      class A
        B = 42
      end

      A.class_eval do
        def self.f
          B
        end

        def f
          B
        end
      end

      begin
        A.f
      rescue NameError
        puts "A.f"
      end
      begin
        A.new.f
      rescue NameError
        puts "A.new.f"
      end
    INPUT
    assert_in_out_err([], src, %w(A.f A.new.f), [])
  end

  def test_constant_lookup_in_toplevel_class_eval
    src = <<-INPUT
      module X
        A = 123
      end
      begin
        X.class_eval { A }
      rescue NameError => e
        puts e
      end
    INPUT
    assert_in_out_err([], src, ["uninitialized constant A"], [])
  end

  def test_constant_lookup_in_module_in_class_eval
    src = <<-INPUT
      class A
        B = 42
      end

      A.class_eval do
        module C
          begin
            B
          rescue NameError
            puts "NameError"
          end
        end
      end
    INPUT
    assert_in_out_err([], src, ["NameError"], [])
  end

  def test_mix_method
    american = Module.new do
      attr_accessor :address
    end
    japanese = Module.new do
      attr_accessor :address
    end

    japanese_american = Class.new
    assert_nothing_raised(ArgumentError) {
      japanese_american.class_eval {mix american}
    }
    assert_raise(ArgumentError) {
      japanese_american.class_eval {mix japanese}
    }

    japanese_american = Class.new
    assert_nothing_raised(ArgumentError) {
      japanese_american.class_eval {
        mix american, :address => :us_address, :address= => :us_address=
      }
    }
    assert_nothing_raised(ArgumentError) {
      japanese_american.class_eval {
        mix japanese, :address => :jp_address, :address= => :jp_address=
      }
    }

    japanese_american = Class.new
    assert_nothing_raised(ArgumentError) {
      japanese_american.class_eval {
        mix japanese, :address => nil, :address= => nil
      }
    }
    assert_raise(NoMethodError) {
      japanese_american.new.address
    }
    assert_nothing_raised(ArgumentError) {
      japanese_american.class_eval {
        mix american
      }
    }
  end

  def test_mix_const
    foo = Module.new do
      const_set(:D, 55)
    end
    bar = Class.new do
      const_set(:D, 42)
    end
    assert_nothing_raised(ArgumentError) {
      bar.class_eval {
        mix foo
      }
    }
    assert_equal(42, bar::D)
  end
end
