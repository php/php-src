require 'test/unit'
require 'timeout'

module TestParallel
  PARALLEL_RB = "#{File.dirname(__FILE__)}/../../lib/test/unit/parallel.rb"
  TESTS = "#{File.dirname(__FILE__)}/tests_for_parallel"


  class TestParallelWorker < Test::Unit::TestCase
    def setup
      i, @worker_in = IO.pipe
      @worker_out, o = IO.pipe
      @worker_pid = spawn(*@options[:ruby], PARALLEL_RB,
                          "--ruby", @options[:ruby].join(" "),
                          "-j", "t1", "-v", out: o, in: i)
      [i,o].each(&:close)
    end

    def teardown
      if @worker_pid && @worker_in
        begin
          begin
            @worker_in.puts "quit"
          rescue IOError, Errno::EPIPE
          end
          timeout(2) do
            Process.waitpid(@worker_pid)
          end
        rescue Timeout::Error
          begin
            Process.kill(:KILL, @worker_pid)
          rescue Errno::ESRCH
          end
        end
      end
    end

    def test_run
      timeout(10) do
        assert_match(/^ready/,@worker_out.gets)
        @worker_in.puts "run #{TESTS}/ptest_first.rb test"
        assert_match(/^okay/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^ready/,@worker_out.gets)
      end
    end

    def test_run_multiple_testcase_in_one_file
      timeout(10) do
        assert_match(/^ready/,@worker_out.gets)
        @worker_in.puts "run #{TESTS}/ptest_second.rb test"
        assert_match(/^okay/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^ready/,@worker_out.gets)
      end
    end

    def test_accept_run_command_multiple_times
      timeout(10) do
        assert_match(/^ready/,@worker_out.gets)
        @worker_in.puts "run #{TESTS}/ptest_first.rb test"
        assert_match(/^okay/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^ready/,@worker_out.gets)
        @worker_in.puts "run #{TESTS}/ptest_second.rb test"
        assert_match(/^okay/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^p/,@worker_out.gets)
        assert_match(/^done/,@worker_out.gets)
        assert_match(/^ready/,@worker_out.gets)
      end
    end

    def test_p
      timeout(10) do
        @worker_in.puts "run #{TESTS}/ptest_first.rb test"
        while buf = @worker_out.gets
          break if /^p (.+?)$/ =~ buf
        end
        assert_match(/TestA#test_nothing_test = \d+\.\d+ s = \.\n/, $1.chomp.unpack("m")[0])
      end
    end

    def test_done
      timeout(10) do
        @worker_in.puts "run #{TESTS}/ptest_forth.rb test"
        i = 0
        5.times { @worker_out.gets }
        buf = @worker_out.gets
        assert_match(/^done (.+?)$/, buf)

        /^done (.+?)$/ =~ buf

        result = Marshal.load($1.chomp.unpack("m")[0])

        assert_equal(result[0],3)
        assert_equal(result[1],2)
        assert_kind_of(Array,result[2])
        assert_kind_of(Array,result[3])
        assert_kind_of(Array,result[4])
        assert_match(/Skipped:$/,result[2][1])
        assert_match(/Failure:$/,result[2][0])
        assert_equal(result[5], "TestE")
      end
    end

    def test_quit
      timeout(10) do
        @worker_in.puts "quit"
        assert_match(/^bye$/m,@worker_out.read)
      end
    end
  end

  class TestParallel < Test::Unit::TestCase
    def spawn_runner(*opt_args)
      @test_out, o = IO.pipe
      @test_pid = spawn(*@options[:ruby], TESTS+"/runner.rb",
                        "--ruby", @options[:ruby].join(" "),
                        "-j","t1",*opt_args, out: o, err: o)
      o.close
    end

    def teardown
      begin
        if @test_pid
          timeout(2) do
            Process.waitpid(@test_pid)
          end
        end
      rescue Timeout::Error
        Process.kill(:KILL, @test_pid) if @test_pid
      ensure
        @test_out.close if @test_out
      end
    end

    def test_ignore_jzero
      @test_out, o = IO.pipe
      @test_pid = spawn(*@options[:ruby], TESTS+"/runner.rb",
                        "--ruby", @options[:ruby].join(" "),
                        "-j","0", out: File::NULL, err: o)
      o.close
      timeout(10) {
        assert_match(/Error: parameter of -j option should be greater than 0/,@test_out.read)
      }
    end

    def test_should_run_all_without_any_leaks
      spawn_runner
      buf = timeout(10){@test_out.read}
      assert_match(/^[SF\.]{7}$/,buf)
    end

    def test_should_retry_failed_on_workers
      spawn_runner
      buf = timeout(10){@test_out.read}
      assert_match(/^Retrying\.+$/,buf)
    end

    def test_no_retry_option
      spawn_runner "--no-retry"
      buf = timeout(10){@test_out.read}
      refute_match(/^Retrying\.+$/,buf)
      assert_match(/^ +\d+\) Failure:\ntest_fail_at_worker\(TestD\)/,buf)
    end

    def test_jobs_status
      spawn_runner "--jobs-status"
      buf = timeout(10){@test_out.read}
      assert_match(/\d+=ptest_(first|second|third|forth) */,buf)
    end

    def test_separate
      # this test depends to --jobs-status
      spawn_runner "--jobs-status", "--separate"
      buf = timeout(10){@test_out.read}
      assert(buf.scan(/(\d+?)[:=]/).flatten.uniq.size > 1)
    end
  end
end
