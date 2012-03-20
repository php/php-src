require 'test/unit'
require 'thread'
require 'tmpdir'
require_relative '../ruby/envutil'

class TestQueue < Test::Unit::TestCase
  def test_queue
    grind(5, 1000, 15, Queue)
  end

  def test_sized_queue
    grind(5, 1000, 15, SizedQueue, 1000)
  end

  def grind(num_threads, num_objects, num_iterations, klass, *args)
    from_workers = klass.new(*args)
    to_workers = klass.new(*args)

    workers = (1..num_threads).map {
      Thread.new {
        while object = to_workers.pop
          from_workers.push object
        end
      }
    }

    Thread.new {
      num_iterations.times {
        num_objects.times { to_workers.push 99 }
        num_objects.times { from_workers.pop }
      }
    }.join

    num_threads.times { to_workers.push nil }
    workers.each { |t| t.join }

    assert_equal 0, from_workers.size
    assert_equal 0, to_workers.size
  end

  def test_sized_queue_initialize
    q = SizedQueue.new(1)
    assert_equal 1, q.max
    assert_raise(ArgumentError) { SizedQueue.new(0) }
    assert_raise(ArgumentError) { SizedQueue.new(-1) }
  end

  def test_sized_queue_assign_max
    q = SizedQueue.new(2)
    assert_equal(2, q.max)
    q.max = 1
    assert_equal(1, q.max)
    assert_raise(ArgumentError) { q.max = 0 }
    assert_equal(1, q.max)
    assert_raise(ArgumentError) { q.max = -1 }
    assert_equal(1, q.max)
  end

  def test_thr_kill
    bug5343 = '[ruby-core:39634]'
    Dir.mktmpdir {|d|
      timeout = 20
      total_count = 2000
      begin
        assert_normal_exit(<<-"_eom", bug5343, {:timeout => timeout, :chdir=>d})
          require "thread"
          #{total_count}.times do |i|
            open("test_thr_kill_count", "w") {|f| f.puts i }
            queue = Queue.new
            r, w = IO.pipe
            th = Thread.start {
              queue.push(nil)
              r.read 1
            }
            queue.pop
            th.kill.join
          end
        _eom
      rescue Timeout::Error
        count = File.read("#{d}/test_thr_kill_count").to_i
        flunk "only #{count}/#{total_count} done in #{timeout} seconds."
      end
    }
  end
end
