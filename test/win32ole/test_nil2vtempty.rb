# This is test script to check that WIN32OLE should convert nil to VT_EMPTY in second try.
# [ruby-talk:137054]
begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'

if defined?(WIN32OLE)
  class TestNIL2VT_EMPTY < Test::Unit::TestCase
    def setup
      fs = WIN32OLE.new('Scripting.FileSystemObject')
      @path = fs.GetFolder(".").path
    end
    def test_openSchema
      con = nil
      begin
        con = WIN32OLE.new('ADODB.Connection')
        con.connectionString = "Provider=MSDASQL;Extended Properties="
        con.connectionString +="\"DRIVER={Microsoft Text Driver (*.txt; *.csv)};DBQ=#{@path}\""
        con.open
      rescue
        con = nil
      end
      if con
        rs = con.openSchema(4, [nil,nil,"DUMMY", "TABLE"])
        assert(rs)
        assert_equal("_Recordset", rs.ole_type.name)

        rs = con.openSchema(4, [WIN32OLE_VARIANT::Empty, WIN32OLE_VARIANT::Empty, "DUMMY", "TABLE"])
        assert(rs)
        assert_equal("_Recordset", rs.ole_type.name)
      end
    end
  end
end
