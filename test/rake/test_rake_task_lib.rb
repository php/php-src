require File.expand_path('../helper', __FILE__)
require 'rake/tasklib'

class TestRakeTaskLib < Rake::TestCase
  def test_paste
    tl = Rake::TaskLib.new
    assert_equal :ab, tl.paste(:a, :b)
  end
end
