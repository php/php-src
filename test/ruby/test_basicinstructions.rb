require 'test/unit'

ConstTest = 3
class Class
  alias _remove_const remove_const
  public :_remove_const
end

class TestBasicInstructions < Test::Unit::TestCase

  def test_immediates
    assert_equal((1==1), true)
    assert_equal((1==2), false)
    assert_equal [][0], nil
    assert_equal "sym".intern, :sym
    assert_equal "sym".intern, :"sym"
    assert_equal 1234 + 0, 1234
    assert_equal 1234, 1_2_3_4
    assert_equal 41, 0b0101001
    assert_equal 420, 0644
    assert_equal 18, 0x12
    assert_equal 123456789012345678901234567890 + 0,
                 123456789012345678901234567890
    assert_equal 1.234 + 0.0, 1.234
  end

  def test_self
    assert_equal self, self
    assert_equal false, (self == false)   # Qfalse==0 in C
    assert_equal false, (self == nil)
    assert_equal false, (self == 0)
  end

  def test_string
    expected = "str" + "ing"
    assert_equal expected, 'string'
    assert_equal expected, "string"
    assert_equal expected, %q(string)
    assert_equal expected, %Q(string)
    assert_equal expected, %(string)
  end

  def test_dstring
    assert_equal "2", "#{1+1}"
    s = "OK"
    assert_equal "OK", "#{s}"
    assert_equal "OKx", "#{s}x"
    assert_equal "xOK", "x#{s}"
    assert_equal "xOKx", "x#{s}x"
  end

  def test_dsym
    assert_equal :a3c, :"a#{1+2}c"
    s = "sym"
    assert_equal :sym, :"#{s}"
    assert_equal :sym, :"#{"#{"#{s}"}"}"
  end

  def test_xstr
    assert_equal 'hoge', `echo hoge`.chomp
    assert_equal '3', `echo #{1 + 2}`.chomp
    hoge = 'huga'
    assert_equal 'huga', `echo #{hoge}`.chomp
  end

  def test_regexp
    assert_equal(/test/, /test/)
    assert_equal 'test', /test/.source
    assert_equal 'TEST', /TEST/.source
    assert_equal true, !!(/test/ =~ 'test')
    assert_equal false, !!(/test/ =~ 'does not match')

    re = /test/
    assert_equal re, re
    assert_equal 'test', re.source
    assert_equal true, !!(re =~ 'test')
    assert_equal false, !!(re =~ 'does not match')

    assert_equal(/x#{1+1}x/, /x#{1+1}x/)
    s = "OK"
    assert_equal(/x#{s}x/, /x#{s}x/)
    assert_equal true, !!(/x#{s}x/ =~ "xOKx")
    assert_equal false, !!(/x#{s}x/ =~ "does not match")

    s = "OK"
    prev = nil
    3.times do
      assert_equal prev.object_id, (prev ||= /#{s}/o).object_id  if prev
    end
  end

  def test_array
    assert_equal [], []
    assert_equal 0, [].size
    assert_equal [1, 2, 3], [1, 2, 3]
    assert_equal [3, 7, 11], [1+2, 3+4, 5+6]
    assert_equal [[1], [2], [3]], [[1], [2], [3]]

    a = [1, 2, 3]
    assert_equal 1, a[0]
    assert_equal 2, a[1]
    assert_equal 3, a[2]
    assert_nil a[3]

    a = %w( a b c )
    assert_equal 'a', a[0]
    assert_equal 'b', a[1]
    assert_equal 'c', a[2]
    assert_nil a[3]
  end

  def test_hash
    assert_equal({}, {})
    assert_equal({1=>2}, {1=>2})
    assert_equal({1=>2, 3=>4}, {1=>2, 3=>4})
    assert_equal({1=>2, 3=>4}, {3=>4, 1=>2})
    # assert_equal({1=>2, 3=>4}, {1,2, 3,4}) # 1.9 doesn't support
    assert_equal({"key"=>"val"}, {"key"=>"val"})
  end

  def test_range
    assert_equal((1..3), (1..3))
    assert_equal((1...3), (1...3))
    assert_not_equal((1...3), (1..3))
    assert_not_equal((1..3), (1...3))
    assert_equal((1..3), (1..2+1))
    assert_equal((1...3), (1...2+1))
    assert_equal(('a'..'z'), ('a'..'z'))
  end

  def test_not
    assert_equal true, !nil
    assert_equal true, !false
    assert_equal false, !true
    assert_equal false, !3
    assert_equal false, !(1+1)

    assert_equal false, !!nil
    assert_equal false, !!false
    assert_equal true, !!true
    assert_equal true, !!3
    assert_equal true, !!(1+1)

    assert_equal true, (not nil)
    assert_equal true, (not false)
    assert_equal false, (not true)
    assert_equal false, (not 3)
    assert_equal false, (not (1 + 1))

    assert_equal false, (not not nil)
    assert_equal false, (not not false)
    assert_equal true, (not not true)
    assert_equal true, (not not 3)
    assert_equal true, (not not (1+1))
  end

  def test_local_variable
    a = 7
    assert_equal 7, a
    assert_equal a, a
    b = 17
    assert_equal 7, a
    assert_equal 17, b
    assert_equal a, a
    assert_equal b, b
    assert_not_equal a, b
    assert_not_equal b, a
    a = b
    assert_equal 17, a
    assert_equal 17, b
    assert_equal a, a
    assert_equal b, b
    assert_equal a, b
    assert_equal b, a
    c = 28
    assert_equal 17, a
    assert_equal 17, b
    assert_equal 28, c
    assert_equal a, a
    assert_equal b, b
    assert_equal a, b
    assert_equal c, c
    assert_not_equal a, c
    assert_not_equal b, c
    a = b = c
    assert_equal 28, a
    assert_equal 28, b
    assert_equal 28, c
    assert_equal a, a
    assert_equal b, b
    assert_equal a, b
    assert_equal b, a
    assert_equal a, c
    assert_equal c, a
    assert_equal b, c
    assert_equal c, b

    a = 1
    b = 2
    c = 3
    set_lvar_in_another_method
    assert_equal 1, a
    assert_equal 2, b
    assert_equal 3, c
  end

  def set_lvar_in_another_method
    assert_raise(NameError) { a }
    assert_raise(NameError) { b }
    assert_raise(NameError) { c }
    a = "NOT OK"
    b = "NOT OK"
    c = "NOT OK"
  end

  class Const
    $Const = self
    C = 'Const::C'
    def self.c() C end
    def c() C end

    class A
      C = 'Const::A::C'
      def self.c() C end
      def c() C end
      CC = 'Const::A::CC'
      def self.cc() CC end
      def cc() CC end

      class B
        C = 'Const::A::B::C'
        def self.c() C end
        def c() C end
        def self.cc() CC end
        def cc() CC end
      end
    end

    class AA < A
      def self.cc() CC end
      def cc() CC end
    end

    class AAA < AA
      def self.cc() CC end
      def cc() CC end
    end
  end
  C = 0

  def test_const_path
    do_test_const_path
    do_test_const_path
    do_test_const_path
  end

  def do_test_const_path
    assert_equal 0, C
    assert_equal 0, C
    assert_equal 3, ::ConstTest
    assert_equal 3, ::ConstTest
    assert_equal $Const, Const

    assert_equal 'Const::C', Const::C
    assert_equal 'Const::C', Const::C
    assert_equal 'Const::A::C', Const::A::C
    assert_equal 'Const::A::C', Const::A::C
    assert_equal 'Const::A::B::C', Const::A::B::C
    assert_equal 'Const::A::B::C', Const::A::B::C

    Const::A::B._remove_const :C
    assert_equal 'Const::C', Const::C
    assert_equal 'Const::A::C', Const::A::C
    assert_raise(NameError) { Const::A::B::C }

    Const::A._remove_const :C
    assert_equal 'Const::C', Const::C
    assert_raise(NameError) { Const::A::C }
    assert_raise(NameError) { Const::A::B::C }

    Const._remove_const :C
    assert_raise(NameError) { Const::C }
    assert_raise(NameError) { Const::A::C }
    assert_raise(NameError) { Const::A::B::C }

    Const::A.const_set :C, 'Const::A::C'
    assert_raise(NameError) { Const::C }
    assert_equal 'Const::A::C', Const::A::C
    assert_raise(NameError) { Const::A::B::C }

    Const::A::B.const_set :C, 'Const::A::B::C'
    assert_raise(NameError) { Const::C }
    assert_equal 'Const::A::C', Const::A::C
    assert_equal 'Const::A::B::C', Const::A::B::C

    Const.const_set :C, 'Const::C'
    assert_equal 'Const::C', Const::C
    assert_equal 'Const::A::C', Const::A::C
    assert_equal 'Const::A::B::C', Const::A::B::C
  end

  def test_const_cref
    do_test_const_cref
    do_test_const_cref
    do_test_const_cref
  end

  def do_test_const_cref
    assert_equal 'Const::C', Const.new.c
    assert_equal 'Const::A::C', Const::A.new.c
    assert_equal 'Const::A::B::C', Const::A::B.new.c

    assert_equal 'Const::C', Const.c
    assert_equal 'Const::A::C', Const::A.c
    assert_equal 'Const::A::B::C', Const::A::B.c

    Const::A::B._remove_const :C
    assert_equal 'Const::C', Const.c
    assert_equal 'Const::A::C', Const::A.c
    assert_equal 'Const::A::C', Const::A::B.c
    assert_equal 'Const::C', Const.new.c
    assert_equal 'Const::A::C', Const::A.new.c
    assert_equal 'Const::A::C', Const::A::B.new.c

    Const::A._remove_const :C
    assert_equal 'Const::C', Const.c
    assert_equal 'Const::C', Const::A.c
    assert_equal 'Const::C', Const::A::B.c
    assert_equal 'Const::C', Const.new.c
    assert_equal 'Const::C', Const::A.new.c
    assert_equal 'Const::C', Const::A::B.new.c

    Const::A::B.const_set :C, 'Const::A::B::C'
    assert_equal 'Const::C', Const.c
    assert_equal 'Const::C', Const::A.c
    assert_equal 'Const::A::B::C', Const::A::B.c
    assert_equal 'Const::C', Const.new.c
    assert_equal 'Const::C', Const::A.new.c
    assert_equal 'Const::A::B::C', Const::A::B.new.c

    Const::A.const_set :C, 'Const::A::C'
    assert_equal 'Const::C', Const.c
    assert_equal 'Const::A::C', Const::A.c
    assert_equal 'Const::A::B::C', Const::A::B.c
    assert_equal 'Const::C', Const.new.c
    assert_equal 'Const::A::C', Const::A.new.c
    assert_equal 'Const::A::B::C', Const::A::B.new.c
  ensure
    # reset
    Const.const_set :C, 'Const::C' unless Const.const_defined?(:C)
    Const::A.const_set :C, 'Const::A::C' unless Const::A.const_defined?(:C)
    Const::A::B.const_set :C, 'Const::A::B::C' unless Const::A::B.const_defined?(:C)
  end

  def test_const_inherit
    do_test_const_inherit
    do_test_const_inherit
    do_test_const_inherit
  end

  def do_test_const_inherit
    assert_equal 'Const::A::CC',   Const::A.cc
    assert_equal 'Const::A::CC',   Const::AA.cc
    assert_equal 'Const::A::CC',   Const::AAA.cc
    assert_equal 'Const::A::CC',   Const::A.new.cc
    assert_equal 'Const::A::CC',   Const::AA.new.cc
    assert_equal 'Const::A::CC',   Const::AAA.new.cc

    Const::AA.const_set :CC, 'Const::AA::CC'
    assert_equal 'Const::A::CC',   Const::A.cc
    assert_equal 'Const::AA::CC',  Const::AA.cc
    assert_equal 'Const::AA::CC',  Const::AAA.cc
    assert_equal 'Const::A::CC',   Const::A.new.cc
    assert_equal 'Const::AA::CC',  Const::AA.new.cc
    assert_equal 'Const::AA::CC',  Const::AAA.new.cc

    Const::AAA.const_set :CC, 'Const::AAA::CC'
    assert_equal 'Const::A::CC',   Const::A.cc
    assert_equal 'Const::AA::CC',  Const::AA.cc
    assert_equal 'Const::AAA::CC', Const::AAA.cc
    assert_equal 'Const::A::CC',   Const::A.new.cc
    assert_equal 'Const::AA::CC',  Const::AA.new.cc
    assert_equal 'Const::AAA::CC', Const::AAA.new.cc

    Const::AA._remove_const :CC
    assert_equal 'Const::A::CC',   Const::A.cc
    assert_equal 'Const::A::CC',   Const::AA.cc
    assert_equal 'Const::AAA::CC', Const::AAA.cc
    assert_equal 'Const::A::CC',   Const::A.new.cc
    assert_equal 'Const::A::CC',   Const::AA.new.cc
    assert_equal 'Const::AAA::CC', Const::AAA.new.cc

    Const::AAA._remove_const :CC
    assert_equal 'Const::A::CC',   Const::A.cc
    assert_equal 'Const::A::CC',   Const::AA.cc
    assert_equal 'Const::A::CC',   Const::AAA.cc
    assert_equal 'Const::A::CC',   Const::A.new.cc
    assert_equal 'Const::A::CC',   Const::AA.new.cc
    assert_equal 'Const::A::CC',   Const::AAA.new.cc
  end

  def test_global_variable
    $gvar1 = 1
    assert_equal 1, $gvar1
    $gvar1 = 2
    assert_equal 2, $gvar1
    $gvar2 = 77
    assert_equal 2, $gvar1
    assert_equal 77, $gvar2
    $gvar2 = $gvar1
    assert_equal 2, $gvar1
    assert_equal 2, $gvar2
    $gvar1 = 1
    assert_equal 1, $gvar1
    assert_equal 2, $gvar2
    set_gvar_in_another_method
    assert_equal "OK1", $gvar1
    assert_equal "OK2", $gvar2
  end

  def set_gvar_in_another_method
    assert_equal 1, $gvar1
    assert_equal 2, $gvar2
    $gvar1 = "OK1"
    $gvar2 = "OK2"
  end

  class CVarA
    @@cv = 'CVarA@@cv'
    def self.cv() @@cv end
    def self.cv=(v) @@cv = v end
    class << self
      def cv2() @@cv end
    end
    def cv() @@cv end
    def cv=(v) @@cv = v end
  end

  class CVarB < CVarA
    def self.cvB() @@cv end
    def self.cvB=(v) @@cv = v end
    class << self
      def cvB2() @@cv end
    end
    def cvB() @@cv end
    def cvB=(v) @@cv = v end
  end

  def test_class_variable
    assert_equal 'CVarA@@cv',   CVarA.cv
    assert_equal 'CVarA@@cv',   CVarA.cv2
    assert_equal 'CVarA@@cv',   CVarA.new.cv
    CVarA.cv = 'singleton'
    assert_equal 'singleton',   CVarA.cv
    assert_equal 'singleton',   CVarA.cv2
    assert_equal 'singleton',   CVarA.new.cv
    CVarA.new.cv = 'instance'
    assert_equal 'instance',    CVarA.cv
    assert_equal 'instance',    CVarA.cv2
    assert_equal 'instance',    CVarA.new.cv

    CVarA.cv = 'CVarA@@cv'
    CVarB.cv = 'B/singleton'
    assert_equal 'B/singleton', CVarB.cv
    assert_equal 'B/singleton', CVarB.cv2
    assert_equal 'B/singleton', CVarB.new.cv
    assert_equal 'B/singleton', CVarA.cv
    assert_equal 'B/singleton', CVarA.cv2
    assert_equal 'B/singleton', CVarA.new.cv
    CVarB.new.cv = 'B/instance'
    assert_equal 'B/instance',  CVarB.cv
    assert_equal 'B/instance',  CVarB.cv2
    assert_equal 'B/instance',  CVarB.new.cv
    assert_equal 'B/instance',  CVarA.cv
    assert_equal 'B/instance',  CVarA.cv2
    assert_equal 'B/instance',  CVarA.new.cv

    CVarA.cv = 'CVarA@@cv'
    assert_equal('CVarA@@cv', CVarB.cvB)
    assert_equal('CVarA@@cv', CVarB.cvB2)
    assert_equal('CVarA@@cv', CVarB.new.cvB)
    CVarB.cvB = 'B/singleton'
    assert_equal 'B/singleton', CVarB.cvB
    assert_equal 'B/singleton', CVarB.cvB2
    assert_equal 'B/singleton', CVarB.new.cvB
    assert_equal 'B/singleton', CVarA.cv
    assert_equal 'B/singleton', CVarA.cv2
    assert_equal 'B/singleton', CVarA.new.cv
    CVarB.new.cvB = 'B/instance'
    assert_equal 'B/instance',  CVarB.cvB
    assert_equal 'B/instance',  CVarB.cvB2
    assert_equal 'B/instance',  CVarB.new.cvB
    assert_equal 'B/instance',  CVarA.cv
    assert_equal 'B/instance',  CVarA.cv2
    assert_equal 'B/instance',  CVarA.new.cv

    CVarA.cv = 'CVarA@@cv'
    CVarB.cvB = 'CVarB@@cv'
  end

  class OP
    attr_reader :x
    def x=(x)
      @x = x
      :Bug1996
    end
    Bug1996 = '[ruby-dev:39163], [ruby-core:25143]'
    def [](i)
      @x
    end
    def []=(i, x)
      @x = x
      :Bug2050
    end
    Bug2050 = '[ruby-core:25387]'
  end

  def test_opassign2_1
    x = nil
    assert_equal 1, x ||= 1
    assert_equal 1, x
    assert_equal 2, x &&= 2
    assert_equal 2, x
    assert_equal 2, x ||= 3
    assert_equal 2, x
    assert_equal 4, x &&= 4
    assert_equal 4, x
    assert_equal 5, x += 1
    assert_equal 5, x
    assert_equal 4, x -= 1
    assert_equal 4, x
  end

  def test_opassign2_2
    y = OP.new
    y.x = nil
    assert_equal 1, y.x ||= 1, OP::Bug1996
    assert_equal 1, y.x
    assert_equal 2, y.x &&= 2, OP::Bug1996
    assert_equal 2, y.x
    assert_equal 2, y.x ||= 3
    assert_equal 2, y.x
    assert_equal 4, y.x &&= 4, OP::Bug1996
    assert_equal 4, y.x
    assert_equal 5, y.x += 1, OP::Bug1996
    assert_equal 5, y.x
    assert_equal 4, y.x -= 1, OP::Bug1996
    assert_equal 4, y.x
  end

  def test_opassign2_3
    z = OP.new
    z.x = OP.new
    z.x.x = nil
    assert_equal 1, z.x.x ||= 1, OP::Bug1996
    assert_equal 1, z.x.x
    assert_equal 2, z.x.x &&= 2, OP::Bug1996
    assert_equal 2, z.x.x
    assert_equal 2, z.x.x ||= 3
    assert_equal 2, z.x.x
    assert_equal 4, z.x.x &&= 4, OP::Bug1996
    assert_equal 4, z.x.x
    assert_equal 5, z.x.x += 1, OP::Bug1996
    assert_equal 5, z.x.x
    assert_equal 4, z.x.x -= 1, OP::Bug1996
    assert_equal 4, z.x.x
  end

  def test_opassign1_1
    a = []
    a[0] = nil
    assert_equal 1, a[0] ||= 1
    assert_equal 1, a[0]
    assert_equal 2, a[0] &&= 2
    assert_equal 2, a[0]
    assert_equal 2, a[0] ||= 3
    assert_equal 2, a[0]
    assert_equal 4, a[0] &&= 4
    assert_equal 4, a[0]
    assert_equal 5, a[0] += 1
    assert_equal 5, a[0]
    assert_equal 4, a[0] -= 1
    assert_equal 4, a[0]
  end

  def test_opassign1_2
    x = OP.new
    x[0] = nil
    assert_equal 1, x[0] ||= 1, OP::Bug2050
    assert_equal 1, x[0]
    assert_equal 2, x[0] &&= 2, OP::Bug2050
    assert_equal 2, x[0]
    assert_equal 2, x[0] ||= 3, OP::Bug2050
    assert_equal 2, x[0]
    assert_equal 4, x[0] &&= 4, OP::Bug2050
    assert_equal 4, x[0]
    assert_equal 5, x[0] += 1, OP::Bug2050
    assert_equal 5, x[0]
    assert_equal 4, x[0] -= 1, OP::Bug2050
    assert_equal 4, x[0]
  end

  def test_backref
    /re/ =~ 'not match'
    assert_nil $~
    assert_nil $`
    assert_nil $&
    assert_nil $'
    assert_nil $+
    assert_nil $1
    assert_nil $2
    assert_nil $3
    assert_nil $4
    assert_nil $5
    assert_nil $6
    assert_nil $7
    assert_nil $8
    assert_nil $9

    /(a)(b)(c)(d)(e)(f)(g)(h)(i)/ =~ 'xabcdefghiy'
    assert_not_nil $~
    assert_instance_of MatchData, $~
    assert_equal 'abcdefghi', $~[0]
    assert_equal 'a', $~[1]
    assert_equal 'b', $~[2]
    assert_equal 'c', $~[3]
    assert_equal 'd', $~[4]
    assert_equal 'e', $~[5]
    assert_equal 'f', $~[6]
    assert_equal 'g', $~[7]
    assert_equal 'h', $~[8]
    assert_equal 'i', $~[9]
    assert_equal 'x', $`
    assert_equal 'abcdefghi', $&
    assert_equal "y", $'
    assert_equal 'i', $+
    assert_equal 'a', $1
    assert_equal 'b', $2
    assert_equal 'c', $3
    assert_equal 'd', $4
    assert_equal 'e', $5
    assert_equal 'f', $6
    assert_equal 'g', $7
    assert_equal 'h', $8
    assert_equal 'i', $9

    /re/ =~ 'not match'
    assert_nil $~
    assert_nil $`
    assert_nil $&
    assert_nil $'
    assert_nil $+
    assert_nil $1
    assert_nil $2
    assert_nil $3
    assert_nil $4
    assert_nil $5
    assert_nil $6
    assert_nil $7
    assert_nil $8
    assert_nil $9
  end

  def test_array_splat
    feature1125 = '[ruby-core:21901]'

    a = []
    assert_equal [], [*a]
    assert_equal [1], [1, *a]
    assert_not_same(a, [*a], feature1125)
    a = [2]
    assert_equal [2], [*a]
    assert_equal [1, 2], [1, *a]
    assert_not_same(a, [*a], feature1125)
    a = [2, 3]
    assert_equal [2, 3], [*a]
    assert_equal [1, 2, 3], [1, *a]
    assert_not_same(a, [*a], feature1125)

    a = nil
    assert_equal [], [*a]
    assert_equal [1], [1, *a]
  end

end
