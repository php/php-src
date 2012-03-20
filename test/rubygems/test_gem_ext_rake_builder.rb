require 'rubygems/test_case'
require 'rubygems/ext'

class TestGemExtRakeBuilder < Gem::TestCase
  def setup
    super

    @ext = File.join @tempdir, 'ext'
    @dest_path = File.join @tempdir, 'prefix'

    FileUtils.mkdir_p @ext
    FileUtils.mkdir_p @dest_path
  end

  def test_class_build
    File.open File.join(@ext, 'mkrf_conf.rb'), 'w' do |mkrf_conf|
      mkrf_conf.puts <<-EO_MKRF
        File.open("Rakefile","w") do |f|
          f.puts "task :default"
        end
      EO_MKRF
    end

    output = []
    realdir = nil # HACK /tmp vs. /private/tmp

    build_rake_in do
      Dir.chdir @ext do
        realdir = Dir.pwd
        Gem::Ext::RakeBuilder.build 'mkrf_conf.rb', nil, @dest_path, output
      end
    end

    output = output.join "\n"

    refute_match %r%^rake failed:%, output
    assert_match %r%^#{Regexp.escape @@ruby} mkrf_conf\.rb%, output
    assert_match %r%^#{Regexp.escape @@rake} RUBYARCHDIR=#{Regexp.escape @dest_path} RUBYLIBDIR=#{Regexp.escape @dest_path}%, output
  end

  def test_class_build_fail
    File.open File.join(@ext, 'mkrf_conf.rb'), 'w' do |mkrf_conf|
      mkrf_conf.puts <<-EO_MKRF
        File.open("Rakefile","w") do |f|
          f.puts "task :default do abort 'fail' end"
        end
        EO_MKRF
    end

    output = []

    error = assert_raises Gem::InstallError do
      build_rake_in do
        Dir.chdir @ext do
          Gem::Ext::RakeBuilder.build "mkrf_conf.rb", nil, @dest_path, output
        end
      end
    end

    assert_match %r%^rake failed:%, error.message
    assert_match %r%^#{Regexp.escape @@ruby} mkrf_conf\.rb%, error.message
    assert_match %r%^#{Regexp.escape @@rake} RUBYARCHDIR=#{Regexp.escape @dest_path} RUBYLIBDIR=#{Regexp.escape @dest_path}%, error.message
  end

end

