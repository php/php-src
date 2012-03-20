require 'rubygems'

begin
  gem 'minitest'
rescue Gem::LoadError
end

require 'minitest/autorun'
require 'rake'
require 'tmpdir'
require File.expand_path('../file_creation', __FILE__)

begin
  require 'test/ruby/envutil'
rescue LoadError
  # for ruby trunk
end

class Rake::TestCase < MiniTest::Unit::TestCase
  include FileCreation

  include Rake::DSL

  class TaskManager
    include Rake::TaskManager
  end

  RUBY = defined?(EnvUtil) ? EnvUtil.rubybin : Gem.ruby

  def setup
    ARGV.clear

    @orig_PWD = Dir.pwd
    @orig_APPDATA      = ENV['APPDATA']
    @orig_HOME         = ENV['HOME']
    @orig_HOMEDRIVE    = ENV['HOMEDRIVE']
    @orig_HOMEPATH     = ENV['HOMEPATH']
    @orig_RAKE_COLUMNS = ENV['RAKE_COLUMNS']
    @orig_RAKE_SYSTEM  = ENV['RAKE_SYSTEM']
    @orig_RAKEOPT      = ENV['RAKEOPT']
    @orig_USERPROFILE  = ENV['USERPROFILE']
    ENV.delete 'RAKE_COLUMNS'
    ENV.delete 'RAKE_SYSTEM'
    ENV.delete 'RAKEOPT'

    tmpdir = Dir.chdir Dir.tmpdir do Dir.pwd end
    @tempdir = File.join tmpdir, "test_rake_#{$$}"

    FileUtils.mkdir_p @tempdir

    Dir.chdir @tempdir

    Rake.application = Rake::Application.new
    Rake::TaskManager.record_task_metadata = true
    RakeFileUtils.verbose_flag = false
  end

  def teardown
    Dir.chdir @orig_PWD
    FileUtils.rm_rf @tempdir

    if @orig_APPDATA then
      ENV['APPDATA'] = @orig_APPDATA
    else
      ENV.delete 'APPDATA'
    end

    ENV['HOME']         = @orig_HOME
    ENV['HOMEDRIVE']    = @orig_HOMEDRIVE
    ENV['HOMEPATH']     = @orig_HOMEPATH
    ENV['RAKE_COLUMNS'] = @orig_RAKE_COLUMNS
    ENV['RAKE_SYSTEM']  = @orig_RAKE_SYSTEM
    ENV['RAKEOPT']      = @orig_RAKEOPT
    ENV['USERPROFILE']  = @orig_USERPROFILE
  end

  def ignore_deprecations
    Rake.application.options.ignore_deprecate = true
    yield
  ensure
    Rake.application.options.ignore_deprecate = false
  end

  def rake_system_dir
    @system_dir = 'system'

    FileUtils.mkdir_p @system_dir

    open File.join(@system_dir, 'sys1.rake'), 'w' do |io|
      io << <<-SYS
task "sys1" do
  puts "SYS1"
end
      SYS
    end

    ENV['RAKE_SYSTEM'] = @system_dir
  end

  def rakefile contents
    open 'Rakefile', 'w' do |io|
      io << contents
    end
  end

  def rakefile_access
    rakefile <<-ACCESS
TOP_LEVEL_CONSTANT = 0

def a_top_level_function
end

task :default => [:work, :obj, :const]

task :work do
  begin
    a_top_level_function
    puts "GOOD:M Top level methods can be called in tasks"
  rescue NameError => ex
    puts "BAD:M  Top level methods can not be called in tasks"
  end
end

# TODO: remove `disabled_' when DeprecatedObjectDSL removed
task :obj
task :disabled_obj do
  begin
    Object.new.instance_eval { task :xyzzy }
    puts "BAD:D  Rake DSL are polluting objects"
  rescue StandardError => ex
    puts "GOOD:D Rake DSL are not polluting objects"
  end
end

task :const do
  begin
    TOP_LEVEL_CONSTANT
    puts "GOOD:C Top level constants are available in tasks"
  rescue StandardError => ex
    puts "BAD:C  Top level constants are NOT available in tasks"
  end
end
    ACCESS
  end

  def rakefile_chains
    rakefile <<-DEFAULT
task :default => "play.app"

file "play.scpt" => "base" do |t|
  cp t.prerequisites.first, t.name
end

rule ".app" => ".scpt" do |t|
  cp t.source, t.name
end

file 'base' do
  touch 'base'
end
    DEFAULT
  end

  def rakefile_comments
    rakefile <<-COMMENTS
# comment for t1
task :t1 do
end

# no comment or task because there's a blank line

task :t2 do
end

desc "override comment for t3"
# this is not the description
multitask :t3 do
end

# this is not the description
desc "override comment for t4"
file :t4 do
end
    COMMENTS
  end

  def rakefile_default
    rakefile <<-DEFAULT
if ENV['TESTTOPSCOPE']
  puts "TOPSCOPE"
end

task :default do
  puts "DEFAULT"
end

task :other => [:default] do
  puts "OTHER"
end

task :task_scope do
  if ENV['TESTTASKSCOPE']
    puts "TASKSCOPE"
  end
end
    DEFAULT
  end

  def rakefile_dryrun
    rakefile <<-DRYRUN
task :default => ["temp_main"]

file "temp_main" => [:all_apps]  do touch "temp_main" end

