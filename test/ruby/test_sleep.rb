require 'test/unit'

class TestSleep < Test::Unit::TestCase
  def test_sleep_5sec
    GC.disable
    start = Time.now
    sleep 5
    slept = Time.now-start
    bottom = /linux/ =~ RUBY_PLATFORM && /Linux ([\d.]+)/ =~ `uname -sr` && ($1.split('.')<=>%w/2 6 18/)<1 ? 4.99 : 5.0
    assert_operator(bottom, :<=, slept)
    assert_operator(slept, :<=, 6.0, "[ruby-core:18015]: longer than expected")
  ensure
    GC.enable
  end
end
