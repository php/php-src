require 'pp'
require 'delegate'
require 'test/unit'

module PPTestModule

class PPTest < Test::Unit::TestCase
  def test_list0123_12
    assert_equal("[0, 1, 2, 3]\n", PP.pp([0,1,2,3], '', 12))
  end

  def test_list0123_11
    assert_equal("[0,\n 1,\n 2,\n 3]\n", PP.pp([0,1,2,3], '', 11))
  end

  OverriddenStruct = Struct.new("OverriddenStruct", :members, :class)
  def test_struct_override_members # [ruby-core:7865]
    a = OverriddenStruct.new(1,2)
    assert_equal("#<struct Struct::OverriddenStruct members=1, class=2>\n", PP.pp(a, ''))
  end

  def test_redefined_method
    o = ""
    def o.method
    end
    assert_equal(%(""\n), PP.pp(o, ""))
  end
end

class HasInspect
  def initialize(a)
    @a = a
  end

  def inspect
    return "<inspect:#{@a.inspect}>"
  end
end

class HasPrettyPrint
  def initialize(a)
    @a = a
  end

  def pretty_print(q)
    q.text "<pretty_print:"
    q.pp @a
    q.text ">"
  end
end

class HasBoth
  def initialize(a)
    @a = a
  end

  def inspect
    return "<inspect:#{@a.inspect}>"
  end

  def pretty_print(q)
    q.text "<pretty_print:"
    q.pp @a
    q.text ">"
  end
end

class PrettyPrintInspect < HasPrettyPrint
  alias inspect pretty_print_inspect
end

class PrettyPrintInspectWithoutPrettyPrint
  alias inspect pretty_print_inspect
end

class PPInspectTest < Test::Unit::TestCase
  def test_hasinspect
    a = HasInspect.new(1)
    assert_equal("<inspect:1>\n", PP.pp(a, ''))
  end

  def test_hasprettyprint
    a = HasPrettyPrint.new(1)
    assert_equal("<pretty_print:1>\n", PP.pp(a, ''))
  end

  def test_hasboth
    a = HasBoth.new(1)
    assert_equal("<pretty_print:1>\n", PP.pp(a, ''))
  end

  def test_pretty_print_inspect
    a = PrettyPrintInspect.new(1)
    assert_equal("<pretty_print:1>", a.inspect)
    a = PrettyPrintInspectWithoutPrettyPrint.new
    assert_raise(RuntimeError) { a.inspect }
  end

  def test_proc
    a = proc {1}
    assert_equal("#{a.inspect}\n", PP.pp(a, ''))
  end

  def test_to_s_with_iv
    a = Object.new
    def a.to_s() "aaa" end
    a.instance_eval { @a = nil }
    result = PP.pp(a, '')
    assert_equal("#{a.inspect}\n", result)
    a = 1.0
    a.instance_eval { @a = nil }
    result = PP.pp(a, '')
    assert_equal("#{a.inspect}\n", result)
  end

  def test_to_s_without_iv
    a = Object.new
    def a.to_s() "aaa" end
    result = PP.pp(a, '')
    assert_equal("#{a.inspect}\n", result)
    assert_equal("aaa\n", result)
  end
end

class PPCycleTest < Test::Unit::TestCase
  def test_array
    a = []
    a << a
    assert_equal("[[...]]\n", PP.pp(a, ''))
    assert_equal("#{a.inspect}\n", PP.pp(a, ''))
  end

  def test_hash
    a = {}
    a[0] = a
    assert_equal("{0=>{...}}\n", PP.pp(a, ''))
    assert_equal("#{a.inspect}\n", PP.pp(a, ''))
  end

  S = Struct.new("S", :a, :b)
  def test_struct
    a = S.new(1,2)
    a.b = a
    assert_equal("#<struct Struct::S a=1, b=#<struct Struct::S:...>>\n", PP.pp(a, ''))
    assert_equal("#{a.inspect}\n", PP.pp(a, ''))
  end

  def test_object
    a = Object.new
    a.instance_eval {@a = a}
    assert_equal(a.inspect + "\n", PP.pp(a, ''))
  end

  def test_anonymous
    a = Class.new.new
    assert_equal(a.inspect + "\n", PP.pp(a, ''))
  end

  def test_withinspect
    a = []
    a << HasInspect.new(a)
    assert_equal("[<inspect:[...]>]\n", PP.pp(a, ''))
    assert_equal("#{a.inspect}\n", PP.pp(a, ''))
  end

  def test_share_nil
    begin
      PP.sharing_detection = true
      a = [nil, nil]
      assert_equal("[nil, nil]\n", PP.pp(a, ''))
    ensure
      PP.sharing_detection = false
    end
  end
end

class PPSingleLineTest < Test::Unit::TestCase
  def test_hash
    assert_equal("{1=>1}", PP.singleline_pp({ 1 => 1}, '')) # [ruby-core:02699]
    assert_equal("[1#{', 1'*99}]", PP.singleline_pp([1]*100, ''))
  end
end

class PPDelegateTest < Test::Unit::TestCase
  class A < DelegateClass(Array); end

  def test_delegate
    assert_equal("[]\n", A.new([]).pretty_inspect, "[ruby-core:25804]")
  end
end

end
