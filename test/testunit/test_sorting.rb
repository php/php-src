require 'test/unit'

class TestTestUnitSorting < Test::Unit::TestCase
  def test_sorting
    test_out, o = IO.pipe
    spawn(*@options[:ruby], "#{File.dirname(__FILE__)}/test4test_sorting.rb",
          out: o, err: o)
    o.close
    result = test_out.read

    assert_match(/^  1\) Skipped:/, result)
    assert_match(/^  2\) Failure:/, result)
    assert_match(/^  3\) Error:/,   result)

    test_out.close
  end
end
