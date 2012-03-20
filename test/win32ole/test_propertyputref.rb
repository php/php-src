require 'test/unit'
begin
  require 'win32ole'
rescue LoadError
end

if defined?(WIN32OLE)
  class TestWIN32OLE_PROPERTYPUTREF < Test::Unit::TestCase
    def setup
      begin
        @sapi = WIN32OLE.new('SAPI.SpVoice')
        @sv = @sapi.voice
      rescue WIN32OLERuntimeError
        @sapi = nil
      end
    end
    def test_sapi
      if @sapi
        new_id = @sapi.getvoices.item(0).Id
        @sapi.voice = @sapi.getvoices.item(0)
        assert_equal(new_id, @sapi.voice.Id)
      end
    end
    def teardown
      if @sapi
        @sapi.voice = @sv
      end
    end
  end
end
