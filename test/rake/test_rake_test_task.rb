require File.expand_path('../helper', __FILE__)
require 'rake/testtask'

class TestRakeTestTask < Rake::TestCase
  include Rake

  def test_initialize
    tt = Rake::TestTask.new do |t| end
    refute_nil tt
    assert_equal :test, tt.name
    assert_equal ['lib'], tt.libs
    assert_equal 'test/test*.rb', tt.pattern
    assert_equal false, tt.verbose
    assert Task.task_defined?(:test)
  end

  def test_initialize_override
    tt = Rake::TestTask.new(:example) do |t|
      t.libs = ['src', 'ext']
      t.pattern = 'test/tc_*.rb'
      t.verbose = true
    end
    refute_nil tt
    assert_equal :example, tt.name
    assert_equal ['src', 'ext'], tt.libs
    assert_equal 'test/tc_*.rb', tt.pattern
    assert_equal true, tt.verbose
    assert Task.task_defined?(:example)
  end

  def test_file_list_ENV_TEST
    ENV['TEST'] = 'testfile.rb'
    tt = Rake::TestTask.new do |t|
      t.pattern = '*'
    end

    assert_equal ["testfile.rb"], tt.file_list.to_a
  ensure
    ENV.delete 'TEST'
  end

  def test_libs_equals
    test_task = Rake::TestTask.new do |t|
      t.libs << ["A", "B"]
    end

    path = %w[lib A B].join File::PATH_SEPARATOR

    assert_equal "-I\"#{path}\"", test_task.ruby_opts_string
  end

  def test_libs_equals_empty
    test_task = Rake::TestTask.new do |t|
      t.libs = []
    end

    assert_equal '', test_task.ruby_opts_string
  end

  def test_pattern_equals
    tt = Rake::TestTask.new do |t|
      t.pattern = '*.rb'
    end
    assert_equal ['*.rb'], tt.file_list.to_a
  end

  def test_pattern_equals_test_files_equals
    tt = Rake::TestTask.new do |t|
      t.test_files = FileList['a.rb', 'b.rb']
      t.pattern = '*.rb'
    end
    assert_equal ['a.rb', 'b.rb', '*.rb'], tt.file_list.to_a
  end

  def test_run_code_direct
    test_task = Rake::TestTask.new do |t|
      t.loader = :direct
    end

    assert_equal '-e "ARGV.each{|f| require f}"', test_task.run_code
  end

  def test_run_code_rake
    test_task = Rake::TestTask.new do |t|
      t.loader = :rake
    end

    assert_match(/-I".*?" ".*?"/, test_task.run_code)
  end

  def test_run_code_testrb_ruby_1_8_2
    test_task = Rake::TestTask.new do |t|
      t.loader = :testrb
    end

    def test_task.ruby_version() '1.8.2' end

    assert_match(/^-S testrb +".*"$/, test_task.run_code)
  end

  def test_run_code_testrb_ruby_1_8_6
    test_task = Rake::TestTask.new do |t|
      t.loader = :testrb
    end

    def test_task.ruby_version() '1.8.6' end

    assert_match(/^-S testrb +$/, test_task.run_code)
  end

  def test_test_files_equals
    tt = Rake::TestTask.new do |t|
      t.test_files = FileList['a.rb', 'b.rb']
    end

    assert_equal ["a.rb", 'b.rb'], tt.file_list.to_a
  end

end

