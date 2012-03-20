# coding: utf-8
module MarshalTestLib
  # include this module to a Test::Unit::TestCase and definde encode(o) and
  # decode(s) methods.  e.g.
  #
  # def encode(o)
  #   SOAPMarshal.dump(o)
  # end
  #
  # def decode(s)
  #   SOAPMarshal.load(s)
  # end

  NegativeZero = (-1.0 / (1.0 / 0.0))

  module Mod1; end
  module Mod2; end

  def marshaltest(o1)
    str = encode(o1)
    print str.dump, "\n" if $DEBUG
    o2 = decode(str)
    o2
  end

  def marshal_equal(o1, msg = nil)
    msg = msg ? msg + "(#{ caller[0] })" : caller[0]
    o2 = marshaltest(o1)
    assert_equal(o1.class, o2.class, msg)
    iv1 = o1.instance_variables.sort
    iv2 = o2.instance_variables.sort
    assert_equal(iv1, iv2)
    val1 = iv1.map {|var| o1.instance_eval {eval var.to_s}}
    val2 = iv1.map {|var| o2.instance_eval {eval var.to_s}}
    assert_equal(val1, val2, msg)
    if block_given?
      assert_equal(yield(o1), yield(o2), msg)
    else
      assert_equal(o1, o2, msg)
    end
  end

  class MyObject; def initialize(v) @v = v end; attr_reader :v; end
  def test_object
    o1 = Object.new
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_object_subclass
    marshal_equal(MyObject.new(2)) {|o| o.v}
  end

  def test_object_extend
    o1 = Object.new
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  def test_object_subclass_extend
    o1 = MyObject.new(2)
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  class MyArray < Array
    def initialize(v, *args)
      super(args)
      @v = v
    end
  end
  def test_array
    marshal_equal(5)
    marshal_equal([1,2,3])
  end

  def test_array_subclass
    marshal_equal(MyArray.new(0, 1, 2, 3))
  end

  def test_array_ivar
    o1 = Array.new
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  class MyException < Exception; def initialize(v, *args) super(*args); @v = v; end; attr_reader :v; end
  def test_exception
    marshal_equal(Exception.new('foo')) {|o| o.message}
    marshal_equal(assert_raise(NoMethodError) {no_such_method()}) {|o| o.message}
  end

  def test_exception_subclass
    marshal_equal(MyException.new(20, "bar")) {|o| [o.message, o.v]}
  end

  def test_false
    marshal_equal(false)
  end

  class MyHash < Hash; def initialize(v, *args) super(*args); @v = v; end end
  def test_hash
    marshal_equal({1=>2, 3=>4})
  end

  def test_hash_default
    h = Hash.new(:default)
    h[5] = 6
    marshal_equal(h)
  end

  def test_hash_subclass
    h = MyHash.new(7, 8)
    h[4] = 5
    marshal_equal(h)
  end

  def test_hash_default_proc
    h = Hash.new {}
    assert_raise(TypeError) { marshaltest(h) }
  end

  def test_hash_ivar
    o1 = Hash.new
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_hash_extend
    o1 = Hash.new
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  def test_hash_subclass_extend
    o1 = MyHash.new(2)
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  def test_bignum
    marshal_equal(-0x4000_0000_0000_0001)
    marshal_equal(-0x4000_0001)
    marshal_equal(0x4000_0000)
    marshal_equal(0x4000_0000_0000_0000)
  end

  def test_fixnum
    marshal_equal(-0x4000_0000)
    marshal_equal(-0x3fff_ffff)
    marshal_equal(-1)
    marshal_equal(0)
    marshal_equal(1)
    marshal_equal(0x3fff_ffff)
  end

  def test_fixnum_ivar
    o1 = 1
    o1.instance_eval { @iv = 2 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  ensure
    1.instance_eval { remove_instance_variable("@iv") }
  end

  def test_fixnum_ivar_self
    o1 = 1
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  ensure
    1.instance_eval { remove_instance_variable("@iv") }
  end

  def test_float
    marshal_equal(-1.0)
    marshal_equal(0.0)
    marshal_equal(1.0)
  end

  def test_float_inf_nan
    marshal_equal(1.0/0.0)
    marshal_equal(-1.0/0.0)
    marshal_equal(0.0/0.0) {|o| o.nan?}
    marshal_equal(NegativeZero) {|o| 1.0/o}
  end

  def test_float_ivar
    o1 = 1.23
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_float_ivar_self
    o1 = 5.5
    o1.instance_eval { @iv = o1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_float_extend
    o1 = 0.0/0.0
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  class MyRange < Range; def initialize(v, *args) super(*args); @v = v; end end
  def test_range
    marshal_equal(1..2)
    marshal_equal(1...3)
  end

  def test_range_subclass
    marshal_equal(MyRange.new(4,5,8, false))
  end

  class MyRegexp < Regexp; def initialize(v, *args) super(*args); @v = v; end end
  def test_regexp
    marshal_equal(/a/)
    marshal_equal(/A/i)
    marshal_equal(/A/mx)
    marshal_equal(/a\u3042/)
    marshal_equal(/aあ/)
    assert_equal(Regexp.new("あ".force_encoding("ASCII-8BIT")),
                 Marshal.load("\004\b/\b\343\201\202\000"))
    assert_equal(/au3042/, Marshal.load("\004\b/\fa\\u3042\000"))
    #assert_equal(/au3042/u, Marshal.load("\004\b/\fa\\u3042@")) # spec
  end

  def test_regexp_subclass
    marshal_equal(MyRegexp.new(10, "a"))
  end

  class MyString < String; def initialize(v, *args) super(*args); @v = v; end end
  def test_string
    marshal_equal("abc")
  end

  def test_string_ivar
    o1 = ""
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_string_subclass
    marshal_equal(MyString.new(10, "a"))
  end

  def test_string_subclass_cycle
    str = MyString.new(10, "b")
    str.instance_eval { @v = str }
    marshal_equal(str) { |o|
      assert_equal(o.__id__, o.instance_eval { @v }.__id__)
      o.instance_eval { @v }
    }
  end

  def test_string_subclass_extend
    o = "abc"
    o.extend(Mod1)
    str = MyString.new(o, "c")
    marshal_equal(str) { |v|
      assert(v.instance_eval { @v }.kind_of?(Mod1))
    }
  end

  MyStruct = Struct.new("MyStruct", :a, :b)
  if RUBY_VERSION < "1.8.0"
    # Struct#== is not defined in ruby/1.6
    class MyStruct
      def ==(rhs)
	return true if __id__ == rhs.__id__
	return false unless rhs.is_a?(::Struct)
	return false if self.class != rhs.class
	members.each do |member|
	  return false if self.__send__(member) != rhs.__send__(member)
	end
	return true
      end
    end
  end
  class MySubStruct < MyStruct; def initialize(v, *args) super(*args); @v = v; end end
  def test_struct
    marshal_equal(MyStruct.new(1,2))
  end

  def test_struct_subclass
    if RUBY_VERSION < "1.8.0"
      # Substruct instance cannot be dumped in ruby/1.6
      # ::Marshal.dump(MySubStruct.new(10, 1, 2)) #=> uninitialized struct
      return false
    end
    marshal_equal(MySubStruct.new(10,1,2))
  end

  def test_struct_ivar
    o1 = MyStruct.new
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_struct_subclass_extend
    o1 = MyStruct.new
    o1.extend(Mod1)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
    o1.extend(Mod2)
    marshal_equal(o1) { |o|
      (class << self; self; end).ancestors
    }
  end

  def test_symbol
    marshal_equal(:a)
    marshal_equal(:a?)
    marshal_equal(:a!)
    marshal_equal(:a=)
    marshal_equal(:|)
    marshal_equal(:^)
    marshal_equal(:&)
    marshal_equal(:<=>)
    marshal_equal(:==)
    marshal_equal(:===)
    marshal_equal(:=~)
    marshal_equal(:>)
    marshal_equal(:>=)
    marshal_equal(:<)
    marshal_equal(:<=)
    marshal_equal(:<<)
    marshal_equal(:>>)
    marshal_equal(:+)
    marshal_equal(:-)
    marshal_equal(:*)
    marshal_equal(:/)
    marshal_equal(:%)
    marshal_equal(:**)
    marshal_equal(:~)
    marshal_equal(:+@)
    marshal_equal(:-@)
    marshal_equal(:[])
    marshal_equal(:[]=)
    marshal_equal(:`)   #`
    marshal_equal("a b".intern)
  end

  class MyTime < Time; def initialize(v, *args) super(*args); @v = v; end end
  def test_time
    # once there was a bug caused by usec overflow.  try a little harder.
    10.times do
      t = Time.now
      marshal_equal(t, t.usec.to_s)
    end
  end

  def test_time_subclass
    marshal_equal(MyTime.new(10))
  end

  def test_time_ivar
    o1 = Time.now
    o1.instance_eval { @iv = 1 }
    marshal_equal(o1) {|o| o.instance_eval { @iv }}
  end

  def test_time_in_array
    t = Time.now
    assert_equal([t,t], Marshal.load(Marshal.dump([t, t])), "[ruby-dev:34159]")
  end

  def test_true
    marshal_equal(true)
  end

  def test_nil
    marshal_equal(nil)
  end

  def test_share
    o = [:share]
    o1 = [o, o]
    o2 = marshaltest(o1)
    assert_same(o2.first, o2.last)
  end

  class CyclicRange < Range
    def <=>(other); true; end
  end
  def test_range_cyclic
    return unless CyclicRange.respond_to?(:allocate) # test for 1.8
    o1 = CyclicRange.allocate
    o1.instance_eval { initialize(o1, o1) }
    o2 = marshaltest(o1)
    assert_same(o2, o2.begin)
    assert_same(o2, o2.end)
  end

  def test_singleton
    o = Object.new
    def o.m() end
    assert_raise(TypeError) { marshaltest(o) }
    o = Object.new
    c = class << o
      @v = 1
      class C; self; end
    end
    assert_raise(TypeError) { marshaltest(o) }
    assert_raise(TypeError) { marshaltest(c) }
    assert_raise(TypeError) { marshaltest(ARGF) }
    assert_raise(TypeError) { marshaltest(ENV) }
  end

  def test_extend
    o = Object.new
    o.extend Mod1
    marshal_equal(o) { |obj| obj.kind_of? Mod1 }
    o = Object.new
    o.extend Mod1
    o.extend Mod2
    marshal_equal(o) {|obj| class << obj; ancestors end}
    o = Object.new
    o.extend Module.new
    assert_raise(TypeError) { marshaltest(o) }
  end

  def test_extend_string
    o = ""
    o.extend Mod1
    marshal_equal(o) { |obj| obj.kind_of? Mod1 }
    o = ""
    o.extend Mod1
    o.extend Mod2
    marshal_equal(o) {|obj| class << obj; ancestors end}
    o = ""
    o.extend Module.new
    assert_raise(TypeError) { marshaltest(o) }
  end

  def test_anonymous
    c = Class.new
    assert_raise(TypeError) { marshaltest(c) }
    o = c.new
    assert_raise(TypeError) { marshaltest(o) }
    m = Module.new
    assert_raise(TypeError) { marshaltest(m) }
  end

  def test_string_empty
    marshal_equal("")
  end

  def test_string_crlf
    marshal_equal("\r\n")
  end

  def test_string_escape
    marshal_equal("\0<;;>\1;;")
  end

  MyStruct2 = Struct.new(:a, :b)
  if RUBY_VERSION < "1.8.0"
    # Struct#== is not defined in ruby/1.6
    class MyStruct2
      def ==(rhs)
	return true if __id__ == rhs.__id__
	return false unless rhs.is_a?(::Struct)
	return false if self.class != rhs.class
	members.each do |member|
	  return false if self.__send__(member) != rhs.__send__(member)
	end
	return true
      end
    end
  end
  def test_struct_toplevel
    o = MyStruct2.new(1,2)
    marshal_equal(o)
  end
end
