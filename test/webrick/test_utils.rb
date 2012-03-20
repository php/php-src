require "test/unit"
require "webrick/utils"

class TestWEBrickUtils < Test::Unit::TestCase
  def assert_expired(flag, m)
    if m == WEBrick::Utils
      handler = WEBrick::Utils::TimeoutHandler.instance
      assert_equal(flag, handler.instance_eval{ @timeout_info.empty? })
    end
  end

  def do_test_timeout(m)
    ex = Class.new(StandardError)

    assert_equal(:foo, m.timeout(10){ :foo })
    assert_expired(true, m)

    i = 0
    assert_raise(Timeout::Error){
      m.timeout(2){
        assert_raise(Timeout::Error){ m.timeout(1){ i += 1; sleep } }
        assert_expired(false, m)
        i += 1
        sleep
      }
    }
    assert_equal(2, i)
    assert_expired(true, m)

    assert_raise(Timeout::Error){ m.timeout(0.1){ sleep } }
    assert_expired(true, m)

    assert_raise(ex){ m.timeout(0.1, ex){ sleep } }
    assert_expired(true, m)

    i = 0
    assert_raise(ex){
      m.timeout(10){
        m.timeout(1, ex){ i += 1; sleep }
      }
      sleep
    }
    assert_equal(1, i)
    assert_expired(true, m)

    i = 0
    assert_raise(Timeout::Error){
      m.timeout(1){
        m.timeout(10, ex){ i += 1; sleep }
      }
      sleep
    }
    assert_equal(1, i)
    assert_expired(true, m)
  end

  def test_webrick_timeout
    do_test_timeout(WEBrick::Utils)
  end

  #def test_timeout
  #  do_test_timeout(Timeout)
  #end
end
