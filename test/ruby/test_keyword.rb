require 'test/unit'

class TestKeywordArguments < Test::Unit::TestCase
  def f1(str: "foo", num: 424242)
    [str, num]
  end

  def test_f1
    assert_equal(["foo", 424242], f1)
    assert_equal(["bar", 424242], f1(str: "bar"))
    assert_equal(["foo", 111111], f1(num: 111111))
    assert_equal(["bar", 111111], f1(str: "bar", num: 111111))
    assert_raise(ArgumentError) { f1(str: "bar", check: true) }
    assert_raise(ArgumentError) { f1("string") }
  end


  def f2(x, str: "foo", num: 424242)
    [x, str, num]
  end

  def test_f2
    assert_equal([:xyz, "foo", 424242], f2(:xyz))
  end


  def f3(str: "foo", num: 424242, **h)
    [str, num, h]
  end

  def test_f3
    assert_equal(["foo", 424242, {}], f3)
    assert_equal(["bar", 424242, {}], f3(str: "bar"))
    assert_equal(["foo", 111111, {}], f3(num: 111111))
    assert_equal(["bar", 111111, {}], f3(str: "bar", num: 111111))
    assert_equal(["bar", 424242, {:check=>true}], f3(str: "bar", check: true))
    assert_raise(ArgumentError) { f3("string") }
  end


  define_method(:f4) {|str: "foo", num: 424242| [str, num] }

  def test_f4
    assert_equal(["foo", 424242], f4)
    assert_equal(["bar", 424242], f4(str: "bar"))
    assert_equal(["foo", 111111], f4(num: 111111))
    assert_equal(["bar", 111111], f4(str: "bar", num: 111111))
    assert_raise(ArgumentError) { f4(str: "bar", check: true) }
    assert_raise(ArgumentError) { f4("string") }
  end


  define_method(:f5) {|str: "foo", num: 424242, **h| [str, num, h] }

  def test_f5
    assert_equal(["foo", 424242, {}], f5)
    assert_equal(["bar", 424242, {}], f5(str: "bar"))
    assert_equal(["foo", 111111, {}], f5(num: 111111))
    assert_equal(["bar", 111111, {}], f5(str: "bar", num: 111111))
    assert_equal(["bar", 424242, {:check=>true}], f5(str: "bar", check: true))
    assert_raise(ArgumentError) { f5("string") }
  end


  def f6(str: "foo", num: 424242, **h, &blk)
    [str, num, h, blk]
  end

  def test_f6 # [ruby-core:40518]
    assert_equal(["foo", 424242, {}, nil], f6)
    assert_equal(["bar", 424242, {}, nil], f6(str: "bar"))
    assert_equal(["foo", 111111, {}, nil], f6(num: 111111))
    assert_equal(["bar", 111111, {}, nil], f6(str: "bar", num: 111111))
    assert_equal(["bar", 424242, {:check=>true}, nil], f6(str: "bar", check: true))
    a = f6 {|x| x + 42 }
    assert_equal(["foo", 424242, {}], a[0, 3])
    assert_equal(43, a.last.call(1))
  end

  def f7(*r, str: "foo", num: 424242, **h)
    [r, str, num, h]
  end

  def test_f7 # [ruby-core:41772]
    assert_equal([[], "foo", 424242, {}], f7)
    assert_equal([[], "bar", 424242, {}], f7(str: "bar"))
    assert_equal([[], "foo", 111111, {}], f7(num: 111111))
    assert_equal([[], "bar", 111111, {}], f7(str: "bar", num: 111111))
    assert_equal([[1], "foo", 424242, {}], f7(1))
    assert_equal([[1, 2], "foo", 424242, {}], f7(1, 2))
    assert_equal([[1, 2, 3], "foo", 424242, {}], f7(1, 2, 3))
    assert_equal([[1], "bar", 424242, {}], f7(1, str: "bar"))
    assert_equal([[1, 2], "bar", 424242, {}], f7(1, 2, str: "bar"))
    assert_equal([[1, 2, 3], "bar", 424242, {}], f7(1, 2, 3, str: "bar"))
  end

  def test_method_parameters
    assert_equal([[:key, :str], [:key, :num]], method(:f1).parameters);
    assert_equal([[:req, :x], [:key, :str], [:key, :num]], method(:f2).parameters);
    assert_equal([[:key, :str], [:key, :num], [:keyrest, :h]], method(:f3).parameters);
    assert_equal([[:key, :str], [:key, :num]], method(:f4).parameters);
    assert_equal([[:key, :str], [:key, :num], [:keyrest, :h]], method(:f5).parameters);
    assert_equal([[:key, :str], [:key, :num], [:keyrest, :h], [:block, :blk]], method(:f6).parameters);
    assert_equal([[:rest, :r], [:key, :str], [:key, :num], [:keyrest, :h]], method(:f7).parameters);
  end

  def test_lambda
    f = ->(str: "foo", num: 424242) { [str, num] }
    assert_equal(["foo", 424242], f[])
    assert_equal(["bar", 424242], f[str: "bar"])
    assert_equal(["foo", 111111], f[num: 111111])
    assert_equal(["bar", 111111], f[str: "bar", num: 111111])
  end
end
