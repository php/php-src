require 'rubygems/test_case'
require 'rubygems/commands/cleanup_command'

class TestGemCommandsCleanupCommand < Gem::TestCase
  
  def setup
    super

    @cmd = Gem::Commands::CleanupCommand.new

    @a_1 = quick_spec 'a', 1
    @a_2 = quick_spec 'a', 2

    install_gem @a_1
    install_gem @a_2
  end

  def test_execute
    @cmd.options[:args] = %w[a]

    @cmd.execute

    refute_path_exists @a_1.gem_dir
  end

  def test_execute_all
    @b_1 = quick_spec 'b', 1
    @b_2 = quick_spec 'b', 2

    install_gem @b_1
    install_gem @b_2

    @cmd.options[:args] = []

    @cmd.execute

    refute_path_exists @a_1.gem_dir
    refute_path_exists @b_1.gem_dir
  end

  def test_execute_all_user
    @a_1_1 = quick_spec 'a', '1.1'
    @a_1_1 = install_gem_user @a_1_1 # pick up user install path

    Gem::Specification.dirs = [Gem.dir, Gem.user_dir]

    assert_path_exists @a_1.gem_dir
    assert_path_exists @a_1_1.gem_dir

    @cmd.options[:args] = %w[a]

    @cmd.execute

    refute_path_exists @a_1.gem_dir
    refute_path_exists @a_1_1.gem_dir
  end

  def test_execute_all_user_no_sudo
    FileUtils.chmod 0555, @gemhome

    @a_1_1 = quick_spec 'a', '1.1'
    @a_1_1 = install_gem_user @a_1_1 # pick up user install path

    Gem::Specification.dirs = [Gem.dir, Gem.user_dir]

    assert_path_exists @a_1.gem_dir
    assert_path_exists @a_1_1.gem_dir

    @cmd.options[:args] = %w[a]

    @cmd.execute

    assert_path_exists @a_1.gem_dir
    refute_path_exists @a_1_1.gem_dir
  ensure
    FileUtils.chmod 0755, @gemhome
  end unless win_platform?

  def test_execute_dry_run
    @cmd.options[:args] = %w[a]
    @cmd.options[:dryrun] = true

    @cmd.execute

    assert_path_exists @a_1.gem_dir
  end

end

