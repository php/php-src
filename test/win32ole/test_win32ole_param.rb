begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_PARAM)
  class TestWIN32OLE_PARAM < Test::Unit::TestCase

    def setup
      ole_type = WIN32OLE_TYPE.new("Microsoft Internet Controls", "WebBrowser")
      m_navigate = WIN32OLE_METHOD.new(ole_type, "Navigate")
      m_before_navigate = WIN32OLE_METHOD.new(ole_type, "BeforeNavigate")
      params = m_navigate.params
      @param_url = params[0]
      @param_flags = params[1]
      @param_cancel = m_before_navigate.params[5]

      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "ShellLinkObject")
      m_geticonlocation = WIN32OLE_METHOD.new(ole_type, "GetIconLocation")
      @param_pbs = m_geticonlocation.params[0]

      ole_type = WIN32OLE_TYPE.new("Microsoft HTML Object Library", "FontNames")
      m_count = WIN32OLE_METHOD.new(ole_type, "Count")
      @param_p = m_count.params[0]

      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "FileSystemObject")
      m_copyfile = WIN32OLE_METHOD.new(ole_type, "CopyFile")
      @param_overwritefiles = m_copyfile.params[2]
    end

    def test_s_new
      assert_raise(ArgumentError) {
        WIN32OLE_PARAM.new("hoge")
      }
      ole_type = WIN32OLE_TYPE.new("Microsoft Scripting Runtime", "FileSystemObject")
      m_copyfile = WIN32OLE_METHOD.new(ole_type, "CopyFile")
      assert_raise(IndexError) {
        WIN32OLE_PARAM.new(m_copyfile, 4);
      }
      assert_raise(IndexError) {
        WIN32OLE_PARAM.new(m_copyfile, 0);
      }
      assert_raise(IndexError) {
        WIN32OLE_PARAM.new(m_copyfile, 0);
      }
      param = WIN32OLE_PARAM.new(m_copyfile, 3)
      assert_equal("OverWriteFiles", param.name)
      assert_equal(WIN32OLE_PARAM, param.class)
      assert_equal(true, param.default)
      assert_equal("#<WIN32OLE_PARAM:OverWriteFiles=true>", param.inspect)
    end

    def test_name
      assert_equal('URL', @param_url.name)
      assert_equal('Flags', @param_flags.name)
      assert_equal('Cancel', @param_cancel.name)
    end

    def test_ole_type
      assert_equal('BSTR', @param_url.ole_type)
      assert_equal('VARIANT', @param_flags.ole_type)
    end

    def test_ole_type_detail
      assert_equal(['BSTR'], @param_url.ole_type_detail)
      assert_equal(['PTR', 'VARIANT'], @param_flags.ole_type_detail)
    end

    def test_input?
      assert(@param_url.input?)
      assert(@param_cancel.input?)
      assert(!@param_pbs.input?)
    end

    def test_output?
      assert(!@param_url.output?)
      assert(@param_cancel.output?)
      assert(@param_pbs.output?)
    end

    def test_optional?
      assert(!@param_url.optional?)
      assert(@param_flags.optional?)
    end

    def test_retval?
      assert(!@param_url.retval?)
      assert(@param_p.retval?)
    end

    def test_default
      assert_equal(nil, @param_url.default)
      assert_equal(true, @param_overwritefiles.default)
    end

    def test_to_s
      assert_equal(@param_url.name, @param_url.to_s)
    end

    def test_inspect
      assert_equal("#<WIN32OLE_PARAM:URL>", @param_url.inspect)
      assert_equal("#<WIN32OLE_PARAM:OverWriteFiles=true>", @param_overwritefiles.inspect)
    end
  end
end
