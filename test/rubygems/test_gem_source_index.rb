require 'rubygems/test_case'
require 'rubygems/source_index'
require 'rubygems/config_file'
require 'rubygems/deprecate'

class TestGemSourceIndex < Gem::TestCase
  def setup
    super

    util_setup_fake_fetcher

    @source_index = Gem::Deprecate.skip_during { Gem.source_index }
  end

  def test_find_name
    Gem::Deprecate.skip_during do
      assert_equal [@a1, @a2, @a3a], @source_index.find_name('a')
      assert_equal [@a2], @source_index.find_name('a', '= 2')
      assert_equal [], @source_index.find_name('bogusstring')
      assert_equal [], @source_index.find_name('a', '= 3')

      source_index = Gem::SourceIndex.new
      source_index.add_spec @a1
      source_index.add_spec @a2

      assert_equal [@a1], source_index.find_name(@a1.name, '= 1')

      r1 = Gem::Requirement.create '= 1'
      assert_equal [@a1], source_index.find_name(@a1.name, r1)
    end
  end

  def test_find_name_empty_cache
    Gem::Deprecate.skip_during do
      empty_source_index = Gem::SourceIndex.new
      assert_equal [], empty_source_index.find_name("foo")
    end
  end

  # HACK: deprecated impl is failing tests, but I may want to port it over
  def test_latest_specs
    Gem::Deprecate.skip_during do
      p1_ruby = quick_spec 'p', '1'
      p1_platform = quick_spec 'p', '1' do |spec|
        spec.platform = Gem::Platform::CURRENT
      end

      a1_platform = quick_spec @a1.name, (@a1.version) do |s|
        s.platform = Gem::Platform.new 'x86-my_platform1'
      end

      a2_platform = quick_spec @a2.name, (@a2.version) do |s|
        s.platform = Gem::Platform.new 'x86-my_platform1'
      end

      a2_platform_other = quick_spec @a2.name, (@a2.version) do |s|
        s.platform = Gem::Platform.new 'x86-other_platform1'
      end

      a3_platform_other = quick_spec @a2.name, (@a2.version.bump) do |s|
        s.platform = Gem::Platform.new 'x86-other_platform1'
      end

      @source_index.add_spec p1_ruby
      @source_index.add_spec p1_platform
      @source_index.add_spec a1_platform
      @source_index.add_spec a2_platform
      @source_index.add_spec a2_platform_other
      @source_index.add_spec a3_platform_other

      expected = [
                  @a2.full_name,
                  a2_platform.full_name,
                  a3_platform_other.full_name,
                  @b2.full_name,
                  @c1_2.full_name,
                  @a_evil9.full_name,
                  p1_ruby.full_name,
                  p1_platform.full_name,
                  @pl1.full_name
                 ].sort

      latest_specs = @source_index.latest_specs.map { |s| s.full_name }.sort

      assert_equal expected, latest_specs
    end
  end

  def test_load_gems_in
    Gem::Deprecate.skip_during do
      spec_dir1 = File.join @gemhome, 'specifications'
      spec_dir2 = File.join @tempdir, 'gemhome2', 'specifications'

      FileUtils.rm_r spec_dir1

      FileUtils.mkdir_p spec_dir1
      FileUtils.mkdir_p spec_dir2

      a1 = quick_spec 'a', '1' do |spec| spec.author = 'author 1' end
      a2 = quick_spec 'a', '1' do |spec| spec.author = 'author 2' end

      path1 = File.join(spec_dir1, a1.spec_name)
      path2 = File.join(spec_dir2, a2.spec_name)

      File.open path1, 'w' do |fp|
        fp.write a1.to_ruby
      end

      File.open path2, 'w' do |fp|
        fp.write a2.to_ruby
      end

      @source_index.load_gems_in File.dirname(path1), File.dirname(path2)

      assert_equal a1.author, @source_index.specification(a1.full_name).author
    end
  end

  # REFACTOR: move to test_gem_commands_outdated_command.rb
  def test_outdated
    Gem::Deprecate.skip_during do
      util_setup_spec_fetcher

      assert_equal [], @source_index.outdated

      updated = quick_spec @a2.name, (@a2.version.bump)
      util_setup_spec_fetcher updated

      assert_equal [updated.name], @source_index.outdated

      updated_platform = quick_spec @a2.name, (updated.version.bump) do |s|
        s.platform = Gem::Platform.new 'x86-other_platform1'
      end

      util_setup_spec_fetcher updated, updated_platform

      assert_equal [updated_platform.name], @source_index.outdated
    end
  end

  def test_prerelease_specs_kept_in_right_place
    Gem::Deprecate.skip_during do
      gem_a1_alpha = quick_spec 'abba', '1.a'
      @source_index.add_spec gem_a1_alpha

      refute_includes @source_index.latest_specs,       gem_a1_alpha
      assert_includes @source_index.latest_specs(true), gem_a1_alpha
      assert_empty    @source_index.find_name           gem_a1_alpha.full_name
      assert_includes @source_index.prerelease_specs,   gem_a1_alpha
    end
  end

  def test_refresh_bang
    Gem::Deprecate.skip_during do
      a1_spec = File.join @gemhome, "specifications", @a1.spec_name

      FileUtils.mv a1_spec, @tempdir

      Gem::Specification.reset
      Gem.send :class_variable_set, :@@source_index, nil
      source_index = Gem.source_index

      refute_includes source_index.gems.keys.sort, @a1.full_name

      FileUtils.mv File.join(@tempdir, @a1.spec_name), a1_spec

      source_index.refresh!

      assert source_index.gems.include?(@a1.full_name)
    end
  end

  def test_remove_spec
    Gem::Deprecate.skip_during do
      si = Gem.source_index

      expected = si.gems.keys.sort

      expected.delete "a-1"
      @source_index.remove_spec 'a-1'

      assert_equal expected, si.gems.keys.sort

      expected.delete "a-3.a"
      @source_index.remove_spec 'a-3.a'

      assert_equal expected, si.gems.keys.sort
    end
  end

  def test_search
    Gem::Deprecate.skip_during do
      requirement = Gem::Requirement.create '= 9'
      with_version = Gem::Dependency.new(/^a/, requirement)
      assert_equal [@a_evil9], @source_index.search(with_version)

      with_default = Gem::Dependency.new(/^a/, Gem::Requirement.default)
      assert_equal [@a1, @a2, @a3a, @a_evil9], @source_index.search(with_default)

      c1_1_dep = Gem::Dependency.new 'c', '~> 1.1'
      assert_equal [@c1_2], @source_index.search(c1_1_dep)
    end
  end

  def test_search_platform
    Gem::Deprecate.skip_during do
      util_set_arch 'x86-my_platform1'

      a1 = quick_spec 'a', '1'
      a1_mine = quick_spec 'a', '1' do |s|
        s.platform = Gem::Platform.new 'x86-my_platform1'
      end
      a1_other = quick_spec 'a', '1' do |s|
        s.platform = Gem::Platform.new 'x86-other_platform1'
      end

      si = Gem::SourceIndex.new
      si.add_specs a1, a1_mine, a1_other

      dep = Gem::Dependency.new 'a', Gem::Requirement.new('1')

      gems = si.search dep, true

      assert_equal [a1, a1_mine], gems.sort
    end
  end

  def test_signature
    Gem::Deprecate.skip_during do
      sig = @source_index.gem_signature('foo-1.2.3')
      assert_equal 64, sig.length
      assert_match(/^[a-f0-9]{64}$/, sig)
    end
  end

  def test_specification
    Gem::Deprecate.skip_during do
      assert_equal @a1, @source_index.specification(@a1.full_name)

      assert_nil @source_index.specification("foo-1.2.4")
    end
  end

  def test_index_signature
    Gem::Deprecate.skip_during do
      sig = @source_index.index_signature
      assert_match(/^[a-f0-9]{64}$/, sig)
    end
  end
end
