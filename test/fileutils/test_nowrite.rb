# $Id$

require 'fileutils'
require 'test/unit'
require_relative 'clobber'

class TestFileUtilsNoWrite < Test::Unit::TestCase

  include FileUtils::NoWrite
  include TestFileUtils::Clobber

  FileUtils::METHODS.each do |m|
    define_method "test_singleton_visibility_#{m}" do
      assert_equal true, FileUtils::NoWrite.respond_to?(m, true),
                   "FileUtils::NoWrite.#{m} is not defined"
      assert_equal true, FileUtils::NoWrite.respond_to?(m, false),
                   "FileUtils::NoWrite.#{m} is not public"
    end

    define_method "test_instance_visibility_#{m}" do
      assert_equal true, respond_to?(m, true),
                   "FileUtils::NoWrite\##{m} is not defined"
      assert_equal true, FileUtils::NoWrite.private_method_defined?(m),
                   "FileUtils::NoWrite\##{m} is not private"
    end
  end

end
