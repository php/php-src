begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_TYPE)
  class TestWIN32OLE_TYPE < Test::Unit::TestCase

    def test_s_progids
      progids = WIN32OLE_TYPE.progids
      assert_instance_of(Array, progids)
      assert(progids.size > 0)
      assert_instance_of(String, progids[0])
      assert(progids.include?("Shell.Application.1"))
    end

    def test_initialize
      assert_raise(ArgumentError) {
        WIN32OLE_TYPE.new
      }
      assert_raise(ArgumentError) {
        WIN32OLE_TYPE.new("foo")
      }
      assert_raise(TypeError) {
        WIN32OLE_TYPE.new(1, 2)
      }
      assert_raise(TypeError) {
        WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", 1)
      }
      assert_raise(WIN32OLERuntimeError) {
        WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "foo")
      }
      assert_raise(WIN32OLERuntimeError) {
        WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "Application")
      }
      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "Shell")
      assert_instance_of(WIN32OLE_TYPE, ole_type)
      assert_equal("Shell", ole_type.name)
      assert_equal("Class", ole_type.ole_type)
      assert_equal("{13709620-C279-11CE-A49E-444553540000}", ole_type.guid)
      assert_equal("Shell.Application.1", ole_type.progid)
      assert_equal(true, ole_type.visible?)
      assert_equal("Shell", ole_type.to_s)
      assert_equal(0, ole_type.major_version)
      assert_equal(0, ole_type.minor_version)
      assert_equal(5, ole_type.typekind)
      assert_equal("Shell Object Type Information", ole_type.helpstring)
      assert_equal(nil, ole_type.src_type)
      assert_equal("", ole_type.helpfile)
      assert_equal(0, ole_type.helpcontext)
      assert_equal([], ole_type.variables)
      assert(ole_type.ole_methods.select{|m|/NameSpace/i =~ m.name}.size > 0)

      ole_type2 = WIN32OLE_TYPE.new("{13709620-C279-11CE-A49E-444553540000}", "Shell")
      assert_instance_of(WIN32OLE_TYPE, ole_type)
      assert_equal(ole_type.name, ole_type2.name)
      assert_equal(ole_type.ole_type, ole_type2.ole_type)
      assert_equal(ole_type.guid, ole_type2.guid)
      assert_equal(ole_type.progid, ole_type2.progid)
      assert_equal(ole_type.visible?, ole_type2.visible?)
      assert_equal(ole_type.to_s, ole_type2.to_s)
      assert_equal(ole_type.major_version, ole_type2.major_version)
      assert_equal(ole_type.minor_version, ole_type2.minor_version)
      assert_equal(ole_type.typekind, ole_type2.typekind)
      assert_equal(ole_type.helpstring, ole_type2.helpstring)
      assert_equal(ole_type.src_type, ole_type2.src_type)
      assert_equal(ole_type.helpfile, ole_type2.helpfile)
      assert_equal(ole_type.helpcontext, ole_type2.helpcontext)
      assert_equal(ole_type.variables.size, ole_type2.variables.size)
      assert_equal(ole_type.ole_methods[0].name, ole_type2.ole_methods[0].name)
      assert_equal(ole_type.ole_typelib.name, ole_type2.ole_typelib.name)
      assert_equal(ole_type.implemented_ole_types.size, ole_type2.implemented_ole_types.size)
      assert_equal(ole_type.inspect, ole_type2.inspect)
    end

    def setup
      @ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "Shell")
    end

    def test_name
      assert_equal("Shell", @ole_type.name)
    end

    def test_ole_type
      assert_equal("Class", @ole_type.ole_type)
    end

    def test_guid
      assert_equal("{13709620-C279-11CE-A49E-444553540000}", @ole_type.guid)
    end

    def test_progid
      assert_equal("Shell.Application.1", @ole_type.progid)
    end

    def test_visible?
      assert(@ole_type.visible?)
      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "IShellDispatch")
      assert(!ole_type.visible?)
    end

    def test_to_s
      assert_equal(@ole_type.to_s, @ole_type.name)
    end

    def test_major_version
      assert_equal(0, @ole_type.major_version)
      # ole_type = WIN32OLE_TYPE.new("Microsoft Word 11.0 Object Library", "Documents")
      # assert_equal(8, ole_type.major_version)
    end

    def test_minor_version
      assert_equal(0, @ole_type.minor_version)
      # ole_type = WIN32OLE_TYPE.new("Microsoft Word 11.0 Object Library", "Documents")
      # assert_equal(3, ole_type.minor_version)
    end

    def test_typekind
      assert_equal(5, @ole_type.typekind)
    end

    def test_helpstring
      assert_equal("Shell Object Type Information", @ole_type.helpstring)
    end

    def test_src_type
      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "DriveTypeConst")
      assert_match(/__MIDL___MIDL_itf_scrrun_/, ole_type.src_type)
      assert_equal(nil, @ole_type.src_type)
    end

    def test_helpfile
      assert_equal("", @ole_type.helpfile)
      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "Folders")
      assert_match(/VBENLR98\.CHM$/i, ole_type.helpfile)
    end

    def test_helpcontext
      assert_equal(0, @ole_type.helpcontext)
      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "Folders")
      assert_equal(2181929, ole_type.helpcontext)
    end

    def test_variables
      variables = @ole_type.variables
      assert_instance_of(Array, variables)
      assert(variables.size == 0)

      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "ShellSpecialFolderConstants")
      variables = ole_type.variables
      assert_instance_of(Array, variables)
      assert(variables.size > 0)

      assert_instance_of(WIN32OLE_VARIABLE, variables[0])
    end

    def test_ole_methods
      methods = @ole_type.ole_methods
      assert_instance_of(Array, methods)
      assert(methods.size > 0)
      assert_instance_of(WIN32OLE_METHOD, methods[0]);
      assert(methods.collect{|m| m.name}.include?("Application"))
    end

    def test_ole_typelib
      tlib = @ole_type.ole_typelib
      assert_instance_of(WIN32OLE_TYPELIB, tlib)
      assert_equal("Microsoft Shell Controls And Automation", tlib.name)
    end

    def test_implemented_ole_types
      ole_types = @ole_type.implemented_ole_types
      assert_instance_of(Array, ole_types)
      assert_equal(1, ole_types.size)
      assert_match(/^IShellDispatch5{0,1}$/, ole_types[0].name)

      ie_otype = WIN32OLE_TYPE.new("Microsoft Internet Controls", "InternetExplorer")
      ole_types = ie_otype.implemented_ole_types
      assert_equal(4, ole_types.size)
      otype = ole_types.select{|t| t.name == "IWebBrowser2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "IWebBrowserApp"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents"}
      assert_equal(1, otype.size)
    end

    def test_default_ole_types
      ie_otype = WIN32OLE_TYPE.new("Microsoft Internet Controls", "InternetExplorer")
      ole_types = ie_otype.default_ole_types
      otype = ole_types.select{|t| t.name == "IWebBrowser2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "IWebBrowserApp"}
      assert_equal(0, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents"}
      assert_equal(0, otype.size)
    end

    def test_source_ole_types
      ie_otype = WIN32OLE_TYPE.new("Microsoft Internet Controls", "InternetExplorer")
      ole_types = ie_otype.source_ole_types
      otype = ole_types.select{|t| t.name == "IWebBrowser2"}
      assert_equal(0, otype.size)
      otype = ole_types.select{|t| t.name == "IWebBrowserApp"}
      assert_equal(0, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents"}
      assert_equal(1, otype.size)
    end

    def test_default_event_sources
      ie_otype = WIN32OLE_TYPE.new("Microsoft Internet Controls", "InternetExplorer")
      ole_types = ie_otype.default_event_sources
      otype = ole_types.select{|t| t.name == "IWebBrowser2"}
      assert_equal(0, otype.size)
      otype = ole_types.select{|t| t.name == "IWebBrowserApp"}
      assert_equal(0, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents2"}
      assert_equal(1, otype.size)
      otype = ole_types.select{|t| t.name == "DWebBrowserEvents"}
      assert_equal(0, otype.size)
    end

    def test_inspect
      assert_equal("#<WIN32OLE_TYPE:Shell>", @ole_type.inspect)
    end
    # WIN32OLE_TYPE.typelibs will be obsoleted.
    def test_s_typelibs
      tlibs = WIN32OLE_TYPE.typelibs.sort
      tlibs2 = WIN32OLE_TYPELIB.typelibs.collect{|t|t.name}.sort
      assert_equal(tlibs2, tlibs)
    end

    # WIN32OLE_TYPE.ole_classes will be obsoleted.
    def test_s_ole_classes
      ots1 = WIN32OLE_TYPE.ole_classes("Microsoft Shell Controls And Automation")
      ots2 = WIN32OLE_TYPELIB.new("Microsoft Shell Controls And Automation").ole_types
      otns1 = ots1.collect{|t| t.name}.sort
      otns2 = ots2.collect{|t| t.name}.sort
      assert_equal(otns2, otns1)
    end
  end
end
