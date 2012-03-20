require 'test/unit'
require 'thread'
require 'mutex_m'

class TestMutexM < Test::Unit::TestCase
  def test_cv_wait
    o = Object.new
    o.instance_variable_set(:@foo, nil)
    o.extend(Mutex_m)
    c = ConditionVariable.new
    t = Thread.start {
      o.synchronize do
        until foo = o.instance_variable_get(:@foo)
          c.wait(o)
        end
        foo
      end
    }
    sleep(0.0001)
    o.synchronize do
      o.instance_variable_set(:@foo, "abc")
    end
    c.signal
    assert_equal "abc", t.value
  end
end
