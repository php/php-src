begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'
require 'fileutils'

def ado_csv_installed?
  installed = false
  if defined?(WIN32OLE)
    db = nil
    begin
      db = WIN32OLE.new('ADODB.Connection')
      db.connectionString = "Driver={Microsoft Text Driver (*.txt; *.csv)};DefaultDir=.;"
      db.open
      db.close
      db = nil
      installed = true
    rescue
    end
  end
  installed
end

if defined?(WIN32OLE_VARIANT)
    class TestWIN32OLE_VARIANT_OUTARG < Test::Unit::TestCase
      module ADO
      end
      CONNSTR="Driver={Microsoft Text Driver (*.txt; *.csv)};DefaultDir=.;"
      def setup
        return if !ado_csv_installed?

        FileUtils.cp(File.dirname(__FILE__) + '/orig_data.csv', './data.csv')
        @db = WIN32OLE.new('ADODB.Connection')
        if !defined?(ADO::AdStateOpen)
          WIN32OLE.const_load(@db, ADO)
        end
        @db.connectionString = CONNSTR
        @db.open
      end

      def test_variant_ref_and_argv
        if !ado_csv_installed?
          skip("ActiveX Data Object Library not found")
        end
        sql = "INSERT INTO data.csv VALUES (5, 'E')"
        @db.execute(sql, -1)
        c = WIN32OLE::ARGV[1]
        assert_equal(1, WIN32OLE::ARGV[1])
        obj = WIN32OLE_VARIANT.new(nil, WIN32OLE::VARIANT::VT_VARIANT|WIN32OLE::VARIANT::VT_BYREF)
        assert_equal(nil, obj.value)
        @db.execute(sql , obj)
        assert_equal(1, obj.value)
        obj = WIN32OLE_VARIANT.new(-100, WIN32OLE::VARIANT::VT_VARIANT|WIN32OLE::VARIANT::VT_BYREF)
        assert_equal(-100, obj.value)
        @db.execute(sql, obj)
        assert_equal(1, obj.value)
      end

      def teardown
        return if !ado_csv_installed?
        if @db && @db.state == ADO::AdStateOpen
          @db.close
        end
        File.unlink("data.csv")
      end
    end
end
