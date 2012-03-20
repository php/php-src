require 'test/unit'
require_relative 'envutil'

class TestObjectSpace < Test::Unit::TestCase
  def self.deftest_id2ref(obj)
    /:(\d+)/ =~ caller[0]
    file = $`
    line = $1.to_i
    code = <<"End"
    define_method("test_id2ref_#{line}") {\
      o = ObjectSpace._id2ref(obj.object_id);\
      assert_same(obj, o, "didn't round trip: \#{obj.inspect}");\
    }
End
    eval code, binding, file, line
  end

  deftest_id2ref(-0x4000000000000001)
  deftest_id2ref(-0x4000000000000000)
  deftest_id2ref(-0x40000001)
  deftest_id2ref(-0x40000000)
  deftest_id2ref(-1)
  deftest_id2ref(0)
  deftest_id2ref(1)
  deftest_id2ref(0x3fffffff)
  deftest_id2ref(0x40000000)
  deftest_id2ref(0x3fffffffffffffff)
  deftest_id2ref(0x4000000000000000)
  deftest_id2ref(:a)
  deftest_id2ref(:abcdefghijilkjl)
  deftest_id2ref(:==)
  deftest_id2ref(Object.new)
  deftest_id2ref(self)
  deftest_id2ref(true)
  deftest_id2ref(false)
  deftest_id2ref(nil)

  def test_count_objects
    h = {}
    ObjectSpace.count_objects(h)
    assert_kind_of(Hash, h)
    assert(h.keys.all? {|x| x.is_a?(Symbol) || x.is_a?(Integer) })
    assert(h.values.all? {|x| x.is_a?(Integer) })

    h = ObjectSpace.count_objects
    assert_kind_of(Hash, h)
    assert(h.keys.all? {|x| x.is_a?(Symbol) || x.is_a?(Integer) })
    assert(h.values.all? {|x| x.is_a?(Integer) })

    assert_raise(TypeError) { ObjectSpace.count_objects(1) }

    h0 = {:T_FOO=>1000}
    h = ObjectSpace.count_objects(h0)
    assert_same(h0, h)
    assert_equal(0, h0[:T_FOO])
  end

  def test_finalizer
    assert_in_out_err(["-e", <<-END], "", %w(:ok :ok :ok :ok), [])
      a = []
      ObjectSpace.define_finalizer(a) { p :ok }
      b = a.dup
      ObjectSpace.define_finalizer(a) { p :ok }
      !b
    END
    assert_raise(ArgumentError) { ObjectSpace.define_finalizer([], Object.new) }
  end
end
