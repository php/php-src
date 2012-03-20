require 'test/unit'
require 'continuation'
require 'stringio'

class TestEnumerable < Test::Unit::TestCase
  def setup
    @obj = Object.new
    class << @obj
      include Enumerable
      def each
        yield 1
        yield 2
        yield 3
        yield 1
        yield 2
        self
      end
    end
    @empty = Object.new
    class << @empty
      attr_reader :block
      include Enumerable
      def each(&block)
        @block = block
        self
      end
    end
    @verbose = $VERBOSE
    $VERBOSE = nil
  end

  def teardown
    $VERBOSE = @verbose
  end

  def assert_not_warn
    begin
      org_stderr = $stderr
      v = $VERBOSE
      $stderr = StringIO.new(warn = '')
      $VERBOSE = true
      yield
    ensure
      $stderr = org_stderr
      $VERBOSE = v
    end
    assert_equal("", warn)
  end

  def test_grep
    assert_equal([1, 2, 1, 2], @obj.grep(1..2))
    a = []
    @obj.grep(2) {|x| a << x }
    assert_equal([2, 2], a)

    bug5801 = '[ruby-dev:45041]'
    @empty.grep(//)
    assert_nothing_raised(bug5801) {100.times {@empty.block.call}}
  end

  def test_count
    assert_equal(5, @obj.count)
    assert_equal(2, @obj.count(1))
    assert_equal(3, @obj.count {|x| x % 2 == 1 })
    assert_equal(2, @obj.count(1) {|x| x % 2 == 1 })
    assert_raise(ArgumentError) { @obj.count(0, 1) }

    if RUBY_ENGINE == "ruby"
      en = Class.new {
        include Enumerable
        alias :size :count
        def each
          yield 1
        end
      }
      assert_equal(1, en.new.count, '[ruby-core:24794]')
    end
  end

  def test_find
    assert_equal(2, @obj.find {|x| x % 2 == 0 })
    assert_equal(nil, @obj.find {|x| false })
    assert_equal(:foo, @obj.find(proc { :foo }) {|x| false })
  end

  def test_find_index
    assert_equal(1, @obj.find_index(2))
    assert_equal(1, @obj.find_index {|x| x % 2 == 0 })
    assert_equal(nil, @obj.find_index {|x| false })
    assert_raise(ArgumentError) { @obj.find_index(0, 1) }
    assert_equal(1, @obj.find_index(2) {|x| x == 1 })
  end

  def test_find_all
    assert_equal([1, 3, 1], @obj.find_all {|x| x % 2 == 1 })
  end

  def test_reject
    assert_equal([2, 3, 2], @obj.reject {|x| x < 2 })
  end

  def test_to_a
    assert_equal([1, 2, 3, 1, 2], @obj.to_a)
  end

  def test_inject
    assert_equal(12, @obj.inject {|z, x| z * x })
    assert_equal(48, @obj.inject {|z, x| z * 2 + x })
    assert_equal(12, @obj.inject(:*))
    assert_equal(24, @obj.inject(2) {|z, x| z * x })
    assert_equal(24, @obj.inject(2, :*) {|z, x| z * x })
  end

  def test_partition
    assert_equal([[1, 3, 1], [2, 2]], @obj.partition {|x| x % 2 == 1 })
  end

  def test_group_by
    h = { 1 => [1, 1], 2 => [2, 2], 3 => [3] }
    assert_equal(h, @obj.group_by {|x| x })
  end

  def test_first
    assert_equal(1, @obj.first)
    assert_equal([1, 2, 3], @obj.first(3))
    assert_nil(@empty.first)
  end

  def test_sort
    assert_equal([1, 1, 2, 2, 3], @obj.sort)
  end

  def test_sort_by
    assert_equal([3, 2, 2, 1, 1], @obj.sort_by {|x| -x })
    assert_equal((1..300).to_a.reverse, (1..300).sort_by {|x| -x })
  end

  def test_all
    assert_equal(true, @obj.all? {|x| x <= 3 })
    assert_equal(false, @obj.all? {|x| x < 3 })
    assert_equal(true, @obj.all?)
    assert_equal(false, [true, true, false].all?)
  end

  def test_any
    assert_equal(true, @obj.any? {|x| x >= 3 })
    assert_equal(false, @obj.any? {|x| x > 3 })
    assert_equal(true, @obj.any?)
    assert_equal(false, [false, false, false].any?)
  end

  def test_one
    assert(@obj.one? {|x| x == 3 })
    assert(!(@obj.one? {|x| x == 1 }))
    assert(!(@obj.one? {|x| x == 4 }))
    assert(%w{ant bear cat}.one? {|word| word.length == 4})
    assert(!(%w{ant bear cat}.one? {|word| word.length > 4}))
    assert(!(%w{ant bear cat}.one? {|word| word.length < 4}))
    assert(!([ nil, true, 99 ].one?))
    assert([ nil, true, false ].one?)
  end

  def test_none
    assert(@obj.none? {|x| x == 4 })
    assert(!(@obj.none? {|x| x == 1 }))
    assert(!(@obj.none? {|x| x == 3 }))
    assert(%w{ant bear cat}.none? {|word| word.length == 5})
    assert(!(%w{ant bear cat}.none? {|word| word.length >= 4}))
    assert([].none?)
    assert([nil].none?)
    assert([nil,false].none?)
  end

  def test_min
    assert_equal(1, @obj.min)
    assert_equal(3, @obj.min {|a,b| b <=> a })
    ary = %w(albatross dog horse)
    assert_equal("albatross", ary.min)
    assert_equal("dog", ary.min {|a,b| a.length <=> b.length })
    assert_equal(1, [3,2,1].min)
  end

  def test_max
    assert_equal(3, @obj.max)
    assert_equal(1, @obj.max {|a,b| b <=> a })
    ary = %w(albatross dog horse)
    assert_equal("horse", ary.max)
    assert_equal("albatross", ary.max {|a,b| a.length <=> b.length })
    assert_equal(1, [3,2,1].max{|a,b| b <=> a })
  end

  def test_minmax
    assert_equal([1, 3], @obj.minmax)
    assert_equal([3, 1], @obj.minmax {|a,b| b <=> a })
    ary = %w(albatross dog horse)
    assert_equal(["albatross", "horse"], ary.minmax)
    assert_equal(["dog", "albatross"], ary.minmax {|a,b| a.length <=> b.length })
    assert_equal([1, 3], [2,3,1].minmax)
    assert_equal([3, 1], [2,3,1].minmax {|a,b| b <=> a })
    assert_equal([1, 3], [2,2,3,3,1,1].minmax)
  end

  def test_min_by
    assert_equal(3, @obj.min_by {|x| -x })
    a = %w(albatross dog horse)
    assert_equal("dog", a.min_by {|x| x.length })
    assert_equal(3, [2,3,1].min_by {|x| -x })
  end

  def test_max_by
    assert_equal(1, @obj.max_by {|x| -x })
    a = %w(albatross dog horse)
    assert_equal("albatross", a.max_by {|x| x.length })
    assert_equal(1, [2,3,1].max_by {|x| -x })
  end

  def test_minmax_by
    assert_equal([3, 1], @obj.minmax_by {|x| -x })
    a = %w(albatross dog horse)
    assert_equal(["dog", "albatross"], a.minmax_by {|x| x.length })
    assert_equal([3, 1], [2,3,1].minmax_by {|x| -x })
  end

  def test_member
    assert(@obj.member?(1))
    assert(!(@obj.member?(4)))
    assert([1,2,3].member?(1))
    assert(!([1,2,3].member?(4)))
  end

  class Foo
    include Enumerable
    def each
      yield 1
      yield 1,2
    end
  end

  def test_each_with_index
    a = []
    @obj.each_with_index {|x, i| a << [x, i] }
    assert_equal([[1,0],[2,1],[3,2],[1,3],[2,4]], a)

    hash = Hash.new
    %w(cat dog wombat).each_with_index do |item, index|
      hash[item] = index
    end
    assert_equal({"cat"=>0, "wombat"=>2, "dog"=>1}, hash)
    assert_equal([[1, 0], [[1, 2], 1]], Foo.new.each_with_index.to_a)
  end

  def test_each_with_object
    obj = [0, 1]
    ret = (1..10).each_with_object(obj) {|i, memo|
      memo[0] += i
      memo[1] *= i
    }
    assert_same(obj, ret)
    assert_equal([55, 3628800], ret)
    assert_equal([[1, nil], [[1, 2], nil]], Foo.new.each_with_object(nil).to_a)
  end

  def test_each_entry
    assert_equal([1, 2, 3], [1, 2, 3].each_entry.to_a)
    assert_equal([1, [1, 2]], Foo.new.each_entry.to_a)
  end

  def test_zip
    assert_equal([[1,1],[2,2],[3,3],[1,1],[2,2]], @obj.zip(@obj))
    a = []
    @obj.zip([:a, :b, :c]) {|x,y| a << [x, y] }
    assert_equal([[1,:a],[2,:b],[3,:c],[1,nil],[2,nil]], a)

    a = []
    @obj.zip({a: "A", b: "B", c: "C"}) {|x,y| a << [x, y] }
    assert_equal([[1,[:a,"A"]],[2,[:b,"B"]],[3,[:c,"C"]],[1,nil],[2,nil]], a)

    ary = Object.new
    def ary.to_a;   [1, 2]; end
    assert_raise(NoMethodError){ %w(a b).zip(ary) }
    def ary.each; [3, 4].each{|e|yield e}; end
    assert_equal([[1, 3], [2, 4], [3, nil], [1, nil], [2, nil]], @obj.zip(ary))
    def ary.to_ary; [5, 6]; end
    assert_equal([[1, 5], [2, 6], [3, nil], [1, nil], [2, nil]], @obj.zip(ary))
  end

  def test_take
    assert_equal([1,2,3], @obj.take(3))
  end

  def test_take_while
    assert_equal([1,2], @obj.take_while {|x| x <= 2})

    bug5801 = '[ruby-dev:45040]'
    @empty.take_while {true}
    assert_nothing_raised(bug5801) {100.times {@empty.block.call}}
  end

  def test_drop
    assert_equal([3,1,2], @obj.drop(2))
  end

  def test_drop_while
    assert_equal([3,1,2], @obj.drop_while {|x| x <= 2})
  end

  def test_cycle
    assert_equal([1,2,3,1,2,1,2,3,1,2], @obj.cycle.take(10))
  end

  def test_callcc
    assert_raise(RuntimeError) do
      c = nil
      @obj.sort_by {|x| callcc {|c2| c ||= c2 }; x }
      c.call
    end

    assert_raise(RuntimeError) do
      c = nil
      o = Object.new
      class << o; self; end.class_eval do
        define_method(:<=>) do |x|
          callcc {|c2| c ||= c2 }
          0
        end
      end
      [o, o].sort_by {|x| x }
      c.call
    end

    assert_raise(RuntimeError) do
      c = nil
      o = Object.new
      class << o; self; end.class_eval do
        define_method(:<=>) do |x|
          callcc {|c2| c ||= c2 }
          0
        end
      end
      [o, o, o].sort_by {|x| x }
      c.call
    end
  end

  def test_reverse_each
    assert_equal([2,1,3,2,1], @obj.reverse_each.to_a)
  end

  def test_chunk
    e = [].chunk {|elt| true }
    assert_equal([], e.to_a)

    e = @obj.chunk {|elt| elt & 2 == 0 ? false : true }
    assert_equal([[false, [1]], [true, [2, 3]], [false, [1]], [true, [2]]], e.to_a)

    e = @obj.chunk(acc: 0) {|elt, h| h[:acc] += elt; h[:acc].even? }
    assert_equal([[false, [1,2]], [true, [3]], [false, [1,2]]], e.to_a)
    assert_equal([[false, [1,2]], [true, [3]], [false, [1,2]]], e.to_a) # this tests h is duplicated.

    hs = [{}]
    e = [:foo].chunk(hs[0]) {|elt, h|
      hs << h
      true
    }
    assert_equal([[true, [:foo]]], e.to_a)
    assert_equal([[true, [:foo]]], e.to_a)
    assert_equal([{}, {}, {}], hs)
    assert_not_same(hs[0], hs[1])
    assert_not_same(hs[0], hs[2])
    assert_not_same(hs[1], hs[2])

    e = @obj.chunk {|elt| elt < 3 ? :_alone : true }
    assert_equal([[:_alone, [1]],
                  [:_alone, [2]],
                  [true, [3]],
                  [:_alone, [1]],
                  [:_alone, [2]]], e.to_a)

    e = @obj.chunk {|elt| elt == 3 ? :_separator : true }
    assert_equal([[true, [1, 2]],
                  [true, [1, 2]]], e.to_a)

    e = @obj.chunk {|elt| elt == 3 ? nil : true }
    assert_equal([[true, [1, 2]],
                  [true, [1, 2]]], e.to_a)

    e = @obj.chunk {|elt| :_foo }
    assert_raise(RuntimeError) { e.to_a }
  end

  def test_slice_before
    e = [].slice_before {|elt| true }
    assert_equal([], e.to_a)

    e = @obj.slice_before {|elt| elt.even? }
    assert_equal([[1], [2,3,1], [2]], e.to_a)

    e = @obj.slice_before {|elt| elt.odd? }
    assert_equal([[1,2], [3], [1,2]], e.to_a)

    e = @obj.slice_before(acc: 0) {|elt, h| h[:acc] += elt; h[:acc].even? }
    assert_equal([[1,2], [3,1,2]], e.to_a)
    assert_equal([[1,2], [3,1,2]], e.to_a) # this tests h is duplicated.

    hs = [{}]
    e = [:foo].slice_before(hs[0]) {|elt, h|
      hs << h
      true
    }
    assert_equal([[:foo]], e.to_a)
    assert_equal([[:foo]], e.to_a)
    assert_equal([{}, {}, {}], hs)
    assert_not_same(hs[0], hs[1])
    assert_not_same(hs[0], hs[2])
    assert_not_same(hs[1], hs[2])

    ss = %w[abc defg h ijk l mno pqr st u vw xy z]
    assert_equal([%w[abc defg h], %w[ijk l], %w[mno], %w[pqr st u vw xy z]],
                 ss.slice_before(/\A...\z/).to_a)
    assert_not_warn{ss.slice_before(/\A...\z/).to_a}
  end

end
