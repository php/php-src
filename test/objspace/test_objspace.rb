require "test/unit"
require "objspace"
require_relative "../ruby/envutil"

class TestObjSpace < Test::Unit::TestCase
  def test_memsize_of
    assert_equal(0, ObjectSpace.memsize_of(true))
    assert_equal(0, ObjectSpace.memsize_of(nil))
    assert_equal(0, ObjectSpace.memsize_of(1))
    assert_kind_of(Integer, ObjectSpace.memsize_of(Object.new))
    assert_kind_of(Integer, ObjectSpace.memsize_of(Class))
    assert_kind_of(Integer, ObjectSpace.memsize_of(""))
    assert_kind_of(Integer, ObjectSpace.memsize_of([]))
    assert_kind_of(Integer, ObjectSpace.memsize_of({}))
    assert_kind_of(Integer, ObjectSpace.memsize_of(//))
    f = File.new(__FILE__)
    assert_kind_of(Integer, ObjectSpace.memsize_of(f))
    f.close
    assert_kind_of(Integer, ObjectSpace.memsize_of(/a/.match("a")))
    assert_kind_of(Integer, ObjectSpace.memsize_of(Struct.new(:a)))

    assert_operator(ObjectSpace.memsize_of(Regexp.new("(a)"*1000).match("a"*1000)),
                    :>,
                    ObjectSpace.memsize_of(//.match("")))
  end

  def test_argf_memsize
    size = ObjectSpace.memsize_of(ARGF)
    assert_kind_of(Integer, size)
    assert_operator(size, :>, 0)
    argf = ARGF.dup
    argf.inplace_mode = nil
    size = ObjectSpace.memsize_of(argf)
    argf.inplace_mode = "inplace_mode_suffix"
    assert_equal(size + 20, ObjectSpace.memsize_of(argf))
  end

  def test_memsize_of_all
    assert_kind_of(Integer, a = ObjectSpace.memsize_of_all)
    assert_kind_of(Integer, b = ObjectSpace.memsize_of_all(String))
    assert(a > b)
    assert(a > 0)
    assert(b > 0)
    assert_raise(TypeError) {ObjectSpace.memsize_of_all('error')}
  end

  def test_count_objects_size
    res = ObjectSpace.count_objects_size
    assert_equal(false, res.empty?)
    assert_equal(true, res[:TOTAL] > 0)
    arg = {}
    ObjectSpace.count_objects_size(arg)
    assert_equal(false, arg.empty?)
  end

  def test_count_nodes
    res = ObjectSpace.count_nodes
    assert_equal(false, res.empty?)
    arg = {}
    ObjectSpace.count_nodes(arg)
    assert_equal(false, arg.empty?)
  end

  def test_count_tdata_objects
    res = ObjectSpace.count_tdata_objects
    assert_equal(false, res.empty?)
    arg = {}
    ObjectSpace.count_tdata_objects(arg)
    assert_equal(false, arg.empty?)
  end
end
