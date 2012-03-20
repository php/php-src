require 'test/unit'
require_relative '../ruby/envutil'

class Test_BUG_3571 < Test::Unit::TestCase
  def test_block_call_id
    bug3571 = '[ruby-dev:41852]'
    src = <<SRC
begin
  Bug.start
rescue NotImplementedError => e
  STDERR.puts e.message, e.backtrace[$0.size..-1]
end
SRC
    out = [
      "start() function is unimplemented on this machine",
      "-:2:in `start'",
      "-:2:in `<main>'",
    ]
    assert_in_out_err(%w"-r-test-/bug-3571/bug", src, [], out, bug3571)
  end
end
