require 'rubygems/installer_test_case'
require 'rubygems/uninstaller'

class TestGemUninstaller < Gem::InstallerTestCase

  def setup
    super

    @user_spec.executables = ["executable"]

    build_rake_in do
      use_ui ui do
        @installer.install
        @user_installer.install

        Gem.use_paths @gemhome, Gem.user_dir

        @spec      = Gem::Specification.find_by_name 'a'
        @user_spec = Gem::Specification.find_by_name 'b'
      end
    end
  end

  def test_initialize_expand_path
    uninstaller = Gem::Uninstaller.new nil, :install_dir => '/foo//bar'

    assert_match %r|/foo/bar$|, uninstaller.instance_variable_get(:@gem_home)
  end

  def test_remove_all
    uninstaller = Gem::Uninstaller.new nil

    ui = Gem::MockGemUi.new "y\n"

    use_ui ui do
      uninstaller.remove_all [@spec]
    end

    refute_path_exists @spec.gem_dir
  end

  def test_remove_executables_force_keep
    uninstaller = Gem::Uninstaller.new nil, :executables => false

    executable = File.join Gem.user_dir, 'bin', 'executable'
    assert File.exist?(executable), 'executable not written'

    use_ui @ui do
      uninstaller.remove_executables @user_spec
    end

    assert File.exist? executable

    assert_equal "Executables and scripts will remain installed.\n", @ui.output
  end

  def test_remove_executables_force_remove
    uninstaller = Gem::Uninstaller.new nil, :executables => true

    executable = File.join Gem.user_dir, 'bin', 'executable'
    assert File.exist?(executable), 'executable not written'

    use_ui @ui do
      uninstaller.remove_executables @user_spec
    end

    assert_equal "Removing executable\n", @ui.output

    refute File.exist? executable
  end

  def test_remove_executables_user
    uninstaller = Gem::Uninstaller.new nil, :executables => true

    use_ui @ui do
      uninstaller.remove_executables @user_spec
    end

    exec_path = File.join Gem.user_dir, 'bin', 'executable'
    assert_equal false, File.exist?(exec_path), 'removed exec from bin dir'

    assert_equal "Removing executable\n", @ui.output
  end

  def test_remove_executables_user_format
    Gem::Installer.exec_format = 'foo-%s-bar'

    uninstaller = Gem::Uninstaller.new nil, :executables => true, :format_executable => true

    use_ui @ui do
      uninstaller.remove_executables @user_spec
    end

    exec_path = File.join Gem.user_dir, 'bin', 'foo-executable-bar'
    assert_equal false, File.exist?(exec_path), 'removed exec from bin dir'

    assert_equal "Removing foo-executable-bar\n", @ui.output
  ensure
    Gem::Installer.exec_format = nil
  end

  def test_remove_executables_user_format_disabled
    Gem::Installer.exec_format = 'foo-%s-bar'

    uninstaller = Gem::Uninstaller.new nil, :executables => true

    use_ui @ui do
      uninstaller.remove_executables @user_spec
    end

    exec_path = File.join Gem.user_dir, 'bin', 'executable'
    refute File.exist?(exec_path), 'removed exec from bin dir'

    assert_equal "Removing executable\n", @ui.output
  ensure
    Gem::Installer.exec_format = nil
  end

  def test_path_ok_eh
    uninstaller = Gem::Uninstaller.new nil

    assert_equal true, uninstaller.path_ok?(@gemhome, @spec)
  end

  def test_path_ok_eh_legacy
    uninstaller = Gem::Uninstaller.new nil

    @spec.loaded_from = @spec.loaded_from.gsub @spec.full_name, '\&-legacy'
    @spec.platform = 'legacy'

    assert_equal true, uninstaller.path_ok?(@gemhome, @spec)
  end

  def test_path_ok_eh_user
    uninstaller = Gem::Uninstaller.new nil

    assert_equal true, uninstaller.path_ok?(Gem.user_dir, @user_spec)
  end

  def test_uninstall
    uninstaller = Gem::Uninstaller.new @spec.name, :executables => true

    gem_dir = File.join @gemhome, 'gems', @spec.full_name

    Gem.pre_uninstall do
      assert File.exist?(gem_dir), 'gem_dir should exist'
    end

    Gem.post_uninstall do
      refute File.exist?(gem_dir), 'gem_dir should not exist'
    end

    uninstaller.uninstall

    refute File.exist?(gem_dir)

    assert_same uninstaller, @pre_uninstall_hook_arg
    assert_same uninstaller, @post_uninstall_hook_arg
  end

  def test_uninstall_nonexistent
    uninstaller = Gem::Uninstaller.new 'bogus', :executables => true

    e = assert_raises Gem::InstallError do
      uninstaller.uninstall
    end

    assert_equal 'gem "bogus" is not installed', e.message
  end

  def test_uninstall_not_ok
    quick_gem 'z' do |s|
      s.add_runtime_dependency @spec.name
    end

    uninstaller = Gem::Uninstaller.new @spec.name

    gem_dir = File.join @gemhome, 'gems', @spec.full_name
    executable = File.join @gemhome, 'bin', 'executable'

    assert File.exist?(gem_dir),    'gem_dir must exist'
    assert File.exist?(executable), 'executable must exist'

    ui = Gem::MockGemUi.new "n\n"

    assert_raises Gem::DependencyRemovalException do
      use_ui ui do
        uninstaller.uninstall
      end
    end

    assert File.exist?(gem_dir),    'gem_dir must still exist'
    assert File.exist?(executable), 'executable must still exist'
  end

  def test_uninstall_user
    @user_spec = Gem::Specification.find_by_name 'b'

    uninstaller = Gem::Uninstaller.new(@user_spec.name,
                                       :executables  => true,
                                       :user_install => true)

    gem_dir = File.join Gem.user_dir, 'gems', @user_spec.full_name

    Gem.pre_uninstall do
      assert_path_exists gem_dir
    end

    Gem.post_uninstall do
      refute_path_exists gem_dir
    end

    uninstaller.uninstall

    refute_path_exists gem_dir

    assert_same uninstaller, @pre_uninstall_hook_arg
    assert_same uninstaller, @post_uninstall_hook_arg
  end

  def test_uninstall_selection_greater_than_one
    util_make_gems
    
    list = Gem::Specification.find_all_by_name('a')

    uninstaller = Gem::Uninstaller.new('a')

    use_ui Gem::MockGemUi.new("2\n") do
      uninstaller.uninstall
    end

    updated_list = Gem::Specification.find_all_by_name('a')
    assert_equal list.length - 1, updated_list.length
  end
end
