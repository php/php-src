require 'test/unit'
require 'thread'
require_relative 'envutil'

class TestThread < Test::Unit::TestCase
  class Thread < ::Thread
    Threads = []
    def self.new(*)
      th = super
      th.abort_on_exception = true
      Threads << th
      th
    end
  end

  def setup
    Thread::Threads.clear
  end

  def teardown
    Thread::Threads.each do |t|
      t.kill if t.alive?
      begin
        t.join
      rescue Exception
      end
    end
  end

  def test_mutex_synchronize
    m = Mutex.new
    r = 0
    max = 10
    (1..max).map{
      Thread.new{
        i=0
        while i<max*max
          i+=1
          m.synchronize{
            r += 1
          }
        end
      }
    }.each{|e|
      e.join
    }
    assert_equal(max * max * max, r)
  end

  def test_condvar
    mutex = Mutex.new
    condvar = ConditionVariable.new
    result = []
    mutex.synchronize do
      t = Thread.new do
        mutex.synchronize do
          result << 1
          condvar.signal
        end
      end

      result << 0
      condvar.wait(mutex)
      result << 2
      t.join
    end
    assert_equal([0, 1, 2], result)
  end

  def test_condvar_wait_not_owner
    mutex = Mutex.new
    condvar = ConditionVariable.new

    assert_raise(ThreadError) { condvar.wait(mutex) }
  end

  def test_condvar_wait_exception_handling
    # Calling wait in the only thread running should raise a ThreadError of
    # 'stopping only thread'
    mutex = Mutex.new
    condvar = ConditionVariable.new

    locked = false
    thread = Thread.new do
      Thread.current.abort_on_exception = false
      mutex.synchronize do
        begin
          condvar.wait(mutex)
        rescue Exception
          locked = mutex.locked?
          raise
        end
      end
    end

    until thread.stop?
      sleep(0.1)
    end

    thread.raise Interrupt, "interrupt a dead condition variable"
    assert_raise(Interrupt) { thread.value }
    assert(locked)
  end

  def test_condvar_wait_and_broadcast
    nr_threads = 3
    threads = Array.new
    mutex = Mutex.new
    condvar = ConditionVariable.new
    result = []

    nr_threads.times do |i|
      threads[i] = Thread.new do
        mutex.synchronize do
          result << "C1"
          condvar.wait mutex
          result << "C2"
        end
      end
    end
    sleep 0.1
    mutex.synchronize do
      result << "P1"
      condvar.broadcast
      result << "P2"
    end
    nr_threads.times do |i|
      threads[i].join
    end

    assert_equal ["C1", "C1", "C1", "P1", "P2", "C2", "C2", "C2"], result
  end

