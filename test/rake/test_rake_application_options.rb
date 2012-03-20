require File.expand_path('../helper', __FILE__)

TESTING_REQUIRE = [ ]

class TestRakeApplicationOptions < Rake::TestCase

  def setup
    super

    clear_argv
    Rake::FileUtilsExt.verbose_flag = false
    Rake::FileUtilsExt.nowrite_flag = false
    TESTING_REQUIRE.clear
  end

  def teardown
    clear_argv
    Rake::FileUtilsExt.verbose_flag = false
    Rake::FileUtilsExt.nowrite_flag = false

    super
  end

  def clear_argv
    while ! ARGV.empty?
      ARGV.pop
    end
  end

  def test_default_options
    opts = command_line
    assert_nil opts.classic_namespace
    assert_nil opts.dryrun
    assert_nil opts.ignore_system
    assert_nil opts.load_system
    assert_nil opts.nosearch
    assert_equal ['rakelib'], opts.rakelib
    assert_nil opts.show_prereqs
    assert_nil opts.show_task_pattern
    assert_nil opts.show_tasks
    assert_nil opts.silent
    assert_nil opts.trace
    assert_equal ['rakelib'], opts.rakelib
    assert ! Rake::FileUtilsExt.verbose_flag
    assert ! Rake::FileUtilsExt.nowrite_flag
  end

  def test_dry_run
    flags('--dry-run', '-n') do |opts|
      assert opts.dryrun
      assert opts.trace
      assert Rake::FileUtilsExt.verbose_flag
      assert Rake::FileUtilsExt.nowrite_flag
    end
  end

  def test_describe
    flags('--describe') do |opts|
      assert_equal :describe, opts.show_tasks
      assert_equal(//.to_s, opts.show_task_pattern.to_s)
    end
  end

  def test_describe_with_pattern
    flags('--describe=X') do |opts|
      assert_equal :describe, opts.show_tasks
      assert_equal(/X/.to_s, opts.show_task_pattern.to_s)
    end
  end

  def test_execute
    $xyzzy = 0
    flags('--execute=$xyzzy=1', '-e $xyzzy=1') do |opts|
      assert_equal 1, $xyzzy
      assert_equal :exit, @exit
      $xyzzy = 0
    end
  end

  def test_execute_and_continue
    $xyzzy = 0
    flags('--execute-continue=$xyzzy=1', '-E $xyzzy=1') do |opts|
      assert_equal 1, $xyzzy
      refute_equal :exit, @exit
      $xyzzy = 0
    end
  end

  def test_execute_and_print
    $xyzzy = 0
    out, = capture_io do
      flags('--execute-print=$xyzzy="pugh"', '-p $xyzzy="pugh"') do |opts|
        assert_equal 'pugh', $xyzzy
        assert_equal :exit, @exit
        $xyzzy = 0
      end
    end

    assert_match(/^pugh$/, out)
  end

  def test_help
    out, = capture_io do
      flags '--help', '-H', '-h'
    end

    assert_match(/\Arake/, out)
    assert_match(/\boptions\b/, out)
    assert_match(/\btargets\b/, out)
    assert_equal :exit, @exit
  end

  def test_libdir
    flags(['--libdir', 'xx'], ['-I', 'xx'], ['-Ixx']) do |opts|
      $:.include?('xx')
    end
  ensure
    $:.delete('xx')
  end

  def test_rakefile
    flags(['--rakefile', 'RF'], ['--rakefile=RF'], ['-f', 'RF'], ['-fRF']) do |opts|
      assert_equal ['RF'], @app.instance_eval { @rakefiles }
    end
  end

  def test_rakelib
    flags(['--rakelibdir', 'A:B:C'], ['--rakelibdir=A:B:C'], ['-R', 'A:B:C'], ['-RA:B:C']) do |opts|
      assert_equal ['A', 'B', 'C'], opts.rakelib
    end
  end

  def test_require
    $LOAD_PATH.unshift @tempdir

    open 'reqfile.rb',    'w' do |io| io << 'TESTING_REQUIRE << 1' end
    open 'reqfile2.rb',   'w' do |io| io << 'TESTING_REQUIRE << 2' end
    open 'reqfile3.rake', 'w' do |io| io << 'TESTING_REQUIRE << 3' end

    flags(['--require', 'reqfile'], '-rreqfile2', '-rreqfile3')

    assert_includes TESTING_REQUIRE, 1
    assert_includes TESTING_REQUIRE, 2
    assert_includes TESTING_REQUIRE, 3

    assert_equal 3, TESTING_REQUIRE.size
  ensure
    $LOAD_PATH.delete @tempdir
  end

  def test_missing_require
    ex = assert_raises(LoadError) do
      flags(['--require', 'test/missing']) do |opts|
      end
    end
    assert_match(/such file/, ex.message)
    assert_match(/test\/missing/, ex.message)
  end

  def test_prereqs
    flags('--prereqs', '-P') do |opts|
      assert opts.show_prereqs
    end
  end

  def test_quiet
    flags('--quiet', '-q') do |opts|
      assert ! Rake::FileUtilsExt.verbose_flag
      assert ! opts.silent
    end
  end

  def test_no_search
    flags('--nosearch', '--no-search', '-N') do |opts|
      assert opts.nosearch
    end
  end

  def test_silent
    flags('--silent', '-s') do |opts|
      assert ! Rake::FileUtilsExt.verbose_flag
      assert opts.silent
    end
  end

  def test_system
    flags('--system', '-g') do |opts|
      assert opts.load_system
    end
  end

  def test_no_system
    flags('--no-system', '-G') do |opts|
      assert opts.ignore_system
    end
  end

  def test_trace
    flags('--trace', '-t') do |opts|
      assert opts.trace
      assert Rake::FileUtilsExt.verbose_flag
      assert ! Rake::FileUtilsExt.nowrite_flag
    end
  end

  def test_trace_rules
    flags('--rules') do |opts|
      assert opts.trace_rules
    end
  end

  def test_tasks
    flags('--tasks', '-T') do |opts|
      assert_equal :tasks, opts.show_tasks
      assert_equal(//.to_s, opts.show_task_pattern.to_s)
    end
    flags(['--tasks', 'xyz'], ['-Txyz']) do |opts|
      assert_equal :tasks, opts.show_tasks
      assert_equal(/xyz/.to_s, opts.show_task_pattern.to_s)
    end
  end

  def test_where
    flags('--where', '-W') do |opts|
      assert_equal :lines, opts.show_tasks
      assert_equal(//.to_s, opts.show_task_pattern.to_s)
    end
    flags(['--where', 'xyz'], ['-Wxyz']) do |opts|
      assert_equal :lines, opts.show_tasks
      assert_equal(/xyz/.to_s, opts.show_task_pattern.to_s)
    end
  end

  def test_no_deprecated_messages
    flags('--no-deprecation-warnings', '-X') do |opts|
      assert opts.ignore_deprecate
    end
  end

  def test_verbose
    out, = capture_io do
      flags('--verbose', '-V') do |opts|
        assert Rake::FileUtilsExt.verbose_flag
        assert ! opts.silent
      end
    end

    assert_equal "rake, version #{Rake::VERSION}\n", out
  end

  def test_version
    out, = capture_io do
      flags '--version', '-V'
    end

    assert_match(/\bversion\b/, out)
    assert_match(/\b#{RAKEVERSION}\b/, out)
    assert_equal :exit, @exit
  end

  def test_classic_namespace
    _, err = capture_io do
      flags(['--classic-namespace'],
            ['-C', '-T', '-P', '-n', '-s', '-t']) do |opts|
        assert opts.classic_namespace
        assert_equal opts.show_tasks, $show_tasks
        assert_equal opts.show_prereqs, $show_prereqs
        assert_equal opts.trace, $trace
        assert_equal opts.dryrun, $dryrun
        assert_equal opts.silent, $silent
            end
    end

    assert_match(/deprecated/, err)
  end

  def test_bad_option
    _, err = capture_io do
      ex = assert_raises(OptionParser::InvalidOption) do
        flags('--bad-option')
      end

      if ex.message =~ /^While/ # Ruby 1.9 error message
        assert_match(/while parsing/i, ex.message)
      else                      # Ruby 1.8 error message
        assert_match(/(invalid|unrecognized) option/i, ex.message)
        assert_match(/--bad-option/, ex.message)
      end
    end

    assert_equal '', err
  end

  def test_task_collection
    command_line("a", "b")
    assert_equal ["a", "b"], @tasks.sort
  end

  def test_default_task_collection
    command_line()
    assert_equal ["default"], @tasks
  end

  def test_environment_definition
    ENV.delete('TESTKEY')
    command_line("a", "TESTKEY=12")
    assert_equal ["a"], @tasks.sort
    assert '12', ENV['TESTKEY']
  end

  def flags(*sets)
    sets.each do |set|
      ARGV.clear

      @exit = catch(:system_exit) { command_line(*set) }

      yield(@app.options) if block_given?
    end
  end

  def command_line(*options)
    options.each do |opt| ARGV << opt end
    @app = Rake::Application.new
    def @app.exit(*args)
      throw :system_exit, :exit
    end
    @app.instance_eval do
      handle_options
      collect_tasks
    end
    @tasks = @app.top_level_tasks
    @app.options
  end
end

