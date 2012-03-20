require 'test/unit'
require "-test-/typeddata/typeddata"

class Test_TypedData < Test::Unit::TestCase
  def test_wrong_argtype
    e = assert_raise(TypeError) {Bug::TypedData.check(false)}
    assert_equal("wrong argument type false (expected typed_data)", e.message)

    e = assert_raise(TypeError) {Bug::TypedData.check(true)}
    assert_equal("wrong argument type true (expected typed_data)", e.message)

    e = assert_raise(TypeError) {Bug::TypedData.check(:e)}
    assert_equal("wrong argument type Symbol (expected typed_data)", e.message)

    e = assert_raise(TypeError) {Bug::TypedData.check(0)}
    assert_equal("wrong argument type Fixnum (expected typed_data)", e.message)

    e = assert_raise(TypeError) {Bug::TypedData.check("a")}
    assert_equal("wrong argument type String (expected typed_data)", e.message)
  end
end
