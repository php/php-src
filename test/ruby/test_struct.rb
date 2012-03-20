require 'test/unit'
require 'timeout'
require_relative 'envutil'

class TestStruct < Test::Unit::TestCase
  def test_struct
    struct_test = Struct.new("Test", :foo, :bar)
    assert_equal(Struct::Test, struct_test)

    test = struct_test.new(1, 2)
    assert_equal(1, test.foo)
    assert_equal(2, test.bar)
    assert_equal(1, test[0])
    assert_equal(2, test[1])

    a, b = test.to_a
    assert_equal(1, a)
    assert_equal(2, b)

    test[0] = 22
    assert_equal(22, test.foo)

    test.bar = 47
    assert_equal(47, test.bar)
  end

  # [ruby-dev:26247] more than 10 struct members causes segmentation fault
  def test_morethan10members
    list = %w( a b c d  e f g h  i j k l  m n o p )
    until list.empty?
      c = Struct.new(* list.map {|ch| ch.intern }).new
      list.each do |ch|
        c.__send__(ch)
      end
      list.pop
    end
  end

  def test_small_structs
    names = [:a, :b, :c, :d]
    1.upto(4) {|n|
      fields = names[0, n]
      klass = Struct.new(*fields)
      o = klass.new(*(0...n).to_a)
      fields.each_with_index {|name, i|
        assert_equal(i, o[name])
      }
      o = klass.new(*(0...n).to_a.reverse)
      fields.each_with_index {|name, i|
        assert_equal(n-i-1, o[name])
      }
    }
  end

  def test_inherit
    klass = Struct.new(:a)
    klass2 = Class.new(klass)
    o = klass2.new(1)
    assert_equal(1, o.a)
  end

  def test_members
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal([:a], klass.members)
    assert_equal([:a], o.members)
  end

  def test_ref
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal(1, o[:a])
    assert_raise(NameError) { o[:b] }
  end

  def test_modify
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_raise(SecurityError) do
      Thread.new do
        $SAFE = 4
        o.a = 2
      end.value
    end
  end

  def test_set
    klass = Struct.new(:a)
    o = klass.new(1)
    o[:a] = 2
    assert_equal(2, o[:a])
    assert_raise(NameError) { o[:b] = 3 }
  end

  def test_struct_new
    assert_raise(NameError) { Struct.new("foo") }
    assert_nothing_raised { Struct.new("Foo") }
    Struct.instance_eval { remove_const(:Foo) }
    assert_nothing_raised { Struct.new(:a) { } }
    assert_raise(RuntimeError) { Struct.new(:a) { raise } }

    assert_equal([:utime, :stime, :cutime, :cstime], Process.times.members)
  end

  def test_initialize
    klass = Struct.new(:a)
    assert_raise(ArgumentError) { klass.new(1, 2) }
  end

  def test_each
    klass = Struct.new(:a, :b)
    o = klass.new(1, 2)
    assert_equal([1, 2], o.each.to_a)
  end

  def test_each_pair
    klass = Struct.new(:a, :b)
    o = klass.new(1, 2)
    assert_equal([[:a, 1], [:b, 2]], o.each_pair.to_a)
  end

  def test_inspect
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal("#<struct a=1>", o.inspect)
    o.a = o
    assert_match(/^#<struct a=#<struct #<.*?>:...>>$/, o.inspect)

    Struct.new("Foo", :a)
    o = Struct::Foo.new(1)
    assert_equal("#<struct Struct::Foo a=1>", o.inspect)
    Struct.instance_eval { remove_const(:Foo) }

    klass = Struct.new(:a, :b)
    o = klass.new(1, 2)
    assert_equal("#<struct a=1, b=2>", o.inspect)

    klass = Struct.new(:@a)
    o = klass.new(1)
    assert_equal("#<struct :@a=1>", o.inspect)
  end

  def test_init_copy
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal(o, o.dup)
  end

  def test_aref
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal(1, o[0])
    assert_raise(IndexError) { o[-2] }
    assert_raise(IndexError) { o[1] }
  end

  def test_aset
    klass = Struct.new(:a)
    o = klass.new(1)
    o[0] = 2
    assert_equal(2, o[:a])
    assert_raise(IndexError) { o[-2] = 3 }
    assert_raise(IndexError) { o[1] = 3 }
  end

  def test_values_at
    klass = Struct.new(:a, :b, :c, :d, :e, :f)
    o = klass.new(1, 2, 3, 4, 5, 6)
    assert_equal([2, 4, 6], o.values_at(1, 3, 5))
    assert_equal([2, 3, 4, 3, 4, 5], o.values_at(1..3, 2...5))
  end

  def test_select
    klass = Struct.new(:a, :b, :c, :d, :e, :f)
    o = klass.new(1, 2, 3, 4, 5, 6)
    assert_equal([1, 3, 5], o.select {|v| v % 2 != 0 })
    assert_raise(ArgumentError) { o.select(1) }
  end

  def test_equal
    klass1 = Struct.new(:a)
    klass2 = Struct.new(:a, :b)
    o1 = klass1.new(1)
    o2 = klass1.new(1)
    o3 = klass2.new(1)
    assert(o1.==(o2))
    assert(o1 != o3)
  end

  def test_hash
    klass = Struct.new(:a)
    o = klass.new(1)
    assert(o.hash.is_a?(Fixnum))
  end

  def test_eql
    klass1 = Struct.new(:a)
    klass2 = Struct.new(:a, :b)
    o1 = klass1.new(1)
    o2 = klass1.new(1)
    o3 = klass2.new(1)
    assert(o1.eql?(o2))
    assert(!(o1.eql?(o3)))
  end

  def test_size
    klass = Struct.new(:a)
    o = klass.new(1)
    assert_equal(1, o.size)
  end

  def test_error
    assert_raise(TypeError){
      Struct.new(0)
    }
  end

  def test_nonascii
    struct_test = Struct.new("R\u{e9}sum\u{e9}", :"r\u{e9}sum\u{e9}")
    assert_equal(Struct.const_get("R\u{e9}sum\u{e9}"), struct_test, '[ruby-core:24849]')
    a = struct_test.new(42)
    assert_equal("#<struct Struct::R\u{e9}sum\u{e9} r\u{e9}sum\u{e9}=42>", a.inspect, '[ruby-core:24849]')
  end

  def test_comparison_when_recursive
    klass1 = Struct.new(:a, :b, :c)

    x = klass1.new(1, 2, nil); x.c = x
    y = klass1.new(1, 2, nil); y.c = y
    Timeout.timeout(1) {
      assert x == y
      assert x.eql? y
    }

    z = klass1.new(:something, :other, nil); z.c = z
    Timeout.timeout(1) {
      assert x != z
      assert !x.eql?(z)
    }

    x.c = y; y.c = x
    Timeout.timeout(1) {
      assert x == y
      assert x.eql?(y)
    }

    x.c = z; z.c = x
    Timeout.timeout(1) {
      assert x != z
      assert !x.eql?(z)
    }
  end

  def test_struct_subclass
    bug5036 = '[ruby-dev:44122]'
    st = Class.new(Struct)
    s = st.new("S", :m).new
    error = assert_raise(SecurityError) do
      proc do
        $SAFE = 4
        s.m = 1
      end.call
    end
    assert_equal("Insecure: can't modify #{st}::S", error.message, bug5036)
  end
end