task :all_apps => [:one, :two]
task :one => ["temp_one"]
task :two => ["temp_two"]

file "temp_one" do |t|
  touch "temp_one"
end
file "temp_two" do |t|
  touch "temp_two"
end

task :clean do
  ["temp_one", "temp_two", "temp_main"].each do |file|
    rm_f file
  end
end
    DRYRUN

    FileUtils.touch 'temp_main'
    FileUtils.touch 'temp_two'
  end

  def rakefile_extra
    rakefile 'task :default'

    FileUtils.mkdir_p 'rakelib'

    open File.join('rakelib', 'extra.rake'), 'w' do |io|
      io << <<-EXTRA_RAKE
# Added for testing

namespace :extra do
  desc "An Extra Task"
  task :extra do
    puts "Read all about it"
  end
end
      EXTRA_RAKE
    end
  end

  def rakefile_file_creation
    rakefile <<-'FILE_CREATION'
N = 2

task :default => :run

BUILD_DIR = 'build'
task :clean do
  rm_rf 'build'
  rm_rf 'src'
end

task :run

TARGET_DIR = 'build/copies'

FileList['src/*'].each do |src|
  directory TARGET_DIR
  target = File.join TARGET_DIR, File.basename(src)
  file target => [src, TARGET_DIR] do
    cp src, target
    # sleep 3 if src !~ /foo#{N-1}$/   # I'm commenting out this sleep, it doesn't seem to do anything.
  end
  task :run => target
end

task :prep => :clean do
  mkdir_p 'src'
  N.times do |n|
    touch "src/foo#{n}"
  end
end
    FILE_CREATION
  end

  def rakefile_imports
    rakefile <<-IMPORTS
require 'rake/loaders/makefile'

task :default

task :other do
  puts "OTHER"
end

file "dynamic_deps" do |t|
  open(t.name, "w") do |f| f.puts "puts 'DYNAMIC'" end
end

import "dynamic_deps"
import "static_deps"
import "static_deps"
import "deps.mf"
puts "FIRST"
    IMPORTS

    open 'deps.mf', 'w' do |io|
      io << <<-DEPS
default: other
      DEPS
    end

    open "static_deps", "w" do |f|
      f.puts 'puts "STATIC"'
    end
  end

  def rakefile_multidesc
    rakefile <<-MULTIDESC
task :b

desc "A"
task :a

desc "B"
task :b

desc "A2"
task :a

task :c

desc "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
task :d
    MULTIDESC
  end

  def rakefile_namespace
    rakefile <<-NAMESPACE
desc "copy"
task :copy do
  puts "COPY"
end

namespace "nest" do
  desc "nest copy"
  task :copy do
    puts "NEST COPY"
  end
  task :xx => :copy
end

anon_ns = namespace do
  desc "anonymous copy task"
  task :copy do
    puts "ANON COPY"
  end
end

desc "Top level task to run the anonymous version of copy"
task :anon => anon_ns[:copy]

namespace "very" do
  namespace "nested" do
    task "run" => "rake:copy"
  end
end

namespace "a" do
  desc "Run task in the 'a' namespace"
  task "run" do
    puts "IN A"
  end
end

namespace "b" do
  desc "Run task in the 'b' namespace"
  task "run" => "a:run" do
    puts "IN B"
  end
end

namespace "file1" do
  file "xyz.rb" do
    puts "XYZ1"
  end
end

namespace "file2" do
  file "xyz.rb" do
    puts "XYZ2"
  end
end

namespace "scopedep" do
  task :prepare do
    touch "scopedep.rb"
    puts "PREPARE"
  end
  file "scopedep.rb" => [:prepare] do
    puts "SCOPEDEP"
  end
end
    NAMESPACE
  end

  def rakefile_nosearch
    FileUtils.touch 'dummy'
  end

  def rakefile_rakelib
    FileUtils.mkdir_p 'rakelib'

    Dir.chdir 'rakelib' do
      open 'test1.rb', 'w' do |io|
        io << <<-TEST1
task :default do
  puts "TEST1"
end
        TEST1
      end

      open 'test2.rake', 'w' do |io|
        io << <<-TEST1
task :default do
  puts "TEST2"
end
        TEST1
      end
    end
  end

  def rakefile_rbext
    open 'rakefile.rb', 'w' do |io|
      io << 'task :default do puts "OK" end'
    end
  end

  def rakefile_unittest
    rakefile '# Empty Rakefile for Unit Test'

    readme = File.join 'subdir', 'README'
    FileUtils.mkdir_p File.dirname readme

    FileUtils.touch readme
  end

  def rakefile_verbose
    rakefile <<-VERBOSE
task :standalone_verbose_true do
  verbose true
  sh "#{RUBY} -e '0'"
end

task :standalone_verbose_false do
  verbose false
  sh "#{RUBY} -e '0'"
end

task :inline_verbose_default do
  sh "#{RUBY} -e '0'"
end

task :inline_verbose_false do
  sh "#{RUBY} -e '0'", :verbose => false
end

task :inline_verbose_true do
  sh "#{RUBY} -e '0'", :verbose => true
end

task :block_verbose_true do
  verbose(true) do
    sh "#{RUBY} -e '0'"
  end
end

task :block_verbose_false do
  verbose(false) do
    sh "#{RUBY} -e '0'"
  end
end
    VERBOSE
  end

end

