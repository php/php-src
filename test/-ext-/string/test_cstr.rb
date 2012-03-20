require 'test/unit'
require "-test-/string/string"

class Test_StringCStr < Test::Unit::TestCase
  Bug4319 = '[ruby-dev:43094]'

  def test_embed
    s = Bug::String.new("abcdef")
    s.set_len(3)
    assert_equal(0, s.cstr_term, Bug4319)
  end

  def test_long
    s = Bug::String.new("abcdef")*100000
    assert_equal(0, s.cstr_term, Bug4319)
  end
end
