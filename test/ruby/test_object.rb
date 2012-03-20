require 'test/unit'
require_relative 'envutil'

class TestObject < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_dup
    assert_raise(TypeError) { 1.dup }
    assert_raise(TypeError) { true.dup }
    assert_raise(TypeError) { nil.dup }

    assert_raise(TypeError) do
      Object.new.instance_eval { initialize_copy(1) }
    end
  end

  def test_instance_of
    assert_raise(TypeError) { 1.instance_of?(1) }
  end

  def test_kind_of
    assert_raise(TypeError) { 1.kind_of?(1) }
  end

  def test_taint_frozen_obj
    o = Object.new
    o.freeze
    assert_raise(RuntimeError) { o.taint }

    o = Object.new
    o.taint
    o.freeze
    assert_raise(RuntimeError) { o.untaint }
  end

  def test_freeze_under_safe_4
    o = Object.new
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        o.freeze
      end.join
    end
  end

  def test_freeze_immediate
    assert_equal(false, 1.frozen?)
    1.freeze
    assert_equal(true, 1.frozen?)
    assert_equal(false, 2.frozen?)
  end

  def test_nil_to_f
    assert_equal(0.0, nil.to_f)
  end

  def test_not
    assert_equal(false, Object.new.send(:!))
    assert_equal(true, nil.send(:!))
  end

  def test_true_and
    assert_equal(true, true & true)
    assert_equal(true, true & 1)
    assert_equal(false, true & false)
    assert_equal(false, true & nil)
  end

  def test_true_or
    assert_equal(true, true | true)
    assert_equal(true, true | 1)
    assert_equal(true, true | false)
    assert_equal(true, true | nil)
  end

  def test_true_xor
    assert_equal(false, true ^ true)
    assert_equal(false, true ^ 1)
    assert_equal(true, true ^ false)
    assert_equal(true, true ^ nil)
  end

  def test_false_and
    assert_equal(false, false & true)
    assert_equal(false, false & 1)
    assert_equal(false, false & false)
    assert_equal(false, false & nil)
  end

  def test_false_or
    assert_equal(true, false | true)
    assert_equal(true, false | 1)
    assert_equal(false, false | false)
    assert_equal(false, false | nil)
  end

  def test_false_xor
    assert_equal(true, false ^ true)
    assert_equal(true, false ^ 1)
    assert_equal(false, false ^ false)
    assert_equal(false, false ^ nil)
  end

  def test_methods
    o = Object.new
    a1 = o.methods
    a2 = o.methods(false)

    def o.foo; end

    assert_equal([:foo], o.methods(true) - a1)
    assert_equal([:foo], o.methods(false) - a2)
  end

  def test_methods2
    c0 = Class.new
    c1 = Class.new(c0)
    c1.module_eval do
      public   ; def foo; end
      protected; def bar; end
      private  ; def baz; end
    end
    c2 = Class.new(c1)
    c2.module_eval do
      public   ; def foo2; end
      protected; def bar2; end
      private  ; def baz2; end
    end

    o0 = c0.new
    o2 = c2.new

    assert_equal([:baz, :baz2], (o2.private_methods - o0.private_methods).sort)
    assert_equal([:baz2], (o2.private_methods(false) - o0.private_methods(false)).sort)

    assert_equal([:bar, :bar2], (o2.protected_methods - o0.protected_methods).sort)
    assert_equal([:bar2], (o2.protected_methods(false) - o0.protected_methods(false)).sort)

    assert_equal([:foo, :foo2], (o2.public_methods - o0.public_methods).sort)
    assert_equal([:foo2], (o2.public_methods(false) - o0.public_methods(false)).sort)
  end

  def test_instance_variable_get
    o = Object.new
    o.instance_eval { @foo = :foo }
    assert_equal(:foo, o.instance_variable_get(:@foo))
    assert_equal(nil, o.instance_variable_get(:@bar))
    assert_raise(NameError) { o.instance_variable_get(:foo) }
  end

  def test_instance_variable_set
    o = Object.new
    o.instance_variable_set(:@foo, :foo)
    assert_equal(:foo, o.instance_eval { @foo })
    assert_raise(NameError) { o.instance_variable_set(:foo, 1) }
  end

  def test_instance_variable_defined
    o = Object.new
    o.instance_eval { @foo = :foo }
    assert_equal(true, o.instance_variable_defined?(:@foo))
    assert_equal(false, o.instance_variable_defined?(:@bar))
    assert_raise(NameError) { o.instance_variable_defined?(:foo) }
  end

  def test_remove_instance_variable
    o = Object.new
    o.instance_eval { @foo = :foo }
    o.instance_eval { remove_instance_variable(:@foo) }
    assert_equal(false, o.instance_variable_defined?(:@foo))
  end

  def test_convert_type
    o = Object.new
    def o.to_s; 1; end
    assert_raise(TypeError) { String(o) }
    def o.to_s; "o"; end
    assert_equal("o", String(o))
    def o.respond_to?(*) false; end
    assert_raise(TypeError) { String(o) }
  end

  def test_check_convert_type
    o = Object.new
    def o.to_a; 1; end
    assert_raise(TypeError) { Array(o) }
    def o.to_a; [1]; end
    assert_equal([1], Array(o))
    def o.respond_to?(*) false; end
    assert_equal([o], Array(o))
  end

  def test_convert_hash
    assert_equal({}, Hash(nil))
    assert_equal({}, Hash([]))
    assert_equal({key: :value}, Hash(key: :value))
    assert_raise(TypeError) { Hash([1,2]) }
    assert_raise(TypeError) { Hash(Object.new) }
    o = Object.new
    def o.to_hash; {a: 1, b: 2}; end
    assert_equal({a: 1, b: 2}, Hash(o))
    def o.to_hash; 9; end
    assert_raise(TypeError) { Hash(o) }
  end

  def test_to_integer
    o = Object.new
    def o.to_i; nil; end
    assert_raise(TypeError) { Integer(o) }
    def o.to_i; 42; end
    assert_equal(42, Integer(o))
    def o.respond_to?(*) false; end
    assert_raise(TypeError) { Integer(o) }
  end

  class MyInteger
    def initialize(n); @num = n; end
    def to_int; @num; end
    def <=>(n); @num <=> n.to_int; end
    def <=(n); @num <= n.to_int; end
    def +(n); MyInteger.new(@num + n.to_int); end
  end

  def test_check_to_integer
    o1 = MyInteger.new(1)
    o9 = MyInteger.new(9)
    n = 0
    Range.new(o1, o9).step(2) {|x| n += x.to_int }
    assert_equal(1+3+5+7+9, n)
  end

  def test_add_method_under_safe4
    o = Object.new
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        def o.foo
        end
      end.join
    end
  end

  def test_redefine_method_under_verbose
    assert_in_out_err([], <<-INPUT, %w(2), /warning: method redefined; discarding old foo$/)
      $VERBOSE = true
      o = Object.new
      def o.foo; 1; end
      def o.foo; 2; end
      p o.foo
    INPUT
  end

  def test_redefine_method_which_may_case_serious_problem
    assert_in_out_err([], <<-INPUT, [], /warning: redefining `object_id' may cause serious problems$/)
      $VERBOSE = false
      def (Object.new).object_id; end
    INPUT

    assert_in_out_err([], <<-INPUT, [], /warning: redefining `__send__' may cause serious problems$/)
      $VERBOSE = false
      def (Object.new).__send__; end
    INPUT
  end

  def test_remove_method
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        Object.instance_eval { remove_method(:foo) }
      end.join
    end

    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        Class.instance_eval { remove_method(:foo) }
      end.join
    end

    c = Class.new
    c.freeze
    assert_raise(RuntimeError) do
      c.instance_eval { remove_method(:foo) }
    end

    c = Class.new do
      def meth1; "meth" end
    end
    d = Class.new(c) do
      alias meth2 meth1
    end
    o1 = c.new
    assert_respond_to(o1, :meth1)
    assert_equal("meth", o1.meth1)
    o2 = d.new
    assert_respond_to(o2, :meth1)
    assert_equal("meth", o2.meth1)
    assert_respond_to(o2, :meth2)
    assert_equal("meth", o2.meth2)
    d.class_eval do
      remove_method :meth2
    end
    bug2202 = '[ruby-core:26074]'
    assert_raise(NoMethodError, bug2202) {o2.meth2}

    %w(object_id __send__ initialize).each do |m|
      assert_in_out_err([], <<-INPUT, %w(:ok), /warning: removing `#{m}' may cause serious problems$/)
        $VERBOSE = false
        begin
          Class.new.instance_eval { remove_method(:#{m}) }
        rescue NameError
          p :ok
        end
      INPUT
    end
  end

  def test_method_missing
    assert_raise(ArgumentError) do
      1.instance_eval { method_missing }
    end

    c = Class.new
    c.class_eval do
      protected
      def foo; end
    end
    assert_raise(NoMethodError) do
      c.new.foo
    end

    assert_raise(NoMethodError) do
      1.instance_eval { method_missing(:method_missing) }
    end

    c.class_eval do
      undef_method(:method_missing)
    end

    assert_raise(ArgumentError) do
      c.new.method_missing
    end

    bug2494 = '[ruby-core:27219]'
    c = Class.new do
      def method_missing(meth, *args)
        super
      end
    end
    b = c.new
    foo rescue nil
    assert_nothing_raised(bug2494) {[b].flatten}
  end

  def test_respond_to_missing_string
    c = Class.new do
      def respond_to_missing?(id, priv)
        !(id !~ /\Agadzoks\d+\z/) ^ priv
      end
    end
    foo = c.new
    assert_equal(false, foo.respond_to?("gadzooks16"))
    assert_equal(true, foo.respond_to?("gadzooks17", true))
    assert_equal(true, foo.respond_to?("gadzoks16"))
    assert_equal(false, foo.respond_to?("gadzoks17", true))
  end

  def test_respond_to_missing
    c = Class.new do
      def respond_to_missing?(id, priv)
        if id == :foobar
          true
        else
          false
        end
      end
      def method_missing(id, *args)
        if id == :foobar
          return [:foo, *args]
        else
          super
        end
      end
    end

    foo = c.new
    assert_equal([:foo], foo.foobar);
    assert_equal([:foo, 1], foo.foobar(1));
    assert_equal([:foo, 1, 2, 3, 4, 5], foo.foobar(1, 2, 3, 4, 5));
    assert(foo.respond_to?(:foobar))
    assert_equal(false, foo.respond_to?(:foobarbaz))
    assert_raise(NoMethodError) do
      foo.foobarbaz
    end

    foobar = foo.method(:foobar)
    assert_equal(-1, foobar.arity);
    assert_equal([:foo], foobar.call);
    assert_equal([:foo, 1], foobar.call(1));
    assert_equal([:foo, 1, 2, 3, 4, 5], foobar.call(1, 2, 3, 4, 5));
    assert_equal(foobar, foo.method(:foobar))
    assert_not_equal(foobar, c.new.method(:foobar))

    c = Class.new(c)
    assert_equal(false, c.method_defined?(:foobar))
    assert_raise(NameError, '[ruby-core:25748]') do
      c.instance_method(:foobar)
    end

    m = Module.new
    assert_equal(false, m.method_defined?(:foobar))
    assert_raise(NameError, '[ruby-core:25748]') do
      m.instance_method(:foobar)
    end
  end

  def test_implicit_respond_to
    bug5158 = '[ruby-core:38799]'

    p = Object.new

    called = []
    p.singleton_class.class_eval do
      define_method(:to_ary) do
        called << [:to_ary, bug5158]
      end
    end
    [[p]].flatten
    assert_equal([[:to_ary, bug5158]], called, bug5158)

    called = []
    p.singleton_class.class_eval do
      define_method(:respond_to?) do |*a|
        called << [:respond_to?, *a]
        false
      end
    end
    [[p]].flatten
    assert_equal([[:respond_to?, :to_ary, true]], called, bug5158)
  end

  def test_implicit_respond_to_arity_1
    p = Object.new

    called = []
    p.singleton_class.class_eval do
      define_method(:respond_to?) do |a|
        called << [:respond_to?, a]
        false
      end
    end
    [[p]].flatten
    assert_equal([[:respond_to?, :to_ary]], called, '[bug:6000]')
  end

  def test_implicit_respond_to_arity_3
    p = Object.new

    called = []
    p.singleton_class.class_eval do
      define_method(:respond_to?) do |a, b, c|
        called << [:respond_to?, a, b, c]
        false
      end
    end

    e = assert_raise(ArgumentError, '[bug:6000]') do
      [[p]].flatten
    end

    assert_equal('respond_to? must accept 1 or 2 arguments (requires 3)', e.message)
  end

  def test_method_missing_passed_block
    bug5731 = '[ruby-dev:44961]'

    c = Class.new do
      def method_missing(meth, *args) yield(meth, *args) end
    end
    a = c.new
    result = nil
    assert_nothing_raised(LocalJumpError, bug5731) do
      a.foo {|x| result = x}
    end
    assert_equal(:foo, result, bug5731)
    result = nil
    e = a.enum_for(:foo)
    assert_nothing_raised(LocalJumpError, bug5731) do
      e.each {|x| result = x}
    end
    assert_equal(:foo, result, bug5731)

    c = Class.new do
      def respond_to_missing?(id, priv)
        true
      end
      def method_missing(id, *args, &block)
        return block.call(:foo, *args)
      end
    end
    foo = c.new

    result = nil
    assert_nothing_raised(LocalJumpError, bug5731) do
      foo.foobar {|x| result = x}
    end
    assert_equal(:foo, result, bug5731)
    result = nil
    assert_nothing_raised(LocalJumpError, bug5731) do
      foo.enum_for(:foobar).each {|x| result = x}
    end
    assert_equal(:foo, result, bug5731)

    result = nil
    foobar = foo.method(:foobar)
    foobar.call {|x| result = x}
    assert_equal(:foo, result, bug5731)

    result = nil
    foobar = foo.method(:foobar)
    foobar.enum_for(:call).each {|x| result = x}
    assert_equal(:foo, result, bug5731)
  end

  def test_send_with_no_arguments
    assert_raise(ArgumentError) { 1.send }
  end

  def test_send_with_block
    x = :ng
    1.send(:times) { x = :ok }
    assert_equal(:ok, x)

    x = :ok
    o = Object.new
    def o.inspect
      yield if block_given?
      super
    end
    begin
      nil.public_send(o) { x = :ng }
    rescue
    end
    assert_equal(:ok, x)
  end

  def test_no_superclass_method
    bug2312 = '[ruby-dev:39581]'

    o = Object.new
    e = assert_raise(NoMethodError) {
      o.method(:__send__).call(:never_defined_test_no_superclass_method)
    }
    m1 = e.message
    assert_no_match(/no superclass method/, m1, bug2312)
    e = assert_raise(NoMethodError) {
      o.method(:__send__).call(:never_defined_test_no_superclass_method)
    }
    assert_equal(m1, e.message, bug2312)
    e = assert_raise(NoMethodError) {
      o.never_defined_test_no_superclass_method
    }
    assert_equal(m1, e.message, bug2312)
  end

  def test_superclass_method
    bug2312 = '[ruby-dev:39581]'
    assert_in_out_err(["-e", "module Enumerable;undef min;end; (1..2).min{}"],
                      "", [], /no superclass method/, bug2312)
  end

  def test_specific_eval_with_wrong_arguments
    assert_raise(ArgumentError) do
      1.instance_eval("foo") { foo }
    end

    assert_raise(ArgumentError) do
      1.instance_eval
    end

    assert_raise(ArgumentError) do
      1.instance_eval("", 1, 1, 1)
    end
  end

  class InstanceExec
    INSTANCE_EXEC = 123
  end

  def test_instance_exec
    x = 1.instance_exec(42) {|a| self + a }
    assert_equal(43, x)

    x = "foo".instance_exec("bar") {|a| self + a }
    assert_equal("foobar", x)

    assert_raise(NameError) do
      InstanceExec.new.instance_exec { INSTANCE_EXEC }
    end
  end

  def test_extend
    assert_raise(ArgumentError) do
      1.extend
    end
  end

  def test_untrusted
    obj = lambda {
      $SAFE = 4
      x = Object.new
      x.instance_eval { @foo = 1 }
      x
    }.call
    assert_equal(true, obj.untrusted?)
    assert_equal(true, obj.tainted?)

    x = Object.new
    assert_equal(false, x.untrusted?)
    assert_raise(SecurityError) do
      lambda {
        $SAFE = 4
        x.instance_eval { @foo = 1 }
      }.call
    end

    x = Object.new
    x.taint
    assert_raise(SecurityError) do
      lambda {
        $SAFE = 4
        x.instance_eval { @foo = 1 }
      }.call
    end

    x.untrust
    assert_equal(true, x.untrusted?)
    assert_nothing_raised do
      lambda {
        $SAFE = 4
        x.instance_eval { @foo = 1 }
      }.call
    end

    x.trust
    assert_equal(false, x.untrusted?)
    assert_raise(SecurityError) do
      lambda {
        $SAFE = 4
        x.instance_eval { @foo = 1 }
      }.call
    end

    a = Object.new
    a.untrust
    assert_equal(true, a.untrusted?)
    b = a.dup
    assert_equal(true, b.untrusted?)
    c = a.clone
    assert_equal(true, c.untrusted?)

    a = Object.new
    b = lambda {
      $SAFE = 4
      a.dup
    }.call
    assert_equal(true, b.untrusted?)

    a = Object.new
    b = lambda {
      $SAFE = 4
      a.clone
    }.call
    assert_equal(true, b.untrusted?)
  end

  def test_to_s
    x = Object.new
    x.taint
    x.untrust
    s = x.to_s
    assert_equal(true, s.untrusted?)
    assert_equal(true, s.tainted?)
  end

  def test_exec_recursive
    Thread.current[:__recursive_key__] = nil
    a = [[]]
    a.inspect

    assert_nothing_raised do
      -> do
        $SAFE = 4
        begin
          a.hash
        rescue ArgumentError
        end
      end.call
    end

    -> do
      assert_nothing_raised do
        $SAFE = 4
        a.inspect
      end
    end.call

    -> do
      o = Object.new
      def o.to_ary(x); end
      def o.==(x); $SAFE = 4; false; end
      a = [[o]]
      b = []
      b << b

      assert_nothing_raised do
        b == a
      end
    end.call
  end

  def test_singleton_class
    x = Object.new
    xs = class << x; self; end
    assert_equal(xs, x.singleton_class)

    y = Object.new
    ys = y.singleton_class
    assert_equal(class << y; self; end, ys)

    assert_equal(NilClass, nil.singleton_class)
    assert_equal(TrueClass, true.singleton_class)
    assert_equal(FalseClass, false.singleton_class)

    assert_raise(TypeError) do
      123.singleton_class
    end
    assert_raise(TypeError) do
      :foo.singleton_class
    end
  end

  def test_redef_method_missing
    bug5473 = '[ruby-core:40287]'
    ['ArgumentError.new("bug5473")', 'ArgumentError, "bug5473"', '"bug5473"'].each do |code|
      out, err, status = EnvUtil.invoke_ruby([], <<-SRC, true, true)
      class ::Object
        def method_missing(m, *a, &b)
          raise #{code}
        end
      end

      p((1.foo rescue $!))
      SRC
      assert_send([status, :success?], bug5473)
      assert_equal("", err, bug5473)
      assert_equal((eval("raise #{code}") rescue $!.inspect), out.chomp, bug5473)
    end
  end
end
