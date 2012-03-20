require 'test/unit'
require "-test-/string/string"
require_relative '../../ruby/envutil'

class Test_StringModifyExpand < Test::Unit::TestCase
  def test_modify_expand_memory_leak
    assert_no_memory_leak(["-r-test-/string/string"], <<-PRE, <<-CMD, "rb_str_modify_expand()")
      s=Bug::String.new
    PRE
      size = $initial_size
      10.times{s.modify_expand!(size)}
      s.replace("")
    CMD
  end
end