#  Hmm.. don't we have a way of catch fatal exception?
#
#  def test_cv_wait_deadlock
#    mutex = Mutex.new
#    cv = ConditionVariable.new
#
#    assert_raise(fatal) {
#      mutex.lock
#      cv.wait mutex
#      mutex.unlock
#    }
#  end

  def test_condvar_wait_deadlock_2
    nr_threads = 3
    threads = Array.new
    mutex = Mutex.new
    condvar = ConditionVariable.new

    nr_threads.times do |i|
      if (i != 0)
        mutex.unlock
      end
      threads[i] = Thread.new do
        mutex.synchronize do
          condvar.wait mutex
        end
      end
      mutex.lock
    end

    assert_raise(Timeout::Error) do
      Timeout.timeout(0.1) { condvar.wait mutex }
    end
    mutex.unlock rescue
    threads[i].each.join
  end

  def test_condvar_timed_wait
    mutex = Mutex.new
    condvar = ConditionVariable.new
    timeout = 0.3
    locked = false

    t0 = Time.now
    mutex.synchronize do
      begin
        condvar.wait(mutex, timeout)
      ensure
        locked = mutex.locked?
      end
    end
    t1 = Time.now
    t = t1-t0

    assert_operator(timeout*0.9, :<, t)
    assert(locked)
  end

  def test_condvar_nolock
    mutex = Mutex.new
    condvar = ConditionVariable.new

    assert_raise(ThreadError) { condvar.wait(mutex) }
  end

  def test_condvar_nolock_2
    mutex = Mutex.new
    condvar = ConditionVariable.new

    Thread.new do
      assert_raise(ThreadError) {condvar.wait(mutex)}
    end.join
  end

  def test_condvar_nolock_3
    mutex = Mutex.new
    condvar = ConditionVariable.new

    Thread.new do
      assert_raise(ThreadError) {condvar.wait(mutex, 0.1)}
    end.join
  end

  def test_local_barrier
    dir = File.dirname(__FILE__)
    lbtest = File.join(dir, "lbtest.rb")
    $:.unshift File.join(File.dirname(dir), 'ruby')
    require 'envutil'
    $:.shift
    3.times {
      result = `#{EnvUtil.rubybin} #{lbtest}`
      assert(!$?.coredump?, '[ruby-dev:30653]')
      assert_equal("exit.", result[/.*\Z/], '[ruby-dev:30653]')
    }
  end

  def test_priority
    c1 = c2 = 0
    t1 = Thread.new { loop { c1 += 1 } }
    t1.priority = 3
    t2 = Thread.new { loop { c2 += 1 } }
    t2.priority = -3
    assert_equal(3, t1.priority)
    assert_equal(-3, t2.priority)
    sleep 0.5
    5.times do
      break if c1 > c2
      sleep 0.1
    end
    t1.kill
    t2.kill
    assert_operator(c1, :>, c2, "[ruby-dev:33124]") # not guaranteed
  end

  def test_new
    assert_raise(ThreadError) do
      Thread.new
    end

    t1 = Thread.new { sleep }
    assert_raise(ThreadError) do
      t1.instance_eval { initialize { } }
    end

    t2 = Thread.new(&method(:sleep).to_proc)
    assert_raise(ThreadError) do
      t2.instance_eval { initialize { } }
    end

  ensure
    t1.kill if t1
    t2.kill if t2
  end

  def test_join
    t = Thread.new { sleep }
    assert_nil(t.join(0.5))

  ensure
    t.kill if t
  end

  def test_join2
    t1 = Thread.new { sleep(1.5) }
    t2 = Thread.new do
      t1.join(1)
    end
    t3 = Thread.new do
      sleep 0.5
      t1.join
    end
    assert_nil(t2.value)
    assert_equal(t1, t3.value)

  ensure
    t1.kill if t1
    t2.kill if t2
    t3.kill if t3
  end

  def test_kill_main_thread
    assert_in_out_err([], <<-INPUT, %w(1), [])
      p 1
      Thread.kill Thread.current
      p 2
    INPUT
  end

  def test_kill_wrong_argument
    bug4367 = '[ruby-core:35086]'
    assert_raise(TypeError, bug4367) {
      Thread.kill(nil)
    }
    o = Object.new
    assert_raise(TypeError, bug4367) {
      Thread.kill(o)
    }
  end

  def test_kill_thread_subclass
    c = Class.new(Thread)
    t = c.new { sleep 10 }
    assert_nothing_raised { Thread.kill(t) }
    assert_equal(nil, t.value)
  end

  def test_exit
    s = 0
    Thread.new do
      s += 1
      Thread.exit
      s += 2
    end.join
    assert_equal(1, s)
  end

  def test_wakeup
    s = 0
    t = Thread.new do
      s += 1
      Thread.stop
      s += 1
    end
    sleep 0.5
    assert_equal(1, s)
    t.wakeup
    sleep 0.5
    assert_equal(2, s)
    assert_raise(ThreadError) { t.wakeup }

  ensure
    t.kill if t
  end

  def test_stop
    assert_in_out_err([], <<-INPUT, %w(2), [])
      begin
        Thread.stop
        p 1
      rescue ThreadError
        p 2
      end
    INPUT
  end

  def test_list
    assert_in_out_err([], <<-INPUT) do |r, e|
      t1 = Thread.new { sleep }
      Thread.pass
      t2 = Thread.new { loop { } }
      Thread.new { }.join
      p [Thread.current, t1, t2].map{|t| t.object_id }.sort
      p Thread.list.map{|t| t.object_id }.sort
    INPUT
      assert_equal(r.first, r.last)
      assert_equal([], e)
    end
  end

  def test_main
    assert_in_out_err([], <<-INPUT, %w(true false), [])
      p Thread.main == Thread.current
      Thread.new { p Thread.main == Thread.current }.join
    INPUT
  end

  def test_abort_on_exception
    assert_in_out_err([], <<-INPUT, %w(false 1), [])
      p Thread.abort_on_exception
      begin
        Thread.new { raise }
        sleep 0.5
        p 1
      rescue
        p 2
      end
    INPUT

    assert_in_out_err([], <<-INPUT, %w(true 2), [])
      Thread.abort_on_exception = true
      p Thread.abort_on_exception
      begin
        Thread.new { raise }
        sleep 0.5
        p 1
      rescue
        p 2
      end
    INPUT

    assert_in_out_err(%w(--disable-gems -d), <<-INPUT, %w(false 2), %r".+")
      p Thread.abort_on_exception
      begin
        Thread.new { raise }
        sleep 0.5
        p 1
      rescue
        p 2
      end
    INPUT

    assert_in_out_err([], <<-INPUT, %w(false true 2), [])
      p Thread.abort_on_exception
      begin
        t = Thread.new { sleep 0.5; raise }
        t.abort_on_exception = true
        p t.abort_on_exception
        sleep 1
        p 1
      rescue
        p 2
      end
    INPUT
  end

  def test_status_and_stop_p
    a = ::Thread.new { raise("die now") }
    b = Thread.new { Thread.stop }
    c = Thread.new { Thread.exit }
    d = Thread.new { sleep }
    e = Thread.current
    sleep 0.5

    assert_equal(nil, a.status)
    assert(a.stop?)

    assert_equal("sleep", b.status)
    assert(b.stop?)

    assert_equal(false, c.status)
    assert_match(/^#<TestThread::Thread:.* dead>$/, c.inspect)
    assert(c.stop?)

    d.kill
    assert_equal(["aborting", false], [d.status, d.stop?])

    assert_equal(["run", false], [e.status, e.stop?])

  ensure
    a.kill if a
    b.kill if b
    c.kill if c
    d.kill if d
  end

  def test_safe_level
    t = Thread.new { $SAFE = 3; sleep }
    sleep 0.5
    assert_equal(0, Thread.current.safe_level)
    assert_equal(3, t.safe_level)

  ensure
    t.kill if t
  end

  def test_thread_local
    t = Thread.new { sleep }

    assert_equal(false, t.key?(:foo))

    t["foo"] = "foo"
    t["bar"] = "bar"
    t["baz"] = "baz"

    assert_equal(true, t.key?(:foo))
    assert_equal(true, t.key?("foo"))
    assert_equal(false, t.key?(:qux))
    assert_equal(false, t.key?("qux"))

    assert_equal([:foo, :bar, :baz], t.keys)

  ensure
    t.kill if t
  end

  def test_thread_local_security
    t = Thread.new { sleep }

    assert_raise(SecurityError) do
      Thread.new { $SAFE = 4; t[:foo] }.join
    end

    assert_raise(SecurityError) do
      Thread.new { $SAFE = 4; t[:foo] = :baz }.join
    end

    assert_raise(RuntimeError) do
      Thread.new do
        Thread.current[:foo] = :bar
        Thread.current.freeze
        Thread.current[:foo] = :baz
      end.join
    end
  end

  def test_select_wait
    assert_nil(IO.select(nil, nil, nil, 1))
    t = Thread.new do
      IO.select(nil, nil, nil, nil)
    end
    sleep 0.5
    t.kill
  end

  def test_mutex_deadlock
    m = Mutex.new
    m.synchronize do
      assert_raise(ThreadError) do
        m.synchronize do
          assert(false)
        end
      end
    end
  end

  def test_mutex_interrupt
    m = Mutex.new
    m.lock
    t = Thread.new do
      m.lock
      :foo
    end
    sleep 0.5
    t.kill
    assert_nil(t.value)
  end

  def test_mutex_illegal_unlock
    m = Mutex.new
    m.lock
    assert_raise(ThreadError) do
      Thread.new do
        m.unlock
      end.join
    end
  end

  def test_mutex_fifo_like_lock
    m1 = Mutex.new
    m2 = Mutex.new
    m1.lock
    m2.lock
    m1.unlock
    m2.unlock
    assert_equal(false, m1.locked?)
    assert_equal(false, m2.locked?)

    m3 = Mutex.new
    m1.lock
    m2.lock
    m3.lock
    m1.unlock
    m2.unlock
    m3.unlock
    assert_equal(false, m1.locked?)
    assert_equal(false, m2.locked?)
    assert_equal(false, m3.locked?)
  end

  def test_mutex_trylock
    m = Mutex.new
    assert_equal(true, m.try_lock)
    assert_equal(false, m.try_lock, '[ruby-core:20943]')

    Thread.new{
      assert_equal(false, m.try_lock)
    }.join

    m.unlock
  end

  def test_recursive_outer
    arr = []
    obj = Struct.new(:foo, :visited).new(arr, false)
    arr << obj
    def obj.hash
      self[:visited] = true
      super
      raise "recursive_outer should short circuit intermediate calls"
    end
    assert_nothing_raised {arr.hash}
    assert(obj[:visited])
  end

  def test_thread_instance_variable
    bug4389 = '[ruby-core:35192]'
    assert_in_out_err([], <<-INPUT, %w(), [], bug4389)
      class << Thread.current
        @data = :data
      end
    INPUT
  end

  def test_no_valid_cfp
    skip 'with win32ole, cannot run this testcase because win32ole redefines Thread#intialize' if defined?(WIN32OLE)
    bug5083 = '[ruby-dev:44208]'
    error = assert_raise(RuntimeError) do
      Thread.new(&Module.method(:nesting)).join
    end
    assert_equal("Can't call on top of Fiber or Thread", error.message, bug5083)
    error = assert_raise(RuntimeError) do
      Thread.new(:to_s, &Module.method(:undef_method)).join
    end
    assert_equal("Can't call on top of Fiber or Thread", error.message, bug5083)
  end
end

class TestThreadGroup < Test::Unit::TestCase
  def test_thread_init
    thgrp = ThreadGroup.new
    Thread.new{
      thgrp.add(Thread.current)
      assert_equal(thgrp, Thread.new{sleep 1}.group)
    }.join
  end

  def test_frozen_thgroup
    thgrp = ThreadGroup.new

    t = Thread.new{1}
    Thread.new{
      thgrp.add(Thread.current)
      thgrp.freeze
      assert_raise(ThreadError) do
        Thread.new{1}.join
      end
      assert_raise(ThreadError) do
        thgrp.add(t)
      end
      assert_raise(ThreadError) do
        ThreadGroup.new.add Thread.current
      end
    }.join
    t.join
  end

  def test_enclosed_thgroup
    thgrp = ThreadGroup.new
    assert_equal(false, thgrp.enclosed?)

    t = Thread.new{1}
    Thread.new{
      thgrp.add(Thread.current)
      thgrp.enclose
      assert_equal(true, thgrp.enclosed?)
      assert_nothing_raised do
        Thread.new{1}.join
      end
      assert_raise(ThreadError) do
        thgrp.add t
      end
      assert_raise(ThreadError) do
        ThreadGroup.new.add Thread.current
      end
    }.join
    t.join
  end

  def test_uninitialized
    c = Class.new(Thread)
    c.class_eval { def initialize; end }
    assert_raise(ThreadError) { c.new.start }
  end

  def test_backtrace
    Thread.new{
      assert_equal(Array, Thread.main.backtrace.class)
    }.join

    t = Thread.new{}
    t.join
    assert_equal(nil, t.backtrace)
  end

  def test_thread_timer_and_interrupt
    bug5757 = '[ruby-dev:44985]'
    t0 = Time.now.to_f
    pid = nil
    cmd = 'r,=IO.pipe; Thread.start {Thread.pass until Thread.main.stop?; puts; STDOUT.flush}; r.read'
    s, err = EnvUtil.invoke_ruby(['-e', cmd], "", true, true) do |in_p, out_p, err_p, cpid|
      out_p.gets
      pid = cpid
      Process.kill(:SIGINT, pid)
      Process.wait(pid)
      [$?, err_p.read]
    end
    t1 = Time.now.to_f
    assert_equal(pid, s.pid)
    unless /mswin|mingw/ =~ RUBY_PLATFORM
      # status of signal is not supported on Windows
      assert_equal([false, true, false, Signal.list["INT"]],
                   [s.exited?, s.signaled?, s.stopped?, s.termsig],
                   "[s.exited?, s.signaled?, s.stopped?, s.termsig]")
    end
    assert_in_delta(t1 - t0, 1, 1)
  end
end
