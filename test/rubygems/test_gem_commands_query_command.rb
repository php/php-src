require 'rubygems/test_case'
require 'rubygems/commands/query_command'

class TestGemCommandsQueryCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::QueryCommand.new

    util_setup_fake_fetcher
    util_clear_gems
    util_setup_spec_fetcher @a1, @a2, @pl1, @a3a

    @fetcher.data["#{@gem_repo}Marshal.#{Gem.marshal_version}"] = proc do
      raise Gem::RemoteFetcher::FetchError
    end
  end

  def test_execute
    @cmd.handle_options %w[-r]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (2)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_platform
    @a1r = @a1.dup

    @a1.platform = 'x86-linux'
    @a2.platform = 'universal-darwin'

    util_clear_gems
    util_setup_spec_fetcher @a1, @a1r, @a2, @b2, @pl1

    @cmd.handle_options %w[-r -a]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (2 universal-darwin, 1 ruby x86-linux)
b (2)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_all
    @cmd.handle_options %w[-r --all]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (2, 1)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_all_prerelease
    @cmd.handle_options %w[-r --all --prerelease]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (3.a, 2, 1)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_details
    @a2.summary = 'This is a lot of text. ' * 4
    @a2.authors = ['Abraham Lincoln', 'Hirohito']
    @a2.homepage = 'http://a.example.com/'
    @a2.rubyforge_project = 'rubygems'

    util_clear_gems
    util_setup_spec_fetcher @a1, @a2, @pl1

    @cmd.handle_options %w[-r -d]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (2)
    Authors: Abraham Lincoln, Hirohito
    Rubyforge: http://rubyforge.org/projects/rubygems
    Homepage: http://a.example.com/

    This is a lot of text. This is a lot of text. This is a lot of text.
    This is a lot of text.

pl (1)
    Platform: i386-linux
    Author: A User
    Homepage: http://example.com

    this is a summary
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_details_platform
    @a1.platform = 'x86-linux'

    @a2.summary = 'This is a lot of text. ' * 4
    @a2.authors = ['Abraham Lincoln', 'Hirohito']
    @a2.homepage = 'http://a.example.com/'
    @a2.rubyforge_project = 'rubygems'
    @a2.platform = 'universal-darwin'

    util_clear_gems
    util_setup_spec_fetcher @a1, @a2, @pl1

    @cmd.handle_options %w[-r -d]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (2, 1)
    Platforms:
        1: x86-linux
        2: universal-darwin
    Authors: Abraham Lincoln, Hirohito
    Rubyforge: http://rubyforge.org/projects/rubygems
    Homepage: http://a.example.com/

    This is a lot of text. This is a lot of text. This is a lot of text.
    This is a lot of text.

pl (1)
    Platform: i386-linux
    Author: A User
    Homepage: http://example.com

    this is a summary
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_installed
    @cmd.handle_options %w[-n a --installed]

    assert_raises Gem::MockGemUi::SystemExitException do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal "true\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_installed_no_name
    @cmd.handle_options %w[--installed]

    e = assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal '', @ui.output
    assert_equal "ERROR:  You must specify a gem name\n", @ui.error

    assert_equal 4, e.exit_code
  end

  def test_execute_installed_not_installed
    @cmd.handle_options %w[-n not_installed --installed]

    e = assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal "false\n", @ui.output
    assert_equal '', @ui.error

    assert_equal 1, e.exit_code
  end

  def test_execute_installed_version
    @cmd.handle_options %w[-n a --installed --version 2]

    assert_raises Gem::MockGemUi::SystemExitException do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal "true\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_installed_version_not_installed
    @cmd.handle_options %w[-n c --installed --version 2]

    e = assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal "false\n", @ui.output
    assert_equal '', @ui.error

    assert_equal 1, e.exit_code
  end

  def test_execute_local_notty
    @cmd.handle_options %w[]

    @ui.outs.tty = false

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF
a (3.a, 2, 1)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_no_versions
    @cmd.handle_options %w[-r --no-versions]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a
pl
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_notty
    @cmd.handle_options %w[-r]

    @ui.outs.tty = false

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF
a (2)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_prerelease
    @cmd.handle_options %w[-r --prerelease]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** REMOTE GEMS ***

a (3.a)
    EOF

    assert_equal expected, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_prerelease_local
    @cmd.handle_options %w[-l --prerelease]

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF

*** LOCAL GEMS ***

a (3.a, 2, 1)
pl (1 i386-linux)
    EOF

    assert_equal expected, @ui.output
    assert_equal "WARNING:  prereleases are always shown locally\n", @ui.error
  end

end

