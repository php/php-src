begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'

if defined?(WIN32OLE)
  class TestThread < Test::Unit::TestCase
    #
    # test for Bug #2618(ruby-core:27634)
    #
    def test_creating_win32ole_object_in_thread
      t = Thread.new do
        dict = WIN32OLE.new('Scripting.Dictionary')
        assert(true)
      end
      t.join
    end
  end
end
