require 'runit/testcase'
require 'runit/cui/testrunner'
require 'timerholder'

class TimerHolderTest < RUNIT::TestCase
  def do_test(timeout, keeper_sleep = nil)
    holder = TimerHolder.new(timeout)
    holder.keeper_sleep = keeper_sleep if keeper_sleep
    key = holder.add(self)
    sleep(timeout * 0.5)
    assert_equal(holder.peek(key), self)
    holder.delete(key)
    assert(!holder.include?(key))
    key = holder.add(self)
    sleep(timeout+0.5)
    assert_equal(holder.fetch(key), nil)
    key = holder.add(self)
    assert_equal(holder.fetch(key), self)
    holder.store(key, true)
    assert_equal(holder.fetch(key), true)
    assert_equal(holder.include?(key), true)
    sleep(timeout+0.5)
    assert_exception(TimerHolder::InvalidIndexError) do
      holder.store(key, 1)
    end
    assert_equal(holder.include?(key), false)
    key = holder.add(self)
    sleep(timeout * 0.5)
    assert(holder.include?(key))
    holder.extend(key, timeout)
    sleep(timeout * 0.5)
    assert(holder.include?(key))
    sleep(timeout * 0.6)
    assert(!holder.include?(key))
    holder.delete(key)
  end

  def test_00
    do_test(0.5)
  end

  def test_01
    do_test(1, 0.5)
  end
end

if __FILE__ == $0
  RUNIT::CUI::TestRunner.run(TimerHolderTest.suite)
end
