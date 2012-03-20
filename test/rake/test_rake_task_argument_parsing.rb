require File.expand_path('../helper', __FILE__)

class TestRakeTaskArgumentParsing < Rake::TestCase
  def setup
    super

    @app = Rake::Application.new
  end

  def test_name_only
    name, args = @app.parse_task_string("name")
    assert_equal "name", name
    assert_equal [], args
  end

  def test_empty_args
    name, args = @app.parse_task_string("name[]")
    assert_equal "name", name
    assert_equal [], args
  end

  def test_one_argument
    name, args = @app.parse_task_string("name[one]")
    assert_equal "name", name
    assert_equal ["one"], args
  end

  def test_two_arguments
    name, args = @app.parse_task_string("name[one,two]")
    assert_equal "name", name
    assert_equal ["one", "two"], args
  end

  def test_can_handle_spaces_between_args
    name, args = @app.parse_task_string("name[one, two,\tthree , \tfour]")
    assert_equal "name", name
    assert_equal ["one", "two", "three", "four"], args
  end

  def test_keeps_embedded_spaces
    name, args = @app.parse_task_string("name[a one ana, two]")
    assert_equal "name", name
    assert_equal ["a one ana", "two"], args
  end

  def test_terminal_width_using_env
    app = Rake::Application.new
    app.terminal_columns = 1234

    assert_equal 1234, app.terminal_width
  end

  def test_terminal_width_using_stty
    def @app.unix?() true end
    def @app.dynamic_width_stty() 1235 end
    def @app.dynamic_width_tput() 0 end

    assert_equal 1235, @app.terminal_width
  end

  def test_terminal_width_using_tput
    def @app.unix?() true end
    def @app.dynamic_width_stty() 0 end
    def @app.dynamic_width_tput() 1236 end

    assert_equal 1236, @app.terminal_width
  end

  def test_terminal_width_using_hardcoded_80
    def @app.unix?() false end

    assert_equal 80, @app.terminal_width
  end

  def test_terminal_width_with_failure
    def @app.unix?() raise end

    assert_equal 80, @app.terminal_width
  end

  def test_no_rakeopt
    ARGV << '--trace'
    app = Rake::Application.new
    app.init
    assert !app.options.silent
  end

  def test_rakeopt_with_blank_options
    ARGV << '--trace'
    app = Rake::Application.new
    app.init
    assert !app.options.silent
  end

  def test_rakeopt_with_silent_options
    ENV['RAKEOPT'] = '-s'
    app = Rake::Application.new

    app.init

    assert app.options.silent
  end
end
