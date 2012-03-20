require 'test/unit'
require 'stringio'

class TestParse < Test::Unit::TestCase
  def setup
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def test_else_without_rescue
    x = eval <<-END
      begin
      else
        42
      end
    END
    assert_equal(42, x)
  end

  def test_alias_backref
    assert_raise(SyntaxError) do
      eval <<-END
        alias $foo $1
      END
    end
  end

  def test_command_call
    t = Object.new
    def t.foo(x); x; end

    a = false
    b = c = d = true
    assert_nothing_raised do
      eval <<-END
        a &&= t.foo 42
        b &&= t.foo 42
        c &&= t.foo nil
        d &&= t.foo false
      END
    end
    assert_equal([false, 42, nil, false], [a, b, c, d])

    a = 3
    assert_nothing_raised { eval("a &= t.foo 5") }
    assert_equal(1, a)

    a = [nil, nil, true, true]
    assert_nothing_raised do
      eval <<-END
        a[0] ||= t.foo 42
        a[1] &&= t.foo 42
        a[2] ||= t.foo 42
        a[3] &&= t.foo 42
      END
    end
    assert_equal([42, nil, true, 42], a)

    o = Object.new
    class << o
      attr_accessor :foo, :bar, :Foo, :Bar, :baz, :qux
    end
    o.foo = o.Foo = o::baz = nil
    o.bar = o.Bar = o::qux = 1
    assert_nothing_raised do
      eval <<-END
        o.foo ||= t.foo 42
        o.bar &&= t.foo 42
        o.Foo ||= t.foo 42
        o.Bar &&= t.foo 42
        o::baz ||= t.foo 42
        o::qux &&= t.foo 42
      END
    end
    assert_equal([42, 42], [o.foo, o.bar])
    assert_equal([42, 42], [o.Foo, o.Bar])
    assert_equal([42, 42], [o::baz, o::qux])

    assert_raise(SyntaxError) do
      eval <<-END
        $1 ||= t.foo 42
      END
    end

    def t.bar(x); x + yield; end

    a = b = nil
    assert_nothing_raised do
      eval <<-END
        a = t.bar "foo" do
          "bar"
        end.gsub "ob", "OB"
        b = t.bar "foo" do
          "bar"
        end::gsub "ob", "OB"
      END
    end
    assert_equal("foOBar", a)
    assert_equal("foOBar", b)

    a = nil
    assert_nothing_raised do
      t.instance_eval <<-END
        a = bar "foo" do "bar" end
      END
    end
    assert_equal("foobar", a)

    a = nil
    assert_nothing_raised do
      eval <<-END
        a = t::bar "foo" do "bar" end
      END
    end
    assert_equal("foobar", a)

    def t.baz(*r)
      @baz = r + (block_given? ? [yield] : [])
    end

    assert_nothing_raised do
      t.instance_eval "baz (1), 2"
    end
    assert_equal([1, 2], t.instance_eval { @baz })
  end

  def test_mlhs_node
    c = Class.new
    class << c
      attr_accessor :foo, :bar, :Foo, :Bar
      FOO = BAR = nil
    end

    assert_nothing_raised do
      eval <<-END
        c::foo, c::bar = 1, 2
        c.Foo, c.Bar = 1, 2
        c::FOO, c::BAR = 1, 2
      END
    end
    assert_equal([1, 2], [c::foo, c::bar])
    assert_equal([1, 2], [c.Foo, c.Bar])
    assert_equal([1, 2], [c::FOO, c::BAR])
  end

  def test_dynamic_constant_assignment
    assert_raise(SyntaxError) do
      Object.new.instance_eval <<-END
        def foo
          self::FOO, self::BAR = 1, 2
          ::FOO, ::BAR = 1, 2
        end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        $1, $2 = 1, 2
      END
    end

    assert_raise(SyntaxError) do
      Object.new.instance_eval <<-END
        def foo
          ::FOO = 1
        end
      END
    end

    c = Class.new
    assert_raise(SyntaxError) do
      eval <<-END
        c::FOO &= 1
        ::FOO &= 1
      END
    end

    c = Class.new
    assert_raise(SyntaxError) do
      eval <<-END
        $1 &= 1
      END
    end
  end

  def test_class_module
    assert_raise(SyntaxError) do
      eval <<-END
        class foo; end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        def foo
          class Foo; end
          module Bar; end
        end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        class Foo Bar; end
      END
    end
  end

  def test_op_name
    o = Object.new
    def o.>(x); x; end
    def o./(x); x; end

    a = nil
    assert_nothing_raised do
      o.instance_eval <<-END
        undef >, /
      END
    end
  end

  def test_arg
    o = Object.new
    class << o
      attr_accessor :foo, :bar, :Foo, :Bar, :baz, :qux
    end
    o.foo = o.Foo = o::baz = nil
    o.bar = o.Bar = o::qux = 1
    assert_nothing_raised do
      eval <<-END
        o.foo ||= 42
        o.bar &&= 42
        o.Foo ||= 42
        o.Bar &&= 42
        o::baz ||= 42
        o::qux &&= 42
      END
    end
    assert_equal([42, 42], [o.foo, o.bar])
    assert_equal([42, 42], [o.Foo, o.Bar])
    assert_equal([42, 42], [o::baz, o::qux])

    a = nil
    assert_nothing_raised do
      eval <<-END
        a = -2.0 ** 2
      END
    end
    assert_equal(-4.0, a)
  end

  def test_block_variable
    o = Object.new
    def o.foo(*r); yield(*r); end

    a = nil
    assert_nothing_raised do
      eval <<-END
        o.foo 1 do|; a| a = 42 end
      END
    end
    assert_nil(a)
  end

  def test_bad_arg
    assert_raise(SyntaxError) do
      eval <<-END
        def foo(FOO); end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        def foo(@foo); end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        def foo($foo); end
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        def foo(@@foo); end
      END
    end

    o = Object.new
    def o.foo(*r); yield(*r); end

    assert_raise(SyntaxError) do
      eval <<-END
        o.foo 1 {|; @a| @a = 42 }
      END
    end
  end

  def test_do_lambda
    a = b = nil
    assert_nothing_raised do
      eval <<-END
        a = -> do
          b = 42
        end
      END
    end
    a.call
    assert_equal(42, b)
  end

  def test_block_call_colon2
    o = Object.new
    def o.foo(x); x + yield; end

    a = b = nil
    assert_nothing_raised do
      o.instance_eval <<-END
        a = foo 1 do 42 end.to_s
        b = foo 1 do 42 end::to_s
      END
    end
    assert_equal("43", a)
    assert_equal("43", b)
  end

  def test_call_method
    a = b = nil
    assert_nothing_raised do
      eval <<-END
        a = proc {|x| x + "bar" }.("foo")
        b = proc {|x| x + "bar" }::("foo")
      END
    end
    assert_equal("foobar", a)
    assert_equal("foobar", b)
  end

  def test_xstring
    assert_raise(Errno::ENOENT) do
      eval("``")
    end
  end

  def test_words
    assert_equal([], %W( ))
  end

  def test_dstr
    @@foo = 1
    assert_equal("foo 1 bar", "foo #@@foo bar")
    "1" =~ /(.)/
    assert_equal("foo 1 bar", "foo #$1 bar")
  end

  def test_dsym
    assert_nothing_raised { eval(':""') }
  end

  def test_arg2
    o = Object.new

    assert_nothing_raised do
      eval <<-END
        def o.foo(a=42,*r,z,&b); b.call(r.inject(a*1000+z*100, :+)); end
      END
    end
    assert_equal(-1405, o.foo(1,2,3,4) {|x| -x })
    assert_equal(-1302, o.foo(1,2,3) {|x| -x })
    assert_equal(-1200, o.foo(1,2) {|x| -x })
    assert_equal(-42100, o.foo(1) {|x| -x })
    assert_raise(ArgumentError) { o.foo() }

    assert_nothing_raised do
      eval <<-END
        def o.foo(a=42,z,&b); b.call(a*1000+z*100); end
      END
    end
    assert_equal(-1200, o.foo(1,2) {|x| -x } )
    assert_equal(-42100, o.foo(1) {|x| -x } )
    assert_raise(ArgumentError) { o.foo() }

    assert_nothing_raised do
      eval <<-END
        def o.foo(*r,z,&b); b.call(r.inject(z*100, :+)); end
      END
    end
    assert_equal(-303, o.foo(1,2,3) {|x| -x } )
    assert_equal(-201, o.foo(1,2) {|x| -x } )
    assert_equal(-100, o.foo(1) {|x| -x } )
    assert_raise(ArgumentError) { o.foo() }
  end

  def test_duplicate_argument
    assert_raise(SyntaxError) do
      eval <<-END
        1.times {|&b?| }
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        1.times {|a, a|}
      END
    end

    assert_raise(SyntaxError) do
      eval <<-END
        def foo(a, a); end
      END
    end
  end

  def test_define_singleton_error
    assert_raise(SyntaxError) do
      eval <<-END
        def ("foo").foo; end
      END
    end
  end

  def test_backquote
    t = Object.new

    assert_nothing_raised do
      eval <<-END
        def t.`(x); "foo" + x + "bar"; end
      END
    end
    a = b = nil
    assert_nothing_raised do
      eval <<-END
        a = t.` "zzz"
        1.times {|;z| t.` ("zzz") }
      END
      t.instance_eval <<-END
        b = `zzz`
      END
    end
    assert_equal("foozzzbar", a)
    assert_equal("foozzzbar", b)
  end

  def test_carrige_return
    assert_equal(2, eval("1 +\r\n1"))
  end

  def test_string
    assert_raise(SyntaxError) do
      eval '"\xg1"'
    end

    assert_raise(SyntaxError) do
      eval '"\u{1234"'
    end

    assert_raise(SyntaxError) do
      eval '"\M1"'
    end

    assert_raise(SyntaxError) do
      eval '"\C1"'
    end

    assert_equal("\x81", eval('"\C-\M-a"'))
    assert_equal("\177", eval('"\c?"'))
  end

  def test_question
    assert_raise(SyntaxError) { eval('?') }
    assert_raise(SyntaxError) { eval('? ') }
    assert_raise(SyntaxError) { eval("?\n") }
    assert_raise(SyntaxError) { eval("?\t") }
    assert_raise(SyntaxError) { eval("?\v") }
    assert_raise(SyntaxError) { eval("?\r") }
    assert_raise(SyntaxError) { eval("?\f") }
    assert_equal("\u{1234}", eval("?\u{1234}"))
    assert_equal("\u{1234}", eval('?\u{1234}'))
  end

  def test_percent
    assert_equal(:foo, eval('%s(foo)'))
    assert_raise(SyntaxError) { eval('%s') }
    assert_raise(SyntaxError) { eval('%ss') }
    assert_raise(SyntaxError) { eval('%z()') }
  end

  def test_symbol
    bug = '[ruby-dev:41447]'
    sym = "foo\0bar".to_sym
    assert_nothing_raised(SyntaxError, bug) do
      assert_equal(sym, eval(":'foo\0bar'"))
    end
    assert_nothing_raised(SyntaxError, bug) do
      assert_equal(sym, eval(':"foo\u0000bar"'))
    end
    assert_nothing_raised(SyntaxError, bug) do
      assert_equal(sym, eval(':"foo\u{0}bar"'))
    end
    assert_raise(SyntaxError) do
      eval ':"foo\u{}bar"'
    end
  end

  def test_parse_string
    assert_raise(SyntaxError) do
      eval <<-END
/
      END
    end
  end

  def test_here_document
    x = nil

    assert_raise(SyntaxError) do
      eval %Q(
<\<FOO
      )
    end

    assert_raise(SyntaxError) do
      eval %q(
<<FOO
#$
FOO
      )
    end

    assert_raise(SyntaxError) do
      eval %Q(
<\<\"
      )
    end

    assert_raise(SyntaxError) do
      eval %q(
<<``
      )
    end

    assert_raise(SyntaxError) do
      eval %q(
<<--
      )
    end

    assert_raise(SyntaxError) do
      eval %q(
<<FOO
#$
foo
FOO
      )
    end

    assert_nothing_raised do
      eval "x = <<""FOO\r\n1\r\nFOO"
    end
    assert_equal("1\n", x)
  end

  def test_magic_comment
    x = nil
    assert_nothing_raised do
      eval <<-END
# coding = utf-8
x = __ENCODING__
      END
    end
    assert_equal(Encoding.find("UTF-8"), x)

    assert_raise(ArgumentError) do
      eval <<-END
# coding = foobarbazquxquux_dummy_enconding
x = __ENCODING__
      END
    end
  end

  def test_utf8_bom
    x = nil
    assert_nothing_raised do
      eval "\xef\xbb\xbf x = __ENCODING__"
    end
    assert_equal(Encoding.find("UTF-8"), x)
    assert_raise(NameError) { eval "\xef" }
  end

  def test_dot_in_next_line
    x = nil
    assert_nothing_raised do
      eval <<-END
        x = 1
        .to_s
      END
    end
    assert_equal("1", x)
  end

  def test_pow_asgn
    x = 3
    assert_nothing_raised { eval("x **= 2") }
    assert_equal(9, x)
  end

  def test_embedded_rd
    assert_raise(SyntaxError) do
      eval <<-END
=begin
      END
    end
  end

  def test_float
    assert_equal(1.0/0, eval("1e10000"))
    assert_raise(SyntaxError) { eval('1_E') }
    assert_raise(SyntaxError) { eval('1E1E1') }
  end

  def test_global_variable
    assert_equal(nil, eval('$-x'))
    assert_equal(nil, eval('alias $preserve_last_match $&'))
    assert_equal(nil, eval('alias $& $test_parse_foobarbazqux'))
    $test_parse_foobarbazqux = nil
    assert_equal(nil, $&)
    assert_equal(nil, eval('alias $& $preserve_last_match'))
    assert_raise(SyntaxError) { eval('$#') }
  end

  def test_invalid_instance_variable
    assert_raise(SyntaxError) { eval('@#') }
  end

  def test_invalid_class_variable
    assert_raise(SyntaxError) { eval('@@1') }
  end

  def test_invalid_char
    x = 1
    assert_equal(1, eval("\x01x"))
    assert_equal(nil, eval("\x04x"))
  end

  def test_literal_concat
    x = "baz"
    assert_equal("foobarbaz", eval('"foo" "bar#{x}"'))
  end

  def test_unassignable
    assert_raise(SyntaxError) do
      eval %q(self = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(nil = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(true = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(false = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(__FILE__ = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(__LINE__ = 1)
    end
    assert_raise(SyntaxError) do
      eval %q(__ENCODING__ = 1)
    end
    assert_raise(SyntaxError) do
      eval <<-END
        def foo
          FOO = 1
        end
      END
    end
  end

  def test_block_dup
    assert_raise(SyntaxError) do
      eval <<-END
        foo(&proc{}) {}
      END
    end
  end

  def test_set_backref
    assert_raise(SyntaxError) do
      eval <<-END
        $& = 1
      END
    end
  end

  def test_arg_concat
    o = Object.new
    class << o; self; end.instance_eval do
      define_method(:[]=) {|*r, &b| b.call(r) }
    end
    r = nil
    assert_nothing_raised do
      eval <<-END
        o[&proc{|x| r = x }] = 1
      END
    end
    assert_equal([1], r)
  end

  def test_void_expr_stmts_value
    # This test checks if void contexts are warned correctly.
    # Thus, warnings MUST NOT be suppressed.
    $VERBOSE = true
    stderr = $stderr
    $stderr = StringIO.new("")
    x = 1
    assert_nil eval("x; nil")
    assert_nil eval("1+1; nil")
    assert_nil eval("TestParse; nil")
    assert_nil eval("::TestParse; nil")
    assert_nil eval("x..x; nil")
    assert_nil eval("x...x; nil")
    assert_nil eval("self; nil")
    assert_nil eval("nil; nil")
    assert_nil eval("true; nil")
    assert_nil eval("false; nil")
    assert_nil eval("defined?(1); nil")

    assert_raise(SyntaxError) do
      eval %q(1; next; 2)
    end

    o = Object.new
    assert_nothing_raised do
      eval <<-END
        x = def o.foo; end
      END
    end
    assert_equal(14, $stderr.string.lines.to_a.size)
    $stderr = stderr
  end

  def test_assign_in_conditional
    assert_raise(SyntaxError) do
      eval <<-END
        (x, y = 1, 2) ? 1 : 2
      END
    end

    assert_nothing_raised do
      eval <<-END
        if @x = true
          1
        else
          2
        end
      END
    end
  end

  def test_literal_in_conditional
    assert_nothing_raised do
      eval <<-END
        "foo" ? 1 : 2
      END
    end

    assert_nothing_raised do
      x = "bar"
      eval <<-END
        /foo#{x}baz/ ? 1 : 2
      END
    end

    assert_nothing_raised do
      eval <<-END
        (true..false) ? 1 : 2
      END
    end

    assert_nothing_raised do
      eval <<-END
        ("foo".."bar") ? 1 : 2
      END
    end

    assert_nothing_raised do
      x = "bar"
      eval <<-END
        :"foo#{"x"}baz" ? 1 : 2
      END
    end
  end

  def test_no_blockarg
    assert_raise(SyntaxError) do
      eval <<-END
        yield(&:+)
      END
    end
  end

  def test_intern
    assert_equal(':""', ''.intern.inspect)
    assert_equal(':$foo', '$foo'.intern.inspect)
    assert_equal(':"!foo"', '!foo'.intern.inspect)
    assert_equal(':"foo=="', "foo==".intern.inspect)
  end

  def test_all_symbols
    x = Symbol.all_symbols
    assert_kind_of(Array, x)
    assert(x.all? {|s| s.is_a?(Symbol) })
  end

  def test_is_class_id
    c = Class.new
    assert_raise(NameError) do
      c.instance_eval { remove_class_variable(:@var) }
    end
  end

  def test_method_block_location
    bug5614 = '[ruby-core:40936]'
    expected = nil
    e = assert_raise(NoMethodError) do
      1.times do
        expected = __LINE__+1
      end.print do
        #
      end
    end
    actual = e.backtrace.first[/\A#{Regexp.quote(__FILE__)}:(\d+):/o, 1].to_i
    assert_equal(expected, actual, bug5614)
  end
end
