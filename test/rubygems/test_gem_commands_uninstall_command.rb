require 'rubygems/installer_test_case'
require 'rubygems/commands/uninstall_command'

class TestGemCommandsUninstallCommand < Gem::InstallerTestCase

  def setup
    super

    build_rake_in do
      use_ui @ui do
        @installer.install
      end
    end

    @cmd = Gem::Commands::UninstallCommand.new
    @cmd.options[:executables] = true
    @executable = File.join(@gemhome, 'bin', 'executable')
  end

  def test_execute_mulitple
    @other = quick_gem 'c'
    util_make_exec @other
    util_build_gem @other

    @other_installer = util_installer @other, @gemhome

    ui = Gem::MockGemUi.new
    util_setup_gem ui

    build_rake_in do
      use_ui ui do
        @other_installer.install
      end
    end

    @cmd.options[:args] = [@spec.name, @other.name]

    use_ui @ui do
      @cmd.execute
    end

    output = @ui.output.split "\n"

    assert_includes output, "Successfully uninstalled #{@spec.full_name}"
    assert_includes output, "Successfully uninstalled #{@other.full_name}"
  end

  def test_execute_mulitple_nonexistent
    @cmd.options[:args] = %w[x y]

    use_ui @ui do
      @cmd.execute
    end

    output = @ui.output.split "\n"

    assert_includes output, 'INFO:  gem "x" is not installed'
    assert_includes output, 'INFO:  gem "y" is not installed'
  end

  def test_execute_removes_executable
    ui = Gem::MockGemUi.new
    util_setup_gem ui

    build_rake_in do
      use_ui ui do
        @installer.install
      end
    end

    if win_platform? then
      assert File.exist?(@executable)
    else
      assert File.symlink?(@executable)
    end

    # Evil hack to prevent false removal success
    FileUtils.rm_f @executable

    open @executable, "wb+" do |f| f.puts "binary" end

    @cmd.options[:args] = [@spec.name]
    use_ui @ui do
      @cmd.execute
    end

    output = @ui.output.split "\n"
    assert_match(/Removing executable/, output.shift)
    assert_match(/Successfully uninstalled/, output.shift)
    assert_equal false, File.exist?(@executable)
    assert_nil output.shift, "UI output should have contained only two lines"
  end

  def test_execute_removes_formatted_executable
    FileUtils.rm_f @executable # Wish this didn't happen in #setup

    Gem::Installer.exec_format = 'foo-%s-bar'

    @installer.format_executable = true
    @installer.install

    formatted_executable = File.join @gemhome, 'bin', 'foo-executable-bar'
    assert_equal true, File.exist?(formatted_executable)

    @cmd.options[:format_executable] = true
    @cmd.execute

    assert_equal false, File.exist?(formatted_executable)
  rescue
    Gem::Installer.exec_format = nil
  end

  def test_execute_prerelease
    @spec = quick_spec "pre", "2.b"
    @gem = File.join @tempdir, @spec.file_name
    FileUtils.touch @gem

    util_setup_gem

    build_rake_in do
      use_ui @ui do
        @installer.install
      end
    end

    @cmd.options[:args] = ["pre"]

    use_ui @ui do
      @cmd.execute
    end

    output = @ui.output
    assert_match(/Successfully uninstalled/, output)
  end

end

