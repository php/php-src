require 'rubygems/test_case'
require 'rubygems/commands/install_command'

begin
  gem "rdoc"
rescue Gem::LoadError
  # ignore
end

class TestGemCommandsInstallCommand < Gem::TestCase

  def setup
    super

    @cmd = Gem::Commands::InstallCommand.new
    @cmd.options[:generate_rdoc] = false
    @cmd.options[:generate_ri] = false
  end

  def test_execute_exclude_prerelease
    util_setup_fake_fetcher :prerelease
    util_setup_spec_fetcher

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      read_binary(@a2.cache_file)
    @fetcher.data["#{@gem_repo}gems/#{@a2_pre.file_name}"] =
      read_binary(@a2_pre.cache_file)

    @cmd.options[:args] = [@a2.name]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end
      assert_equal 0, e.exit_code, @ui.error
    end

    assert_match(/Successfully installed #{@a2.full_name}$/, @ui.output)
    refute_match(/Successfully installed #{@a2_pre.full_name}$/, @ui.output)
  end

  def test_execute_explicit_version_includes_prerelease
    util_setup_fake_fetcher :prerelease
    util_setup_spec_fetcher

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      read_binary(@a2.cache_file)
    @fetcher.data["#{@gem_repo}gems/#{@a2_pre.file_name}"] =
      read_binary(@a2_pre.cache_file)

    @cmd.handle_options [@a2_pre.name, '--version', @a2_pre.version.to_s,
                         "--no-ri", "--no-rdoc"]
    assert @cmd.options[:prerelease]
    assert @cmd.options[:version].satisfied_by?(@a2_pre.version)

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end
      assert_equal 0, e.exit_code, @ui.error
    end

    refute_match(/Successfully installed #{@a2.full_name}$/, @ui.output)
    assert_match(/Successfully installed #{@a2_pre.full_name}$/, @ui.output)
  end

  def test_execute_include_dependencies
    @cmd.options[:include_dependencies] = true
    @cmd.options[:args] = []

    assert_raises Gem::CommandLineError do
      use_ui @ui do
        @cmd.execute
      end
    end

    output = @ui.output.split "\n"
    assert_equal "INFO:  `gem install -y` is now default and will be removed",
                 output.shift
    assert_equal "INFO:  use --ignore-dependencies to install only the gems you list",
                 output.shift
    assert output.empty?, output.inspect
  end

  def test_execute_local
    util_setup_fake_fetcher
    @cmd.options[:domain] = :local

    FileUtils.mv @a2.cache_file, @tempdir

    @cmd.options[:args] = [@a2.name]

    use_ui @ui do
      orig_dir = Dir.pwd
      begin
        Dir.chdir @tempdir
        e = assert_raises Gem::SystemExitException do
          @cmd.execute
        end
        assert_equal 0, e.exit_code
      ensure
        Dir.chdir orig_dir
      end
    end

    out = @ui.output.split "\n"
    assert_equal "Successfully installed #{@a2.full_name}", out.shift
    assert_equal "1 gem installed", out.shift
    assert out.empty?, out.inspect
  end

  def test_no_user_install
    skip 'skipped on MS Windows (chmod has no effect)' if win_platform?

    util_setup_fake_fetcher
    @cmd.options[:user_install] = false

    FileUtils.mv @a2.cache_file, @tempdir

    @cmd.options[:args] = [@a2.name]

    use_ui @ui do
      orig_dir = Dir.pwd
      begin
        FileUtils.chmod 0755, @userhome
        FileUtils.chmod 0555, @gemhome

        Dir.chdir @tempdir
        assert_raises Gem::FilePermissionError do
          @cmd.execute
        end
      ensure
        Dir.chdir orig_dir
        FileUtils.chmod 0755, @gemhome
      end
    end
  end

  def test_execute_local_missing
    util_setup_fake_fetcher
    @cmd.options[:domain] = :local

    @cmd.options[:args] = %w[no_such_gem]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end
      assert_equal 2, e.exit_code
    end

    # HACK no repository was checked
    assert_match(/ould not find a valid gem 'no_such_gem'/, @ui.error)
  end

  def test_execute_no_gem
    @cmd.options[:args] = %w[]

    assert_raises Gem::CommandLineError do
      @cmd.execute
    end
  end

  def test_execute_nonexistent
    util_setup_fake_fetcher
    util_setup_spec_fetcher

    @cmd.options[:args] = %w[nonexistent]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end
      assert_equal 2, e.exit_code
    end

    assert_match(/ould not find a valid gem 'nonexistent'/, @ui.error)
  end

  def test_execute_nonexistent_with_hint
    misspelled = "nonexistent_with_hint"
    correctly_spelled = "non_existent_with_hint"

    util_setup_fake_fetcher
    util_setup_spec_fetcher quick_spec(correctly_spelled, '2')

    @cmd.options[:args] = [misspelled]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end

      assert_equal 2, e.exit_code
    end

    expected = "ERROR:  Could not find a valid gem 'nonexistent_with_hint' (>= 0) in any repository
ERROR:  Possible alternatives: non_existent_with_hint
"

    assert_equal expected, @ui.error
  end

  def test_execute_prerelease
    util_setup_fake_fetcher :prerelease
    util_clear_gems
    util_setup_spec_fetcher @a2, @a2_pre

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      read_binary(@a2.cache_file)
    @fetcher.data["#{@gem_repo}gems/#{@a2_pre.file_name}"] =
      read_binary(@a2_pre.cache_file)

    @cmd.options[:prerelease] = true
    @cmd.options[:args] = [@a2_pre.name]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        @cmd.execute
      end
      assert_equal 0, e.exit_code, @ui.error
    end

    refute_match(/Successfully installed #{@a2.full_name}$/, @ui.output)
    assert_match(/Successfully installed #{@a2_pre.full_name}$/, @ui.output)
  end

  def test_execute_remote
    @cmd.options[:generate_rdoc] = true
    @cmd.options[:generate_ri] = true

    util_setup_fake_fetcher
    util_setup_spec_fetcher

    @fetcher.data["#{@gem_repo}gems/#{@a2.file_name}"] =
      read_binary(@a2.cache_file)

    @cmd.options[:args] = [@a2.name]

    use_ui @ui do
      e = assert_raises Gem::SystemExitException do
        capture_io do
          @cmd.execute
        end
      end
      assert_equal 0, e.exit_code
    end

    out = @ui.output.split "\n"
    assert_equal "Successfully installed #{@a2.full_name}", out.shift
    assert_equal "1 gem installed", out.shift
    assert_equal "Installing ri documentation for #{@a2.full_name}...",
                 out.shift
    assert_equal "Installing RDoc documentation for #{@a2.full_name}...",
                 out.shift
    assert out.empty?, out.inspect
  end

  def test_execute_two
    util_setup_fake_fetcher
    @cmd.options[:domain] = :local

    FileUtils.mv @a2.cache_file, @tempdir

    FileUtils.mv @b2.cache_file, @tempdir

    @cmd.options[:args] = [@a2.name, @b2.name]

    use_ui @ui do
      orig_dir = Dir.pwd
      begin
        Dir.chdir @tempdir
        e = assert_raises Gem::SystemExitException do
          @cmd.execute
        end
        assert_equal 0, e.exit_code
      ensure
        Dir.chdir orig_dir
      end
    end

    out = @ui.output.split "\n"
    assert_equal "Successfully installed #{@a2.full_name}", out.shift
    assert_equal "Successfully installed #{@b2.full_name}", out.shift
    assert_equal "2 gems installed", out.shift
    assert out.empty?, out.inspect
  end

  def test_execute_conservative
    util_setup_fake_fetcher
    util_setup_spec_fetcher

    @fetcher.data["#{@gem_repo}gems/#{@b2.file_name}"] =
      read_binary(@b2.cache_file)

    uninstall_gem(@b2)

    @cmd.options[:conservative] = true

    @cmd.options[:args] = [@a2.name, @b2.name]

    use_ui @ui do
      orig_dir = Dir.pwd
      begin
        Dir.chdir @tempdir
        assert_raises Gem::SystemExitException do
          @cmd.execute
        end
      ensure
        Dir.chdir orig_dir
      end
    end

    out = @ui.output.split "\n"
    assert_equal "", @ui.error
    assert_equal "Successfully installed #{@b2.full_name}", out.shift
    assert_equal "1 gem installed", out.shift
    assert out.empty?, out.inspect
  end
end

