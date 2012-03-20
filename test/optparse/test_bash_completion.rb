require 'test/unit'
require 'optparse'

class TestOptionParser < Test::Unit::TestCase
end
class TestOptionParser::BashCompletion < Test::Unit::TestCase
  def setup
    @opt = OptionParser.new
    @opt.define("-z", "zzz") {}
    @opt.define("--foo") {}
    @opt.define("--bar=BAR") {}
    @opt.define("--for=TYPE", [:hello, :help, :zot]) {}
  end

  def test_empty
    assert_equal([], @opt.candidate(""))
  end

  def test_one_hyphen
    assert_equal(%w[-z --foo --bar= --for=], @opt.candidate("-"))
  end

  def test_two_hyphen
    assert_equal(%w[--foo --bar= --for=], @opt.candidate("--"))
  end

  def test_long_f
    assert_equal(%w[--foo --for=], @opt.candidate("--f"))
  end

  def test_long_for_option
    assert_equal(%w[--for=], @opt.candidate("--for"))
  end

  def test_long_for_option_args
    assert_equal(%w[hello help zot], @opt.candidate("--for="))
  end

  def test_long_for_option_complete
    assert_equal(%w[hello help], @opt.candidate("--for=h"))
  end
end
