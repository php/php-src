require 'rubygems/test_case'
require 'rubygems/command'

class Gem::Command
  public :parser
end

class TestGemCommand < Gem::TestCase

  def setup
    super

    @xopt = nil

    Gem::Command.common_options.clear
    Gem::Command.common_options <<  [
      ['-x', '--exe', 'Execute'], lambda do |*a|
        @xopt = true
      end
    ]

    @cmd_name = 'doit'
    @cmd = Gem::Command.new @cmd_name, 'summary'
  end

  def test_self_add_specific_extra_args
    added_args = %w[--all]
    @cmd.add_option '--all' do |v,o| end

    Gem::Command.add_specific_extra_args @cmd_name, added_args

    assert_equal added_args, Gem::Command.specific_extra_args(@cmd_name)

    h = @cmd.add_extra_args []

    assert_equal added_args, h
  end

  def test_self_add_specific_extra_args_unknown
    added_args = %w[--definitely_not_there]

    Gem::Command.add_specific_extra_args @cmd_name, added_args

    assert_equal added_args, Gem::Command.specific_extra_args(@cmd_name)

    h = @cmd.add_extra_args []

    assert_equal [], h
  end

  def test_basic_accessors
    assert_equal "doit", @cmd.command
    assert_equal "gem doit", @cmd.program_name
    assert_equal "summary", @cmd.summary
  end

  def test_common_option_in_class
    assert Array === Gem::Command.common_options
  end

  def test_defaults
    @cmd.add_option('-h', '--help [COMMAND]', 'Get help on COMMAND') do |value, options|
      options[:help] = value
    end

    @cmd.defaults = { :help => true }

    @cmd.when_invoked do |options|
      assert options[:help], "Help options should default true"
    end

    use_ui @ui do
      @cmd.invoke
    end

    assert_match %r|Usage: gem doit|, @ui.output
  end

  def test_invoke
    done = false
    @cmd.when_invoked { done = true }

    use_ui @ui do
      @cmd.invoke
    end

    assert done
  end

  def test_invoke_with_bad_options
    use_ui @ui do
      @cmd.when_invoked do true end

      ex = assert_raises OptionParser::InvalidOption do
        @cmd.invoke('-zzz')
      end

      assert_match(/invalid option:/, ex.message)
    end
  end

  def test_invoke_with_common_options
    @cmd.when_invoked do true end

    use_ui @ui do
      @cmd.invoke "-x"
    end

    assert @xopt, "Should have done xopt"
  end

  # Returning false from the command handler invokes the usage output.
  def test_invoke_with_help
    done = false

    use_ui @ui do
      @cmd.add_option('-h', '--help [COMMAND]', 'Get help on COMMAND') do |value, options|
        options[:help] = true
        done = true
      end

      @cmd.invoke('--help')

      assert done
    end

    assert_match(/Usage/, @ui.output)
    assert_match(/gem doit/, @ui.output)
    assert_match(/\[options\]/, @ui.output)
    assert_match(/-h/, @ui.output)
    assert_match(/--help \[COMMAND\]/, @ui.output)
    assert_match(/Get help on COMMAND/, @ui.output)
    assert_match(/-x/, @ui.output)
    assert_match(/--exe/, @ui.output)
    assert_match(/Execute/, @ui.output)
    assert_match(/Common Options:/, @ui.output)
  end

  def test_invoke_with_options
    @cmd.add_option('-h', '--help [COMMAND]', 'Get help on COMMAND') do |value, options|
      options[:help] = true
    end

    @cmd.when_invoked do |opts|
      assert opts[:help]
    end

    use_ui @ui do
      @cmd.invoke '-h'
    end

    assert_match %r|Usage: gem doit|, @ui.output
  end

  def test_option_recognition
    @cmd.add_option('-h', '--help [COMMAND]', 'Get help on COMMAND') do |value, options|
      options[:help] = true
    end
    @cmd.add_option('-f', '--file FILE', 'File option') do |value, options|
      options[:help] = true
    end
    assert @cmd.handles?(['-x'])
    assert @cmd.handles?(['-h'])
    assert @cmd.handles?(['-h', 'command'])
    assert @cmd.handles?(['--help', 'command'])
    assert @cmd.handles?(['-f', 'filename'])
    assert @cmd.handles?(['--file=filename'])
    refute @cmd.handles?(['-z'])
    refute @cmd.handles?(['-f'])
    refute @cmd.handles?(['--toothpaste'])

    args = ['-h', 'command']
    @cmd.handles?(args)
    assert_equal ['-h', 'command'], args
  end

end

