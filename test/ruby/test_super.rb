require 'test/unit'

class TestSuper < Test::Unit::TestCase
  class Base
    def single(a) a end
    def double(a, b) [a,b] end
    def array(*a) a end
    def optional(a = 0) a end
  end
  class Single1 < Base
    def single(*) super end
  end
  class Single2 < Base
    def single(a,*) super end
  end
  class Double1 < Base
    def double(*) super end
  end
  class Double2 < Base
    def double(a,*) super end
  end
  class Double3 < Base
    def double(a,b,*) super end
  end
  class Array1 < Base
    def array(*) super end
  end
  class Array2 < Base
    def array(a,*) super end
  end
  class Array3 < Base
    def array(a,b,*) super end
  end
  class Array4 < Base
    def array(a,b,c,*) super end
  end
  class Optional1 < Base
    def optional(a = 1) super end
  end
  class Optional2 < Base
    def optional(a, b = 1) super end
  end
  class Optional3 < Base
    def single(a = 1) super end
  end
  class Optional4 < Base
    def array(a = 1, *) super end
  end
  class Optional5 < Base
    def array(a = 1, b = 2, *) super end
  end

  def test_single1
    assert_equal(1, Single1.new.single(1))
  end
  def test_single2
    assert_equal(1, Single2.new.single(1))
  end
  def test_double1
    assert_equal([1, 2], Double1.new.double(1, 2))
  end
  def test_double2
    assert_equal([1, 2], Double2.new.double(1, 2))
  end
  def test_double3
    assert_equal([1, 2], Double3.new.double(1, 2))
  end
  def test_array1
    assert_equal([], Array1.new.array())
    assert_equal([1], Array1.new.array(1))
  end
  def test_array2
    assert_equal([1], Array2.new.array(1))
    assert_equal([1,2], Array2.new.array(1, 2))
  end
  def test_array3
    assert_equal([1,2], Array3.new.array(1, 2))
    assert_equal([1,2,3], Array3.new.array(1, 2, 3))
  end
  def test_array4
    assert_equal([1,2,3], Array4.new.array(1, 2, 3))
    assert_equal([1,2,3,4], Array4.new.array(1, 2, 3, 4))
  end
  def test_optional1
    assert_equal(9, Optional1.new.optional(9))
    assert_equal(1, Optional1.new.optional)
  end
  def test_optional2
    assert_raise(ArgumentError) do
      # call Base#optional with 2 arguments; the 2nd arg is supplied
      assert_equal(9, Optional2.new.optional(9))
    end
    assert_raise(ArgumentError) do
      # call Base#optional with 2 arguments
      assert_equal(9, Optional2.new.optional(9, 2))
    end
  end
  def test_optional3
    assert_equal(9, Optional3.new.single(9))
    # call Base#single with 1 argument; the arg is supplied
    assert_equal(1, Optional3.new.single)
  end
  def test_optional4
    assert_equal([1], Optional4.new.array)
    assert_equal([9], Optional4.new.array(9))
    assert_equal([9, 8], Optional4.new.array(9, 8))
  end
  def test_optional5
    assert_equal([1, 2], Optional5.new.array)
    assert_equal([9, 2], Optional5.new.array(9))
    assert_equal([9, 8], Optional5.new.array(9, 8))
    assert_equal([9, 8, 7], Optional5.new.array(9, 8, 7))
  end

  class A
    def tt(aa)
      "A#tt"
    end

    def uu(a)
      class << self
        define_method(:tt) do |sym|
          super(sym)
        end
      end
    end
  end

  def test_define_method
    a = A.new
    a.uu(12)
    assert_equal("A#tt", a.tt(12), "[ruby-core:3856]")
    e = assert_raise(RuntimeError, "[ruby-core:24244]") {
      lambda {
        Class.new do
          define_method(:a) {super}.call
        end
      }.call
    }
    assert_match(/implicit argument passing of super from method defined by define_method/, e.message)
  end

  class SubSeq
    def initialize
      @first=11
      @first or fail
    end

    def subseq
      @first or fail
    end
  end

  class Indexed
    def subseq
      SubSeq.new
    end
  end

  Overlaid = proc do
    class << self
      def subseq
        super.instance_eval(& Overlaid)
      end
    end
  end

  def test_overlaid
    assert_nothing_raised('[ruby-dev:40959]') do
      overlaid = proc do |obj|
        def obj.reverse
          super
        end
      end
      overlaid.call(str = "123")
      overlaid.call(ary = [1,2,3])
      str.reverse
    end

    assert_nothing_raised('[ruby-core:27230]') do
      mid=Indexed.new
      mid.instance_eval(&Overlaid)
      mid.subseq
      mid.subseq
    end
  end
end
