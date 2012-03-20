require 'rubygems/test_case'
require 'rubygems/user_interaction'
require 'timeout'

class TestGemStreamUI < Gem::TestCase

  module IsTty
    attr_accessor :tty

    def tty?
      @tty = true unless defined? @tty
      return @tty
    end

    alias_method :isatty, :tty?

    def noecho
      yield self
    end
  end

  def setup
    super

    @cfg = Gem.configuration

    @in = StringIO.new
    @out = StringIO.new
    @err = StringIO.new

    @in.extend IsTty
    @out.extend IsTty

    @sui = Gem::StreamUI.new @in, @out, @err, true
  end

  def test_ask
    skip 'TTY detection broken on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    timeout(1) do
      expected_answer = "Arthur, King of the Britons"
      @in.string = "#{expected_answer}\n"
      actual_answer = @sui.ask("What is your name?")
      assert_equal expected_answer, actual_answer
    end
  end

  def test_ask_no_tty
    skip 'TTY detection broken on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    @in.tty = false

    timeout(0.1) do
      answer = @sui.ask("what is your favorite color?")
      assert_equal nil, answer
    end
  end

  def test_ask_for_password
    skip 'Always uses $stdin on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    timeout(1) do
      expected_answer = "Arthur, King of the Britons"
      @in.string = "#{expected_answer}\n"
      actual_answer = @sui.ask_for_password("What is your name?")
      assert_equal expected_answer, actual_answer
    end
  end

  def test_ask_for_password_no_tty
    skip 'TTY handling is broken on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    @in.tty = false

    timeout(0.1) do
      answer = @sui.ask_for_password("what is the airspeed velocity of an unladen swallow?")
      assert_equal nil, answer
    end
  end

  def test_ask_yes_no_no_tty_with_default
    skip 'TTY handling is broken on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    @in.tty = false

    timeout(0.1) do
      answer = @sui.ask_yes_no("do coconuts migrate?", false)
      assert_equal false, answer

      answer = @sui.ask_yes_no("do coconuts migrate?", true)
      assert_equal true, answer
    end
  end

  def test_ask_yes_no_no_tty_without_default
    skip 'TTY handling is broken on windows' if
      Gem.win_platform? unless RUBY_VERSION > '1.9.2'

    @in.tty = false

    timeout(0.1) do
      assert_raises(Gem::OperationNotSupportedError) do
        @sui.ask_yes_no("do coconuts migrate?")
      end
    end
  end

  def test_choose_from_list
    @in.puts "1"
    @in.rewind

    result = @sui.choose_from_list 'which one?', %w[foo bar]

    assert_equal ['foo', 0], result
    assert_equal "which one?\n 1. foo\n 2. bar\n> ", @out.string
  end

  def test_choose_from_list_EOF
    result = @sui.choose_from_list 'which one?', %w[foo bar]

    assert_equal [nil, nil], result
    assert_equal "which one?\n 1. foo\n 2. bar\n> ", @out.string
  end

  def test_progress_reporter_silent_nil
    @cfg.verbose = nil
    reporter = @sui.progress_reporter 10, 'hi'
    assert_kind_of Gem::StreamUI::SilentProgressReporter, reporter
  end

  def test_progress_reporter_silent_false
    @cfg.verbose = false
    reporter = @sui.progress_reporter 10, 'hi'
    assert_kind_of Gem::StreamUI::SilentProgressReporter, reporter
    assert_equal "", @out.string
  end

  def test_progress_reporter_simple
    @cfg.verbose = true
    reporter = @sui.progress_reporter 10, 'hi'
    assert_kind_of Gem::StreamUI::SimpleProgressReporter, reporter
    assert_equal "hi\n", @out.string
  end

  def test_progress_reporter_verbose
    @cfg.verbose = 0
    reporter = @sui.progress_reporter 10, 'hi'
    assert_kind_of Gem::StreamUI::VerboseProgressReporter, reporter
    assert_equal "hi\n", @out.string
  end

  def test_download_reporter_silent_nil
    @cfg.verbose = nil
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    assert_kind_of Gem::StreamUI::SilentDownloadReporter, reporter
    assert_equal "", @out.string
  end

  def test_download_reporter_silent_false
    @cfg.verbose = false
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    assert_kind_of Gem::StreamUI::SilentDownloadReporter, reporter
    assert_equal "", @out.string
  end

  def test_download_reporter_anything
    @cfg.verbose = 0
    reporter = @sui.download_reporter
    assert_kind_of Gem::StreamUI::VerboseDownloadReporter, reporter
  end

  def test_verbose_download_reporter
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    assert_equal "Fetching: a.gem", @out.string
  end

  def test_verbose_download_reporter_progress
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    reporter.update 512
    assert_equal "Fetching: a.gem\rFetching: a.gem ( 50%)", @out.string
  end

  def test_verbose_download_reporter_progress_once
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    reporter.update 510
    reporter.update 512
    assert_equal "Fetching: a.gem\rFetching: a.gem ( 50%)", @out.string
  end

  def test_verbose_download_reporter_progress_complete
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    reporter.update 510
    reporter.done
    assert_equal "Fetching: a.gem\rFetching: a.gem ( 50%)\rFetching: a.gem (100%)\n", @out.string
  end

  def test_verbose_download_reporter_progress_nil_length
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', nil
    reporter.update 1024
    reporter.done
    assert_equal "Fetching: a.gem\rFetching: a.gem (1024B)\rFetching: a.gem (1024B)\n", @out.string
  end

  def test_verbose_download_reporter_progress_zero_length
    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 0
    reporter.update 1024
    reporter.done
    assert_equal "Fetching: a.gem\rFetching: a.gem (1024B)\rFetching: a.gem (1024B)\n", @out.string
  end

  def test_verbose_download_reporter_no_tty
    @out.tty = false

    @cfg.verbose = true
    reporter = @sui.download_reporter
    reporter.fetch 'a.gem', 1024
    assert_equal "", @out.string
  end
end
