require 'test/unit'
require "-test-/string/string"

class Test_StrEncAssociate < Test::Unit::TestCase
  def test_frozen
    s = Bug::String.new("abc")
    s.force_encoding(Encoding::US_ASCII)
    s.freeze
    assert_raise(RuntimeError) {s.associate_encoding!(Encoding::US_ASCII)}
    assert_raise(RuntimeError) {s.associate_encoding!(Encoding::UTF_8)}
  end
end
