#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

require 'test/unit'
require File.join(File.dirname(__FILE__), 'setup_variant')

class TC_JSONFixtures < Test::Unit::TestCase
  def setup
    fixtures = File.join(File.dirname(__FILE__), 'fixtures/*.json')
    passed, failed = Dir[fixtures].partition { |f| f['pass'] }
    @passed = passed.inject([]) { |a, f| a << [ f, File.read(f) ] }.sort
    @failed = failed.inject([]) { |a, f| a << [ f, File.read(f) ] }.sort
  end

  def test_passing
    for name, source in @passed
      begin
        assert JSON.parse(source),
          "Did not pass for fixture '#{name}': #{source.inspect}"
      rescue => e
        warn "\nCaught #{e.class}(#{e}) for fixture '#{name}': #{source.inspect}\n#{e.backtrace * "\n"}"
        raise e
      end
    end
  end

  def test_failing
    for name, source in @failed
      assert_raises(JSON::ParserError, JSON::NestingError,
        "Did not fail for fixture '#{name}': #{source.inspect}") do
        JSON.parse(source)
      end
    end
  end
end
