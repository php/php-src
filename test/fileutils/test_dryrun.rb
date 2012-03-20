# $Id$

require 'fileutils'
require 'test/unit'
require_relative 'clobber'

class TestFileUtilsDryRun < Test::Unit::TestCase

  include FileUtils::DryRun
  include TestFileUtils::Clobber

  FileUtils::METHODS.each do |m|
    define_method "test_singleton_visibility_#{m}" do
      assert_equal true, FileUtils::DryRun.respond_to?(m, true),
                   "FileUtils::DryRun.#{m} not defined"
      assert_equal true, FileUtils::DryRun.respond_to?(m, false),
                   "FileUtils::DryRun.#{m} not public"
    end

    define_method "test_instance_visibility_#{m}" do
      assert_equal true, respond_to?(m, true),
                   "FileUtils::DryRun\##{m} is not defined"
      assert_equal true, FileUtils::DryRun.private_method_defined?(m),
                   "FileUtils::DryRun\##{m} is not private"
    end
  end

end
