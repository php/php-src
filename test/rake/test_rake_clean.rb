require File.expand_path('../helper', __FILE__)
require 'rake/clean'

class TestRakeClean < Rake::TestCase
  include Rake
  def test_clean
    load 'rake/clean.rb', true

    assert Task['clean'], "Should define clean"
    assert Task['clobber'], "Should define clobber"
    assert Task['clobber'].prerequisites.include?("clean"),
      "Clobber should require clean"
  end
end
