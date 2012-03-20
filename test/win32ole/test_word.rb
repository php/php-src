#
# This is test for [ruby-Bugs#3237]
#
begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

def word_installed?
  installed = false
  w = nil
  if defined?(WIN32OLE)
    begin
      w = WIN32OLE.new('Word.Application')
      installed = true
    rescue
    ensure
      if w
        w.quit
        w = nil
      end
    end
  end
  return installed
end

if defined?(WIN32OLE)
  w = nil
  dotest = word_installed?
  if !dotest
    STDERR.puts("\n#{__FILE__} skipped(Microsoft Word not found.)")
  end
  if dotest
    class TestWIN32OLE_WITH_WORD < Test::Unit::TestCase
      def setup
        begin
          @obj = WIN32OLE.new('Word.Application')
        rescue WIN32OLERuntimeError
          @obj = nil
          if !$skipped
              $skipped = true
          end
        end
      end

      def test_ole_methods
        if @obj
          @obj.visible = true
          @obj.wordbasic.disableAutoMacros(true)
          assert(true)
        else
        end
      end

      def test_s_connect
        if @obj
          obj2 = WIN32OLE.connect("Word.Application")
          assert_instance_of(WIN32OLE, obj2)
          obj2.visible = true
        end
      end

      def teardown
        if @obj
          @obj.quit
          @obj = nil
        end
      end
    end
  end
end
