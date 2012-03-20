require 'test/unit'
require 'fiber'
require 'continuation'
require_relative './envutil'

class TestFiber < Test::Unit::TestCase
  def test_normal
    f = Fiber.current
    assert_equal(:ok2,
      Fiber.new{|e|
        assert_equal(:ok1, e)
        Fiber.yield :ok2
      }.resume(:ok1)
    )
    assert_equal([:a, :b], Fiber.new{|a, b| [a, b]}.resume(:a, :b))
  end

  def test_argument
    assert_equal(4, Fiber.new {|i=4| i}.resume)
  end

  def test_term
    assert_equal(:ok, Fiber.new{:ok}.resume)
    assert_equal([:a, :b, :c, :d, :e],
      Fiber.new{
        Fiber.new{
          Fiber.new{
            Fiber.new{
              [:a]
            }.resume + [:b]
          }.resume + [:c]
        }.resume + [:d]
      }.resume + [:e])
  end

  def test_many_fibers
    max = 10000
    assert_equal(max, max.times{
      Fiber.new{}
    })
    assert_equal(max,
      max.times{|i|
        Fiber.new{
        }.resume
      }
    )
  end

  def test_many_fibers_with_threads
    max = 1000
    @cnt = 0
    (1..100).map{|ti|
      Thread.new{
        max.times{|i|
          Fiber.new{
            @cnt += 1
          }.resume
        }
      }
    }.each{|t|
      t.join
    }
    assert_equal(:ok, :ok)
  end

  def test_error
    assert_raise(ArgumentError){
      Fiber.new # Fiber without block
    }
    assert_raise(FiberError){
      f = Fiber.new{}
      Thread.new{f.resume}.join # Fiber yielding across thread
    }
    assert_raise(FiberError){
      f = Fiber.new{}
      f.resume
      f.resume
    }
    assert_raise(RuntimeError){
      f = Fiber.new{
        @c = callcc{|c| @c = c}
      }.resume
      @c.call # cross fiber callcc
    }
    assert_raise(RuntimeError){
      Fiber.new{
        raise
      }.resume
    }
    assert_raise(FiberError){
      Fiber.yield
    }
    assert_raise(FiberError){
      fib = Fiber.new{
        fib.resume
      }
      fib.resume
    }
    assert_raise(FiberError){
      fib = Fiber.new{
        Fiber.new{
          fib.resume
        }.resume
      }
      fib.resume
    }
  end

  def test_return
    assert_raise(LocalJumpError){
      Fiber.new do
        return
      end.resume
    }
  end

  def test_throw
    assert_raise(ArgumentError){
      Fiber.new do
        throw :a
      end.resume
    }
  end

  def test_transfer
    ary = []
    f2 = nil
    f1 = Fiber.new{
      ary << f2.transfer(:foo)
      :ok
    }
    f2 = Fiber.new{
      ary << f1.transfer(:baz)
      :ng
    }
    assert_equal(:ok, f1.transfer)
    assert_equal([:baz], ary)
  end

  def test_tls
    #
    def tvar(var, val)
      old = Thread.current[var]
      begin
        Thread.current[var] = val
        yield
      ensure
        Thread.current[var] = old
      end
    end

    fb = Fiber.new {
      assert_equal(nil, Thread.current[:v]); tvar(:v, :x) {
      assert_equal(:x,  Thread.current[:v]);   Fiber.yield
      assert_equal(:x,  Thread.current[:v]); }
      assert_equal(nil, Thread.current[:v]); Fiber.yield
      raise # unreachable
    }

    assert_equal(nil, Thread.current[:v]); tvar(:v,1) {
    assert_equal(1,   Thread.current[:v]);   tvar(:v,3) {
    assert_equal(3,   Thread.current[:v]);     fb.resume
    assert_equal(3,   Thread.current[:v]);   }
    assert_equal(1,   Thread.current[:v]); }
    assert_equal(nil, Thread.current[:v]); fb.resume
    assert_equal(nil, Thread.current[:v]);
  end

  def test_alive
    fib = Fiber.new{Fiber.yield}
    assert_equal(true, fib.alive?)
    fib.resume
    assert_equal(true, fib.alive?)
    fib.resume
    assert_equal(false, fib.alive?)
  end

  def test_resume_self
    f = Fiber.new {f.resume}
    assert_raise(FiberError, '[ruby-core:23651]') {f.transfer}
  end

  def test_fiber_transfer_segv
    assert_normal_exit %q{
      require 'fiber'
      f2 = nil
      f1 = Fiber.new{ f2.resume }
      f2 = Fiber.new{ f1.resume }
      f1.transfer
    }, '[ruby-dev:40833]'
    assert_normal_exit %q{
      require 'fiber'
      Fiber.new{}.resume
      1.times{Fiber.current.transfer}'
    }
  end

  def test_resume_root_fiber
    assert_raise(FiberError) do
      Thread.new do
        Fiber.current.resume
      end.join
    end
  end

  def test_gc_root_fiber
    bug4612 = '[ruby-core:35891]'

    assert_normal_exit %q{
      require 'fiber'
      GC.stress = true
      Thread.start{ Fiber.current; nil }.join
      GC.start
    }, bug4612
  end

  def test_no_valid_cfp
    bug5083 = '[ruby-dev:44208]'
    error = assert_raise(RuntimeError) do
      Fiber.new(&Module.method(:nesting)).resume
    end
    assert_equal("Can't call on top of Fiber or Thread", error.message, bug5083)
    error = assert_raise(RuntimeError) do
      Fiber.new(&Module.method(:undef_method)).resume(:to_s)
    end
    assert_equal("Can't call on top of Fiber or Thread", error.message, bug5083)
  end

  def test_prohibit_resume_transfered_fiber
    assert_raise(FiberError){
      root_fiber = Fiber.current
      f = Fiber.new{
        root_fiber.transfer
      }
      f.transfer
      f.resume
    }
    assert_raise(FiberError){
      g=nil
      f=Fiber.new{
        g.resume
        g.resume
      }
      g=Fiber.new{
        f.resume
        f.resume
      }
      f.transfer
    }
  end

  def test_fork_from_fiber
    begin
      Process.fork{}
    rescue NotImplementedError
      return
    end
    bug5700 = '[ruby-core:41456]'
    pid = nil
    assert_nothing_raised(bug5700) do
      Fiber.new{ pid = fork {} }.resume
    end
    pid, status = Process.waitpid2(pid)
    assert_equal(0, status.exitstatus, bug5700)
    assert_equal(false, status.signaled?, bug5700)
  end
end

