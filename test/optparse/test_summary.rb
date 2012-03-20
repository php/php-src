require_relative 'test_optparse'

class TestOptionParser::SummaryTest < TestOptionParser
  def test_short_clash
    r = nil
    o = OptionParser.new do |opts|
      opts.on("-f", "--first-option", "description 1", "description 2"){r = "first-option"}
      opts.on("-t", "--test-option"){r = "test-option"}
      opts.on("-t", "--another-test-option"){r = "another-test-option"}
      opts.separator "this is\nseparator"
      opts.on("-l", "--last-option"){r = "last-option"}
    end
    s = o.summarize
    o.parse("-t")
    assert_match(/--#{r}/, s.grep(/^\s*-t,/)[0])
    assert_match(/first-option/, s[0])
    assert_match(/description 1/, s[0])
    assert_match(/description 2/, s[1])
    assert_match(/last-option/, s[-1])
  end
end
