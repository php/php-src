require 'test/unit'

begin
  require 'curses'
rescue LoadError
end

class TestCurses < Test::Unit::TestCase
  def test_version
    assert_instance_of(String, Curses::VERSION)
  end
end if defined? Curses
