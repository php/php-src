require "monitor"
require "thread"

require "test/unit"

class TestMonitor < Test::Unit::TestCase
  def setup
    @monitor = Monitor.new
  end

  def test_enter
    ary = []
    queue = Queue.new
    th = Thread.start {
      queue.pop
      @monitor.enter
      for i in 6 .. 10
        ary.push(i)
        Thread.pass
      end
      @monitor.exit
    }
    @monitor.enter
    queue.enq(nil)
    for i in 1 .. 5
      ary.push(i)
      Thread.pass
    end
    @monitor.exit
    th.join
    assert_equal((1..10).to_a, ary)
  end

  def test_synchronize
    ary = []
    queue = Queue.new
    th = Thread.start {
      queue.pop
      @monitor.synchronize do
        for i in 6 .. 10
          ary.push(i)
          Thread.pass
        end
      end
    }
    @monitor.synchronize do
      queue.enq(nil)
      for i in 1 .. 5
        ary.push(i)
        Thread.pass
      end
    end
    th.join
    assert_equal((1..10).to_a, ary)
  end

  def test_killed_thread_in_synchronize
    ary = []
    queue = Queue.new
    t1 = Thread.start {
      queue.pop
      @monitor.synchronize {
        ary << :t1
      }
    }
    t2 = Thread.start {
      queue.pop
      @monitor.synchronize {
        ary << :t2
      }
    }
    @monitor.synchronize do
      queue.enq(nil)
      queue.enq(nil)
      assert_equal([], ary)
      t1.kill
      t2.kill
      ary << :main
    end
    assert_equal([:main], ary)
  end

  def test_try_enter
    queue1 = Queue.new
    queue2 = Queue.new
    th = Thread.start {
      queue1.deq
      @monitor.enter
      queue2.enq(nil)
      queue1.deq
      @monitor.exit
      queue2.enq(nil)
    }
    assert_equal(true, @monitor.try_enter)
    @monitor.exit
    queue1.enq(nil)
    queue2.deq
    assert_equal(false, @monitor.try_enter)
    queue1.enq(nil)
    queue2.deq
    assert_equal(true, @monitor.try_enter)
  end

  def test_cond
    cond = @monitor.new_cond

    a = "foo"
    queue1 = Queue.new
    Thread.start do
      queue1.deq
      @monitor.synchronize do
        a = "bar"
        cond.signal
      end
    end
    @monitor.synchronize do
      queue1.enq(nil)
      assert_equal("foo", a)
      result1 = cond.wait
      assert_equal(true, result1)
      assert_equal("bar", a)
    end
  end

  def test_timedwait
    cond = @monitor.new_cond
    b = "foo"
    queue2 = Queue.new
    Thread.start do
      queue2.deq
      @monitor.synchronize do
        b = "bar"
        cond.signal
      end
    end
    @monitor.synchronize do
      queue2.enq(nil)
      assert_equal("foo", b)
      result2 = cond.wait(0.1)
      assert_equal(true, result2)
      assert_equal("bar", b)
    end

    c = "foo"
    queue3 = Queue.new
    Thread.start do
      queue3.deq
      @monitor.synchronize do
        c = "bar"
        cond.signal
      end
    end
    @monitor.synchronize do
      assert_equal("foo", c)
      result3 = cond.wait(0.1)
      assert_equal(true, result3) # wait always returns true in Ruby 1.9
      assert_equal("foo", c)
      queue3.enq(nil)
      result4 = cond.wait
      assert_equal(true, result4)
      assert_equal("bar", c)
    end

#     d = "foo"
#     cumber_thread = Thread.start {
#       loop do
#         @monitor.synchronize do
#           d = "foo"
#         end
#       end
#     }
#     queue3 = Queue.new
#     Thread.start do
#       queue3.pop
#       @monitor.synchronize do
#         d = "bar"
#         cond.signal
#       end
#     end
#     @monitor.synchronize do
#       queue3.enq(nil)
#       assert_equal("foo", d)
#       result5 = cond.wait
#       assert_equal(true, result5)
#       # this thread has priority over cumber_thread
#       assert_equal("bar", d)
#     end
#     cumber_thread.kill
  end
end
