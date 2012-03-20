begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_METHOD)
  class TestWIN32OLE_METHOD < Test::Unit::TestCase

    def setup
      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "Shell")
      @m_open = WIN32OLE_METHOD.new(ole_type, "open")
      @m_namespace = WIN32OLE_METHOD.new(ole_type, "namespace")
      @m_parent = WIN32OLE_METHOD.new(ole_type, "parent")
      @m_invoke = WIN32OLE_METHOD.new(ole_type, "invoke")
      @m_browse_for_folder = WIN32OLE_METHOD.new(ole_type, "BrowseForFolder")

      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "File")
      @m_file_name = WIN32OLE_METHOD.new(ole_type, "name")

      ole_type = WIN32OLE_TYPE.new("Microsoft Internet Controls", "WebBrowser")
      @m_navigate_complete = WIN32OLE_METHOD.new(ole_type, "NavigateComplete")
    end

    def test_initialize
      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "Shell")
      assert_raise(TypeError) {
        WIN32OLE_METHOD.new(1, 2)
      }
      assert_raise(ArgumentError) {
        method = WIN32OLE_METHOD.new("foo")
      }
      assert_raise(ArgumentError) {
        method = WIN32OLE_METHOD.new(ole_type)
      }
      assert_raise(WIN32OLERuntimeError) {
        method = WIN32OLE_METHOD.new(ole_type, "NonExistMethod")
      }
      assert_raise(TypeError) {
        method = WIN32OLE_METHOD.new(ole_type, 1)
      }
      method  = WIN32OLE_METHOD.new(ole_type, "Open")
      assert_instance_of(WIN32OLE_METHOD, method)
      method  = WIN32OLE_METHOD.new(ole_type, "open")
      assert_instance_of(WIN32OLE_METHOD, method)
    end

    def test_name
      assert_equal("Open", @m_open.name)
    end

    def test_return_type
      assert_equal("VOID", @m_open.return_type)
      assert_equal("Folder", @m_namespace.return_type)
    end

    def test_return_vtype
      assert_equal(24, @m_open.return_vtype)
      assert_equal(26, @m_namespace.return_vtype)
    end

    def test_return_type_detail
      assert_equal(['VOID'], @m_open.return_type_detail)
      assert_equal(['PTR', 'USERDEFINED', 'Folder'], @m_namespace.return_type_detail)
    end

    def test_invoke_kind
      assert_equal('FUNC', @m_open.invoke_kind)
      assert_equal('FUNC', @m_namespace.invoke_kind)
      assert_equal('PROPERTYGET', @m_parent.invoke_kind)
    end

    def test_invkind
      assert_equal(1, @m_namespace.invkind)
      assert_equal(2, @m_parent.invkind)
    end

    def test_visible?
      assert(@m_namespace.visible?)
      assert(!@m_invoke.visible?)
    end

    def test_event?
      assert(@m_navigate_complete.event?)
      assert(!@m_namespace.event?)
    end

    def test_event_interface
      assert_equal("DWebBrowserEvents", @m_navigate_complete.event_interface)
      assert_equal(nil, @m_namespace.event_interface)
    end

    def test_helpstring
      assert_equal("Get special folder from ShellSpecialFolderConstants", @m_namespace.helpstring)
    end

    def test_helpfile
      assert_equal("", @m_namespace.helpfile)
      assert_match(/VBENLR.*\.CHM$/i, @m_file_name.helpfile)
    end

    def test_helpcontext
      assert_equal(0, @m_namespace.helpcontext)
      assert_equal(2181996, @m_file_name.helpcontext)
    end

    def test_dispid
      assert_equal(1610743810, @m_namespace.dispid)
    end

    def is_ruby64?
      /mswin64|mingw64/ =~ RUBY_PLATFORM
    end

    def test_offset_vtbl
      exp = is_ruby64? ? 48 : 24
      assert_equal(exp, @m_invoke.offset_vtbl)
    end

    def test_size_params
      assert_equal(1, @m_open.size_params)
      assert_equal(4, @m_browse_for_folder.size_params)
    end

    def test_size_opt_params
      assert_equal(0, @m_open.size_opt_params)
      assert_equal(1, @m_browse_for_folder.size_opt_params)
    end

    def test_params
      params = @m_browse_for_folder.params
      assert_instance_of(Array, params)
      assert_equal(4, params.size)
      assert_instance_of(WIN32OLE_PARAM, params[0])
    end

    def test_to_s
      assert_equal(@m_namespace.name, @m_namespace.to_s)
    end

    def test_inspect
      assert_equal("#<WIN32OLE_METHOD:NameSpace>", @m_namespace.inspect)
    end

  end
end
