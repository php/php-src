require 'rubygems/test_case'
require 'rubygems/commands/contents_command'

class TestGemCommandsContentsCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::ContentsCommand.new
  end

  def gem name
    spec = quick_gem name do |gem|
      gem.files = %W[lib/#{name}.rb Rakefile]
    end
    write_file File.join(*%W[gems #{spec.full_name} lib #{name}.rb])
    write_file File.join(*%W[gems #{spec.full_name} Rakefile])
  end

  def test_execute
    @cmd.options[:args] = %w[foo]

    gem 'foo'

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|lib/foo\.rb|, @ui.output
    assert_match %r|Rakefile|, @ui.output
    assert_equal "", @ui.error
  end

  def test_execute_all
    @cmd.options[:all] = true

    gem 'foo'
    gem 'bar'

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|lib/foo\.rb|, @ui.output
    assert_match %r|lib/bar\.rb|, @ui.output
    assert_match %r|Rakefile|, @ui.output
    assert_equal "", @ui.error
  end

  def test_execute_bad_gem
    @cmd.options[:args] = %w[foo]

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_match %r|Unable to find gem 'foo' in default gem paths|, @ui.output
    assert_match %r|Directories searched:|, @ui.output
    assert_equal "", @ui.error
  end

  def test_execute_exact_match
    @cmd.options[:args] = %w[foo]
    gem 'foo'
    gem 'bar'

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|lib/foo\.rb|, @ui.output
    assert_match %r|Rakefile|, @ui.output
    assert_equal "", @ui.error
  end

  def test_execute_lib_only
    @cmd.options[:args] = %w[foo]
    @cmd.options[:lib_only] = true

    gem 'foo'

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|lib/foo\.rb|, @ui.output
    refute_match %r|Rakefile|, @ui.output

    assert_equal "", @ui.error
  end

  def test_execute_multiple
    @cmd.options[:args] = %w[foo bar]

    gem 'foo'
    gem 'bar'

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|lib/foo\.rb|, @ui.output
    assert_match %r|lib/bar\.rb|, @ui.output
    assert_match %r|Rakefile|, @ui.output
    assert_equal "", @ui.error
  end

  def test_execute_no_prefix
    @cmd.options[:args] = %w[foo]
    @cmd.options[:prefix] = false

    gem 'foo'

    use_ui @ui do
      @cmd.execute
    end

    expected = <<-EOF
Rakefile
lib/foo.rb
    EOF

    assert_equal expected, @ui.output

    assert_equal "", @ui.error
  end

  def test_handle_options
    refute @cmd.options[:lib_only]
    assert @cmd.options[:prefix]
    assert_empty @cmd.options[:specdirs]
    assert_nil @cmd.options[:version]

    @cmd.send :handle_options, %w[-l -s foo --version 0.0.2 --no-prefix]

    assert @cmd.options[:lib_only]
    refute @cmd.options[:prefix]
    assert_equal %w[foo], @cmd.options[:specdirs]
    assert_equal Gem::Requirement.new('0.0.2'), @cmd.options[:version]
  end

end

