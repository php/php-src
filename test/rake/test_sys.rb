require File.expand_path('../helper', __FILE__)
begin
  old_verbose = $VERBOSE
  $VERBOSE = nil
  require 'rake/contrib/sys'
ensure
  $VERBOSE = old_verbose
end

class TestSys < Rake::TestCase

  def test_split_all
    assert_equal ['a'], Sys.split_all('a')
    assert_equal ['..'], Sys.split_all('..')
    assert_equal ['/'], Sys.split_all('/')
    assert_equal ['a', 'b'], Sys.split_all('a/b')
    assert_equal ['/', 'a', 'b'], Sys.split_all('/a/b')
    assert_equal ['..', 'a', 'b'], Sys.split_all('../a/b')
  end
end
