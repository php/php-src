require 'test/unit'
require 'timeout'
require 'thread'

class TestTimeout < Test::Unit::TestCase
  def test_queue
    q = Queue.new
    assert_raise(Timeout::Error, "[ruby-dev:32935]") {
      timeout(0.1) { q.pop }
    }
  end

  def test_timeout
    @flag = true
    Thread.start {
      sleep 0.1
      @flag = false
    }
    assert_nothing_raised("[ruby-dev:38319]") do
      Timeout.timeout(1) {
        nil while @flag
      }
    end
    assert !@flag, "[ruby-dev:38319]"
  end

  def test_cannot_convert_into_time_interval
    bug3168 = '[ruby-dev:41010]'
    def (n = Object.new).zero?; false; end
    assert_raise(TypeError, bug3168) {Timeout.timeout(n) { sleep 0.1 }}
  end
end
