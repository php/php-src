require 'test/unit'

class TestHideSkip < Test::Unit::TestCase
  def test_hideskip
    test_out, o = IO.pipe
    spawn(*@options[:ruby], "#{File.dirname(__FILE__)}/test4test_hideskip.rb",
          out: o, err: o)
    o.close
    assert_match(/assertions\/s.\n\n  1\) Skipped/,test_out.read)
    test_out.close

    test_out, o = IO.pipe
    spawn(*@options[:ruby], "#{File.dirname(__FILE__)}/test4test_hideskip.rb",
          "--show-skip", out: o, err: o)
    o.close
    assert_match(/assertions\/s.\n\n  1\) Skipped/,test_out.read)
    test_out.close

    test_out, o = IO.pipe
    spawn(*@options[:ruby], "#{File.dirname(__FILE__)}/test4test_hideskip.rb",
          "--hide-skip", out: o, err: o)
    o.close
    assert_match(/assertions\/s.\n\n1 tests, 0 assertions, 0 failures, 0 errors, 1 skips/,
                 test_out.read)
    test_out.close
  end
end
