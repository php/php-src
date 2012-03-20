require 'rubygems/test_case'
require 'rubygems/commands/specification_command'

class TestGemCommandsSpecificationCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::SpecificationCommand.new
  end

  def test_execute
    foo = quick_spec 'foo'

    install_specs foo

    @cmd.options[:args] = %w[foo]

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|Gem::Specification|, @ui.output
    assert_match %r|name: foo|, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_all
    quick_spec 'foo', '0.0.1'
    quick_spec 'foo', '0.0.2'

    @cmd.options[:args] = %w[foo]
    @cmd.options[:all] = true

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|Gem::Specification|, @ui.output
    assert_match %r|name: foo|, @ui.output
    assert_match %r|version: 0.0.1|, @ui.output
    assert_match %r|version: 0.0.2|, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_bad_name
    @cmd.options[:args] = %w[foo]

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.execute
      end
    end

    assert_equal '', @ui.output
    assert_equal "ERROR:  Unknown gem 'foo'\n", @ui.error
  end

  def test_execute_exact_match
    quick_spec 'foo'
    quick_spec 'foo_bar'

    @cmd.options[:args] = %w[foo]

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|Gem::Specification|, @ui.output
    assert_match %r|name: foo|, @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_field
    foo = new_spec 'foo', '2'

    install_specs foo

    @cmd.options[:args] = %w[foo name]

    use_ui @ui do
      @cmd.execute
    end

    assert_equal "foo", YAML.load(@ui.output)
  end

  def test_execute_marshal
    foo = new_spec 'foo', '2'

    install_specs foo

    @cmd.options[:args] = %w[foo]
    @cmd.options[:format] = :marshal

    use_ui @ui do
      @cmd.execute
    end

    assert_equal foo, Marshal.load(@ui.output)
    assert_equal '', @ui.error
  end

  def test_execute_remote
    foo = quick_gem 'foo'

    @fetcher = Gem::FakeFetcher.new
    Gem::RemoteFetcher.fetcher = @fetcher

    util_setup_spec_fetcher foo

    FileUtils.rm File.join(@gemhome, 'specifications', foo.spec_name)

    @cmd.options[:args] = %w[foo]
    @cmd.options[:domain] = :remote

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|\A--- !ruby/object:Gem::Specification|, @ui.output
    assert_match %r|name: foo|, @ui.output
  end

  def test_execute_ruby
    foo = quick_spec 'foo'

    install_specs foo

    @cmd.options[:args] = %w[foo]
    @cmd.options[:format] = :ruby

    use_ui @ui do
      @cmd.execute
    end

    assert_match %r|Gem::Specification.new|, @ui.output
    assert_match %r|s.name = "foo"|, @ui.output
    assert_equal '', @ui.error
  end

end

