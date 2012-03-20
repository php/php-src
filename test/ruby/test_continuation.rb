require 'test/unit'
require 'continuation'
require 'fiber'
require_relative 'envutil'

class TestContinuation < Test::Unit::TestCase
  def test_create
    assert_equal(:ok, callcc{:ok})
    assert_equal(:ok, callcc{|c| c.call :ok})
  end

  def test_call
    assert_equal(:ok, callcc{|c| c.call :ok})

    ary = []
    ary << callcc{|c|
      @cont = c
      :a
    }
    @cont.call :b if ary.length < 3
    assert_equal([:a, :b, :b], ary)
  end

  def test_check_localvars
    vv = 0
    @v = 0
    @ary = []
    [1, 2, 3].each{|i|
      callcc {|k| @k = k}
      @v += 1
      vv += 1
    }
    @ary << [vv, @v]
    @k.call if @v < 10
    assert_equal((3..10).map{|e| [e, e]}, @ary)
  end

  def test_error
    cont = callcc{|c| c}
    assert_raise(RuntimeError){
      Thread.new{cont.call}.join
    }
    assert_raise(LocalJumpError){
      callcc
    }
    assert_raise(RuntimeError){
      c = nil
      Fiber.new do
        callcc {|c2| c = c2 }
      end.resume
      c.call
    }
  end

  def test_ary_flatten
    assert_normal_exit %q{
      require 'continuation'
      n = 0
      o = Object.new
      def o.to_ary() callcc {|k| $k = k; [1,2,3]} end
      [10,20,o,30,o,40].flatten.inspect
      n += 1
      $k.call if n < 100
    }, '[ruby-dev:34798]'
  end

  def test_marshal_dump
    assert_normal_exit %q{
      require 'continuation'
      n = 0
      o = Object.new
      def o.marshal_dump() callcc {|k| $k = k };  "fofof" end
      a = [1,2,3,o,4,5,6]
      Marshal.dump(a).inspect
      n += 1
      $k.call if n < 100
    }, '[ruby-dev:34802]'
  end

  def tracing_with_set_trace_func
    cont = nil
    func = lambda do |*args|
      @memo += 1
      cont.call(nil)
    end
    cont = callcc { |cc| cc }
    if cont
      set_trace_func(func)
    else
      set_trace_func(nil)
    end
  end

  def test_tracing_with_set_trace_func
    @memo = 0
    tracing_with_set_trace_func
    tracing_with_set_trace_func
    tracing_with_set_trace_func
    assert_equal 3, @memo
  end

  def tracing_with_thread_set_trace_func
    cont = nil
    func = lambda do |*args|
      @memo += 1
      cont.call(nil)
    end
    cont = callcc { |cc| cc }
    if cont
      Thread.current.set_trace_func(func)
    else
      Thread.current.set_trace_func(nil)
    end
  end

  def test_tracing_with_thread_set_trace_func
    @memo = 0
    tracing_with_thread_set_trace_func
    tracing_with_thread_set_trace_func
    tracing_with_thread_set_trace_func
    assert_equal 3, @memo
  end
end

