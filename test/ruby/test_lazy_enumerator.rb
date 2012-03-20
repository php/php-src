require 'test/unit'

class TestLazyEnumerator < Test::Unit::TestCase
  class Step
    include Enumerable
    attr_reader :current, :args

    def initialize(enum)
      @enum = enum
      @current = nil
      @args = nil
    end

    def each(*args)
      @args = args
      @enum.each {|i| @current = i; yield i}
    end
  end

  def test_initialize
    assert_equal([1, 2, 3], [1, 2, 3].lazy.to_a)
    assert_equal([1, 2, 3], Enumerator::Lazy.new([1, 2, 3]).to_a)
  end

  def test_each_args
    a = Step.new(1..3)
    assert_equal(1, a.lazy.each(4).first)
    assert_equal([4], a.args)
  end

  def test_each_line
    name = lineno = nil
    File.open(__FILE__) do |f|
      f.each("").map do |paragraph|
        paragraph[/\A\s*(.*)/, 1]
      end.find do |line|
        if name = line[/^class\s+(\S+)/, 1]
          lineno = f.lineno
          true
        end
      end
    end
    assert_equal(self.class.name, name)
    assert_operator(lineno, :>, 2)

    name = lineno = nil
    File.open(__FILE__) do |f|
      f.lazy.each("").map do |paragraph|
        paragraph[/\A\s*(.*)/, 1]
      end.find do |line|
        if name = line[/^class\s+(\S+)/, 1]
          lineno = f.lineno
          true
        end
      end
    end
    assert_equal(self.class.name, name)
    assert_equal(2, lineno)
  end

  def test_select
    a = Step.new(1..6)
    assert_equal(4, a.select {|x| x > 3}.first)
    assert_equal(6, a.current)
    assert_equal(4, a.lazy.select {|x| x > 3}.first)
    assert_equal(4, a.current)

    a = Step.new(['word', nil, 1])
    assert_raise(TypeError) {a.select {|x| "x"+x}.first}
    assert_equal(nil, a.current)
    assert_equal("word", a.lazy.select {|x| "x"+x}.first)
    assert_equal("word", a.current)
  end

  def test_select_multiple_values
    e = Enumerator.new { |yielder|
      for i in 1..5
        yielder.yield(i, i.to_s)
      end
    }
    assert_equal([[2, "2"], [4, "4"]],
                 e.select {|x| x[0] % 2 == 0})
    assert_equal([[2, "2"], [4, "4"]],
                 e.lazy.select {|x| x[0] % 2 == 0}.force)
  end

  def test_map
    a = Step.new(1..3)
    assert_equal(2, a.map {|x| x * 2}.first)
    assert_equal(3, a.current)
    assert_equal(2, a.lazy.map {|x| x * 2}.first)
    assert_equal(1, a.current)
  end

  def test_flat_map
    a = Step.new(1..3)
    assert_equal(2, a.flat_map {|x| [x * 2]}.first)
    assert_equal(3, a.current)
    assert_equal(2, a.lazy.flat_map {|x| [x * 2]}.first)
    assert_equal(1, a.current)
  end

  def test_flat_map_nested
    a = Step.new(1..3)
    assert_equal([1, "a"],
                 a.flat_map {|x| ("a".."c").map {|y| [x, y]}}.first)
    assert_equal(3, a.current)
    assert_equal([1, "a"],
                 a.lazy.flat_map {|x| ("a".."c").lazy.map {|y| [x, y]}}.first)
    assert_equal(1, a.current)
  end

  def test_flat_map_to_ary
    to_ary = Class.new {
      def initialize(value)
        @value = value
      end

      def to_ary
        [:to_ary, @value]
      end
    }
    assert_equal([:to_ary, 1, :to_ary, 2, :to_ary, 3],
                 [1, 2, 3].flat_map {|x| to_ary.new(x)})
    assert_equal([:to_ary, 1, :to_ary, 2, :to_ary, 3],
                 [1, 2, 3].lazy.flat_map {|x| to_ary.new(x)}.force)
  end

  def test_flat_map_non_array
    assert_equal(["1", "2", "3"], [1, 2, 3].flat_map {|x| x.to_s})
    assert_equal(["1", "2", "3"], [1, 2, 3].lazy.flat_map {|x| x.to_s}.force)
  end

  def test_reject
    a = Step.new(1..6)
    assert_equal(4, a.reject {|x| x < 4}.first)
    assert_equal(6, a.current)
    assert_equal(4, a.lazy.reject {|x| x < 4}.first)
    assert_equal(4, a.current)

    a = Step.new(['word', nil, 1])
    assert_equal(nil, a.reject {|x| x}.first)
    assert_equal(1, a.current)
    assert_equal(nil, a.lazy.reject {|x| x}.first)
    assert_equal(nil, a.current)
  end

  def test_reject_multiple_values
    e = Enumerator.new { |yielder|
      for i in 1..5
        yielder.yield(i, i.to_s)
      end
    }
    assert_equal([[2, "2"], [4, "4"]],
                 e.reject {|x| x[0] % 2 != 0})
    assert_equal([[2, "2"], [4, "4"]],
                 e.lazy.reject {|x| x[0] % 2 != 0}.force)
  end

  def test_grep
    a = Step.new('a'..'f')
    assert_equal('c', a.grep(/c/).first)
    assert_equal('f', a.current)
    assert_equal('c', a.lazy.grep(/c/).first)
    assert_equal('c', a.current)
    assert_equal(%w[a e], a.grep(proc {|x| /[aeiou]/ =~ x}))
    assert_equal(%w[a e], a.lazy.grep(proc {|x| /[aeiou]/ =~ x}).to_a)
  end

  def test_grep_with_block
    a = Step.new('a'..'f')
    assert_equal('C', a.grep(/c/) {|i| i.upcase}.first)
    assert_equal('C', a.lazy.grep(/c/) {|i| i.upcase}.first)
  end

  def test_grep_multiple_values
    e = Enumerator.new { |yielder|
      3.times { |i|
        yielder.yield(i, i.to_s)
      }
    }
    assert_equal([[2, "2"]], e.grep(proc {|x| x == [2, "2"]}))
    assert_equal([[2, "2"]], e.lazy.grep(proc {|x| x == [2, "2"]}).force)
    assert_equal(["22"],
                 e.lazy.grep(proc {|x| x == [2, "2"]}, &:join).force)
  end

  def test_zip
    a = Step.new(1..3)
    assert_equal([1, "a"], a.zip("a".."c").first)
    assert_equal(3, a.current)
    assert_equal([1, "a"], a.lazy.zip("a".."c").first)
    assert_equal(1, a.current)
  end

  def test_zip_short_arg
    a = Step.new(1..5)
    assert_equal([5, nil], a.zip("a".."c").last)
    assert_equal([5, nil], a.lazy.zip("a".."c").force.last)
  end

  def test_zip_without_arg
    a = Step.new(1..3)
    assert_equal([1], a.zip.first)
    assert_equal(3, a.current)
    assert_equal([1], a.lazy.zip.first)
    assert_equal(1, a.current)
  end

  def test_zip_with_block
    # zip should be eager when a block is given
    a = Step.new(1..3)
    ary = []
    assert_equal(nil, a.lazy.zip("a".."c") {|x, y| ary << [x, y]})
    assert_equal(a.zip("a".."c"), ary)
    assert_equal(3, a.current)
  end

  def test_take
    a = Step.new(1..10)
    assert_equal(1, a.take(5).first)
    assert_equal(5, a.current)
    assert_equal(1, a.lazy.take(5).first)
    assert_equal(1, a.current)
    assert_equal((1..5).to_a, a.lazy.take(5).force)
    assert_equal(5, a.current)
    a = Step.new(1..10)
    assert_equal([], a.lazy.take(0).force)
    assert_equal(nil, a.current)
  end

  def test_take_while
    a = Step.new(1..10)
    assert_equal(1, a.take_while {|i| i < 5}.first)
    assert_equal(5, a.current)
    assert_equal(1, a.lazy.take_while {|i| i < 5}.first)
    assert_equal(1, a.current)
    assert_equal((1..4).to_a, a.lazy.take_while {|i| i < 5}.to_a)
  end

  def test_drop
    a = Step.new(1..10)
    assert_equal(6, a.drop(5).first)
    assert_equal(10, a.current)
    assert_equal(6, a.lazy.drop(5).first)
    assert_equal(6, a.current)
    assert_equal((6..10).to_a, a.lazy.drop(5).to_a)
  end

  def test_drop_while
    a = Step.new(1..10)
    assert_equal(5, a.drop_while {|i| i < 5}.first)
    assert_equal(10, a.current)
    assert_equal(5, a.lazy.drop_while {|i| i < 5}.first)
    assert_equal(5, a.current)
    assert_equal((5..10).to_a, a.lazy.drop_while {|i| i < 5}.to_a)
  end

  def test_drop_and_take
    assert_equal([4, 5], (1..Float::INFINITY).lazy.drop(3).take(2).to_a)
  end

  def test_cycle
    a = Step.new(1..3)
    assert_equal("1", a.cycle(2).map(&:to_s).first)
    assert_equal(3, a.current)
    assert_equal("1", a.lazy.cycle(2).map(&:to_s).first)
    assert_equal(1, a.current)
  end

  def test_cycle_with_block
    # cycle should be eager when a block is given
    a = Step.new(1..3)
    ary = []
    assert_equal(nil, a.lazy.cycle(2) {|i| ary << i})
    assert_equal(a.cycle(2).to_a, ary)
    assert_equal(3, a.current)
  end

  def test_force
    assert_equal([1, 2, 3], (1..Float::INFINITY).lazy.take(3).force)
  end
end
