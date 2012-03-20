begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_TYPELIB)
  class TestWIN32OLE_TYPELIB < Test::Unit::TestCase
    def test_s_typelibs
      tlibs = WIN32OLE_TYPELIB.typelibs
      assert_instance_of(Array, tlibs)
      assert(tlibs.size > 0)
      tlib = tlibs.find {|t| t.name == "Microsoft Shell Controls And Automation"}
      assert(tlib)
    end

    def test_initialize
      assert_raise(ArgumentError) {
        WIN32OLE_TYPELIB.new(1,2,3,4)
      }

      assert_raise(TypeError) {
        WIN32OLE_TYPELIB.new(100)
      }

      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_instance_of(WIN32OLE_TYPELIB, tlib)

      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation", 1.0)
      assert_instance_of(WIN32OLE_TYPELIB, tlib)

      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation", 1, 0)
      assert_instance_of(WIN32OLE_TYPELIB, tlib)
      guid = tlib.guid

      tlib_by_guid = WIN32OLE_TYPELIB.new(guid, 1, 0)
      assert_instance_of(WIN32OLE_TYPELIB, tlib_by_guid)
      assert_equal("Microsoft Shell Controls And Automation" , tlib_by_guid.name)

      path = tlib.path
      tlib_by_path =  WIN32OLE_TYPELIB.new(path)
      assert_equal("Microsoft Shell Controls And Automation" , tlib_by_path.name)

      assert_raise(WIN32OLERuntimeError) {
        WIN32OLE_TYPELIB.new("Non Exist Type Library")
      }
    end

    # #Bug:3907 [ruby-dev:42338]
    def test_initialize_with_REG_EXPAND_SZ
      tlib = WIN32OLE_TYPELIB.new("Disk Management Snap-In Object Library")
      assert_instance_of(WIN32OLE_TYPELIB, tlib)
    end

    def test_guid
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal("{50A7E9B0-70EF-11D1-B75A-00A0C90564FE}", tlib.guid)
    end

    def test_name
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal("Microsoft Shell Controls And Automation", tlib.name)
      tlib = WIN32OLE_TYPELIB.new("{50A7E9B0-70EF-11D1-B75A-00A0C90564FE}")
      assert_equal("Microsoft Shell Controls And Automation", tlib.name)
    end

    def test_version
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal(1.0, tlib.version)
    end

    def test_major_version
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal(1, tlib.major_version)
    end

    def test_minor_version
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal(0, tlib.minor_version)
    end

    def test_path
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_match(/shell32\.dll$/i, tlib.path)
    end

    def test_visible?
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert(tlib.visible?)
    end

    def test_library_name
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal("Shell32", tlib.library_name)
    end

    def test_to_s
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal("Microsoft Shell Controls And Automation", tlib.to_s)
    end

    def test_ole_types
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      ole_types = tlib.ole_types
      assert_instance_of(Array, ole_types)
      assert(ole_types.size > 0)
      assert_instance_of(WIN32OLE_TYPE, ole_types[0])
    end

    def test_inspect
      tlib = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation")
      assert_equal("#<WIN32OLE_TYPELIB:Microsoft Shell Controls And Automation>", tlib.inspect)
    end

  end
end
