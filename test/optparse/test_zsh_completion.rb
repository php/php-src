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

  def test_compsys
    compsys = @opt.compsys("", "zshcompsys")
    assert_match(/\"-z\[zzz\]\"/, compsys)
    assert_match(/\"--foo\[\]\"/, compsys)
    assert_match(/\"--bar\[\]\"/, compsys)
    assert_match(/\"--for\[\]\"/, compsys)
  end
end
