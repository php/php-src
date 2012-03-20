require 'rubygems/test_case'
require 'rubygems/gem_runner'

class TestGemGemRunner < Gem::TestCase

  def test_do_configuration
    Gem.clear_paths

    temp_conf = File.join @tempdir, '.gemrc'

    other_gem_path = File.join @tempdir, 'other_gem_path'
    other_gem_home = File.join @tempdir, 'other_gem_home'

    Gem.ensure_gem_subdirectories other_gem_path
    Gem.ensure_gem_subdirectories other_gem_home

    File.open temp_conf, 'w' do |fp|
      fp.puts "gem: --commands"
      fp.puts "gemhome: #{other_gem_home}"
      fp.puts "gempath:"
      fp.puts "  - #{other_gem_path}"
      fp.puts "rdoc: --all"
    end

    gr = Gem::GemRunner.new
    gr.send :do_configuration, %W[--config-file #{temp_conf}]

    assert_equal [other_gem_home, other_gem_path], Gem.path
    assert_equal %w[--commands], Gem::Command.extra_args
    assert_equal %w[--all], Gem::DocManager.configured_args
  end

  def test_build_args__are_handled
    Gem.clear_paths

    Gem::GemRunner.new.run(%W[help -- --build_arg1 --build_arg2])

    assert_equal %w[--build_arg1 --build_arg2], Gem::Command.build_args
  end

end

