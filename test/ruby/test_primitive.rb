require 'test/unit'

class TestRubyPrimitive < Test::Unit::TestCase

  def test_not
    assert_equal false, !true
    assert_equal true, !false
    assert_equal true, !nil
    assert_equal false, !(1+1)
    assert_equal false, !!nil
    assert_equal true, !!1
  end

  def test_lvar
    a = 1
    assert_equal 1, a
    b = 2
    assert_equal 1, a
    a = b = 3
    assert_equal 3, a
    assert_equal 3, b
    a = b = c = 4
    assert_equal 4, a
    assert_equal 4, b
    assert_equal 4, c
  end

  C = 1
  class A
    Const = 1
    class B
      Const = 2
      class C
        Const = 3
        def const
          Const
        end
      end
    end
  end
  (1..2).map {
    A::B::C::Const
  }

  def test_constant
    assert_equal 1, C
    assert_equal 1, C
    assert_equal 1, A::Const
    assert_equal 2, A::B::Const
    assert_equal 3, A::B::C::Const
    assert_equal 3, A::B::C.new.const
    assert_equal 1, ::TestRubyPrimitive::A::Const
    A::B::C.send(:remove_const, :Const)
    assert_equal 2, A::B::C.new.const
    assert_raise(TypeError) {
      C::CONST
    }
  end

  class A2
    class B2
      class C2
        C = 7
      end
    end
  end

  def test_constant_cache
    i = 0
    while i < 3
      r = A2::B2::C2::C
      i += 1
    end
    assert_equal 7, r
  end

  class A3
    class B3
      C = 99
    end
  end
  i = 0
  while i < 3
    r = A3::B3::C   # cache
    class A3::B3
      remove_const :C
    end
    A3::B3::C = i ** i
    i += 1
  end

  def test_constant_cache2
    assert_equal 4, A3::B3::C
  end

  class A4
    Const = 7
    (1..3).map {
      $test_ruby_primitive_constant_cache3 = self::Const
    }
  end

  def test_constant_cache3
    assert_equal 7, $test_ruby_primitive_constant_cache3
  end

  class A5
    Const = 8
    (1..3).map {
      $test_ruby_primitive_constant_cache4 = eval('self')::Const
    }
  end

  def test_constatant_cache4
    assert_equal 8, $test_ruby_primitive_constant_cache4
  end

  class A6
    Const = 0
    def self.foo
      self::Const
    end
  end
  class B6 < A6
    Const = 1
  end
  class C6 < B6
    Const = 2
  end
  $test_ruby_primitive_constant_cache5 = [A6.foo, B6.foo, C6.foo]

  def test_constant_cache5
    assert_equal [0, 1, 2], $test_ruby_primitive_constant_cache5
  end

  def test_gvar
    $test_ruby_primitive_gvar = 7
    assert_equal 7, $test_ruby_primitive_gvar
    assert_equal 7, $test_ruby_primitive_gvar
    $test_ruby_primitive_gvar = 88
    assert_equal 88, $test_ruby_primitive_gvar
    assert_equal 88, $test_ruby_primitive_gvar
    assert_equal 7, ($test_ruby_primitive_gvar = 7)
    assert_equal 7, ($test_ruby_primitive_gvar = 7)
  end

  class A7
    @@c = 1
    def m
      @@c += 1
    end
  end

  def test_cvar_from_instance_method
    assert_equal 2, A7.new.m
    assert_equal 3, A7.new.m
    assert_equal 4, A7.new.m
  end

  class A8
    @@c = 1
    class << self
      def m
        @@c += 1
      end
    end
  end

  def test_cvar_from_singleton_method
    assert_equal 2, A8.m
    assert_equal 3, A8.m
    assert_equal 4, A8.m
  end

  class A9
    @@c = 1
    def self.m
      @@c += 1
    end
  end

  def test_cvar_from_singleton_method2
    assert_equal 2, A9.m
    assert_equal 3, A9.m
    assert_equal 4, A9.m
  end

  class A10
    attr_accessor :a
  end

  def test_opassign
    i = 0
    i += 1
    assert_equal 1, i

    @iv = 2
    @iv += 2
    assert_equal 4, @iv

    @@cv ||= 1
    assert_equal 1, @@cv
    @@cv &&= 2
    assert_equal 2, @@cv
    @@cv ||= 99
    assert_equal 2, @@cv

    $gv = 3
    $gv += 4
    assert_equal 7, $gv

    obj = A10.new
    obj.a = 9
    obj.a &&= 7
    assert_equal 7, obj.a

    obj.a = nil
    obj.a ||= 2
    assert_equal 2, obj.a

    obj.a &&= 3
    assert_equal 3, obj.a

    a = []
    a[0] ||= 3
    assert_equal 3, a[0]
    a[0] &&= 7
    assert_equal 7, a[0]
    a[0] ||= 3
    assert_equal 7, a[0]

    a = [0, 1, nil, 3, 4]
    a[*[2]] ||= :foo
    assert_equal [0, 1, :foo, 3, 4], a
    a[*[1,3]] &&= [:bar]
    assert_equal [0, :bar, 4], a
  end

  def test_opassign_and_or
    a = 1
    a ||= 2
    assert_equal 1, a
    a = nil
    a ||= 2
    assert_equal 2, a
    a = 1
    a &&= 3
    assert_equal 3, a
    a = nil
    a &&= 4
    assert_nil a

    h = {}
    h[0] ||= 1
    assert_equal 1, h[0]
    h = {}
    h[0] &&= 1
    assert_nil h[0]
    h = {0 => 7}
    h[0] ||= 1
    assert_equal 7, h[0]
    h = {0 => 7}
    h[0] &&= 1
    assert_equal 1, h[0]
  end

  def test_backref
    /a(b)(c)d/ =~ 'xyzabcdefgabcdefg'
    assert_equal 'b', $1
    assert_equal 'c', $2
    assert_nil $3
    assert_instance_of MatchData, $~
    assert_equal 'abcd', $&
    assert_equal 'xyz', $`
    assert_equal 'efgabcdefg', $'
    assert_equal 'c', $+

    /(?!)/ =~ 'xyzabcdefgabcdefg'
    assert_nil $1
    assert_nil $2
    assert_nil $3
    assert_nil $~
    assert_nil $&
    assert_nil $`
    assert_nil $'
    assert_nil $+
  end

  def test_fact
    assert_equal 306057512216440636035370461297268629388588804173576999416776741259476533176716867465515291422477573349939147888701726368864263907759003154226842927906974559841225476930271954604008012215776252176854255965356903506788725264321896264299365204576448830388909753943489625436053225980776521270822437639449120128678675368305712293681943649956460498166450227716500185176546469340112226034729724066333258583506870150169794168850353752137554910289126407157154830282284937952636580145235233156936482233436799254594095276820608062232812387383880817049600000000000000000000000000000000000000000000000000000000000000000000000000, fact(300)
  end

  def fact(n)
    if n > 1
      n * fact(n - 1)
    else
      1
    end
  end

  def test_mul
    assert_equal 0, 2 * 0
    assert_equal 0, 0 * 2
    assert_equal 4, 2 * 2
  end

  class MyNum
    def /(a)
      a * 100
    end
  end

  def test_div
    assert_equal 1, 3 / 2
    assert_equal 1.5, 3.0 / 2.0
    assert_equal 300, MyNum.new / 3
  end

  class MyArr
    def length
      'string'
    end
  end

  def test_length
    assert_equal 0, [].length
    assert_equal 1, [1].length
    assert_equal 2, [1,2].length
    assert_equal 0, {}.length
    assert_equal 1, {1=>1}.length
    assert_equal 2, {1=>1, 2=>2}.length
    assert_equal 'string', MyArr.new.length
  end

  class MyNum2
    def %(a)
      a * 100
    end
  end

  def test_mod
    assert_equal 2, 5 % 3
    assert_equal 1.0, 3.0 % 2.0
    assert_equal 300, MyNum2.new % 3
  end

  class MyObj
    def [](*args)
      args
    end

    def []=(*args)
      args
    end
  end

  def test_aref
    a = [0,1]
    assert_equal 0, a[0]
    assert_equal 1, a[1]
    assert_nil a[2]
    h = {0=>0, 1=>1}
    obj = MyObj.new
    assert_equal 0, h[0]
    assert_equal 1, h[1]
    assert_nil h[2]
    assert_equal [0], obj[0]
    assert_equal [0,1], obj[0,1]
    assert_equal [0,1,2], obj[0,1,2]
  end

  def test_aset
    obj = MyObj.new
    assert_equal 7, (obj[0] = 7)
    assert_equal 7, (obj[0,1] = 7)
    assert_equal 7, (obj[0,1,2] = 7)
  end

  class MyObj2
    def attr=(*args)
      args
    end
  end

  def test_attr_setter
    obj = MyObj2.new
    assert_equal 1, (obj.attr = 1)
  end

  def test_list_expand
    a = []
    assert_equal [0], [0, *a]
    a = [1]
    assert_equal [0,1], [0, *a]
    a = [1,2]
    assert_equal [0,1,2], [0, *a]
    a = [1,2,3]
    assert_equal [0,1,2,3], [0, *a]
    #a = [1,2,3]
    #assert_equal [0,1,2,3,4], [0, *a, 4]
  end

  def test_concatarray_ruby_dev_41933
    bug3658 = '[ruby-dev:41933]'
    [0, *x=1]
    assert_equal(1, x, bug3658)
    [0, *x=1, 2]
    assert_equal(1, x, bug3658)
    class << (x = Object.new)
      attr_accessor :to_a_called
      def to_a
        @to_a_called = true
        [self]
      end
    end
    x.to_a_called = false
    [0, *x]
    assert(x.to_a_called, bug3658)
    x.to_a_called = false
    [0, *x, 2]
    assert(x.to_a_called, bug3658)
  end
end
