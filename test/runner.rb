require 'rbconfig'

require 'test/unit'

src_testdir = File.dirname(File.expand_path(__FILE__))
$LOAD_PATH << src_testdir
module Gem
end
class Gem::TestCase < MiniTest::Unit::TestCase
  @@project_dir = File.dirname($LOAD_PATH.last)
end

require_relative 'profile_test_all' if ENV['RUBY_TEST_ALL_PROFILE'] == 'true'

exit Test::Unit::AutoRunner.run(true, src_testdir)
