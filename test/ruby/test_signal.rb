require 'test/unit'
require 'timeout'
require 'tempfile'
require_relative 'envutil'

class TestSignal < Test::Unit::TestCase
  def have_fork?
    begin
      Process.fork {}
      return true
    rescue NotImplementedError
      return false
    end
  end

  def test_signal
    return unless Process.respond_to?(:kill)
    begin
      x = 0
      oldtrap = Signal.trap(:INT) {|sig| x = 2 }
      Process.kill :INT, Process.pid
      10.times do
        break if 2 == x
        sleep 0.1
      end
      assert_equal 2, x

      Signal.trap(:INT) { raise "Interrupt" }
      ex = assert_raise(RuntimeError) {
        Process.kill :INT, Process.pid
        sleep 0.1
      }
      assert_kind_of Exception, ex
      assert_match(/Interrupt/, ex.message)
    ensure
      Signal.trap :INT, oldtrap if oldtrap
    end
  end

  def test_signal_process_group
    return unless Process.respond_to?(:kill)
    return unless Process.respond_to?(:pgroup) # for mswin32

    bug4362 = '[ruby-dev:43169]'
    assert_nothing_raised(bug4362) do
      pid = Process.spawn(EnvUtil.rubybin, '-e', '"sleep 10"', :pgroup => true)
      Process.kill(:"-TERM", pid)
      Process.waitpid(pid)
      assert_equal(true, $?.signaled?)
      assert_equal(Signal.list["TERM"], $?.termsig)
    end
  end

  def test_exit_action
    return unless have_fork?	# skip this test
    begin
      r, w = IO.pipe
      r0, w0 = IO.pipe
      pid = Process.spawn(EnvUtil.rubybin, '-e', <<-'End', 3=>w, 4=>r0)
        w = IO.new(3, "w")
        r0 = IO.new(4, "r")
        Signal.trap(:USR1, "EXIT")
        w.syswrite("a")
        Thread.start { sleep(2) }
        r0.sysread(4096)
      End
      r.sysread(1)
      sleep 0.1
      assert_nothing_raised("[ruby-dev:26128]") {
        Process.kill(:USR1, pid)
        begin
          Timeout.timeout(3) {
            Process.waitpid pid
          }
        rescue Timeout::Error
          Process.kill(:TERM, pid)
          raise
        end
      }
    ensure
      r.close
      w.close
      r0.close
      w0.close
    end
  end

  def test_invalid_signal_name
    return unless Process.respond_to?(:kill)

    assert_raise(ArgumentError) { Process.kill(:XXXXXXXXXX, $$) }
  end

  def test_signal_exception
    assert_raise(ArgumentError) { SignalException.new }
    assert_raise(ArgumentError) { SignalException.new(-1) }
    assert_raise(ArgumentError) { SignalException.new(:XXXXXXXXXX) }
    Signal.list.each do |signm, signo|
      next if signm == "EXIT"
      assert_equal(SignalException.new(signm).signo, signo)
      assert_equal(SignalException.new(signm.to_sym).signo, signo)
      assert_equal(SignalException.new(signo).signo, signo)
    end
  end

  def test_interrupt
    assert_raise(Interrupt) { raise Interrupt.new }
  end

  def test_signal2
    return unless Process.respond_to?(:kill)
    begin
      x = false
      oldtrap = Signal.trap(:INT) {|sig| x = true }
      GC.start

      assert_raise(ArgumentError) { Process.kill }

      Timeout.timeout(10) do
        x = false
        Process.kill(SignalException.new(:INT).signo, $$)
        sleep(0.01) until x

        x = false
        Process.kill("INT", $$)
        sleep(0.01) until x

        x = false
        Process.kill("SIGINT", $$)
        sleep(0.01) until x

        x = false
        o = Object.new
        def o.to_str; "SIGINT"; end
        Process.kill(o, $$)
        sleep(0.01) until x
      end

      assert_raise(ArgumentError) { Process.kill(Object.new, $$) }

    ensure
      Signal.trap(:INT, oldtrap) if oldtrap
    end
  end

  def test_trap
    return unless Process.respond_to?(:kill)
    begin
      oldtrap = Signal.trap(:INT) {|sig| }

      assert_raise(ArgumentError) { Signal.trap }

      assert_raise(SecurityError) do
        s = proc {}.taint
        Signal.trap(:INT, s)
      end

      # FIXME!
      Signal.trap(:INT, nil)
      Signal.trap(:INT, "")
      Signal.trap(:INT, "SIG_IGN")
      Signal.trap(:INT, "IGNORE")

      Signal.trap(:INT, "SIG_DFL")
      Signal.trap(:INT, "SYSTEM_DEFAULT")

      Signal.trap(:INT, "EXIT")

      Signal.trap(:INT, "xxxxxx")
      Signal.trap(:INT, "xxxx")

      Signal.trap(SignalException.new(:INT).signo, "SIG_DFL")

      assert_raise(ArgumentError) { Signal.trap(-1, "xxxx") }

      o = Object.new
      def o.to_str; "SIGINT"; end
      Signal.trap(o, "SIG_DFL")

      assert_raise(ArgumentError) { Signal.trap("XXXXXXXXXX", "SIG_DFL") }

    ensure
      Signal.trap(:INT, oldtrap) if oldtrap
    end
  end

  def test_kill_immediately_before_termination
    return unless have_fork?	# skip this test

    r, w = IO.pipe
    pid = Process.fork do
      r.close
      Signal.trap(:USR1) { w.syswrite("foo") }
      Process.kill :USR1, $$
    end
    w.close
    assert_equal(r.read, "foo")
  end

  def test_signal_requiring
    t = Tempfile.new(%w"require_ensure_test .rb")
    t.puts "sleep"
    t.close
    error = IO.popen([EnvUtil.rubybin, "-e", <<EOS, t.path, :err => File::NULL]) do |child|
trap(:INT, "DEFAULT")
th = Thread.new do
  begin
    require ARGV[0]
  ensure
    Marshal.dump($!, STDOUT)
    STDOUT.flush
  end
end
Thread.pass while th.running?
Process.kill(:INT, $$)
th.join
EOS
      Marshal.load(child)
    end
    t.close!
    assert_nil(error)
  end

  def test_reserved_signal
    assert_raise(ArgumentError) {
      Signal.trap(:SEGV) {}
    }
    assert_raise(ArgumentError) {
      Signal.trap(:BUS) {}
    }
    assert_raise(ArgumentError) {
      Signal.trap(:ILL) {}
    }
    assert_raise(ArgumentError) {
      Signal.trap(:FPE) {}
    }
    assert_raise(ArgumentError) {
      Signal.trap(:VTALRM) {}
    }
  end

end
