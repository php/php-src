# $Id$

require 'test/unit'
require 'fileutils'

class TestFileUtilsVerbose < Test::Unit::TestCase

  include FileUtils::Verbose

  FileUtils::METHODS.each do |m|
    define_method "test_singleton_visibility_#{m}" do
      assert_equal true, FileUtils::Verbose.respond_to?(m, true),
                   "FileUtils::Verbose.#{m} is not defined"
      assert_equal true, FileUtils::Verbose.respond_to?(m, false),
                   "FileUtils::Verbose.#{m} is not public"
    end

    define_method "test_visibility_#{m}" do
      assert_equal true, respond_to?(m, true),
                   "FileUtils::Verbose\##{m} is not defined"
      assert_equal true, FileUtils::Verbose.private_method_defined?(m),
                   "FileUtils::Verbose\##{m} is not private"
    end
  end

end
