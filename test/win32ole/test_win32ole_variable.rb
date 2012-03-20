begin
  require 'win32ole'
rescue LoadError
end
require "test/unit"

if defined?(WIN32OLE_VARIABLE)
  class TestWIN32OLE_VARIABLE < Test::Unit::TestCase

    def setup
      ole_type = WIN32OLE_TYPE.new("Microsoft Shell Controls And Automation", "ShellSpecialFolderConstants")
      @var1 = ole_type.variables.find {|v| v.name == 'ssfDESKTOP'}

      variables = WIN32OLE_TYPE.new("Microsoft Windows Installer Object Library", "Installer").variables
      @var2 = variables.find {|v| v.name == 'UILevel'}
    end

    def test_name
      assert_equal('ssfDESKTOP', @var1.name)
    end

    def test_ole_type
      assert_equal('INT', @var1.ole_type)
      assert_equal('MsiUILevel', @var2.ole_type)
    end

    def test_ole_type_detail
      assert_equal(['INT'], @var1.ole_type_detail)
      assert_equal(['USERDEFINED', 'MsiUILevel'], @var2.ole_type_detail)
    end

    def test_ole_type_value
      assert_equal(0, @var1.value)
      assert_equal(nil, @var2.value)
    end

    def test_ole_type_visible?
      assert(@var1.visible?)
    end

    def test_ole_type_variable_kind
      assert_equal("CONSTANT", @var1.variable_kind)
      assert_equal("DISPATCH", @var2.variable_kind)
    end

    def test_ole_type_varkind
      assert_equal(2, @var1.varkind)
      assert_equal(3, @var2.varkind)
    end

    def test_to_s
      assert_equal(@var1.name, @var1.to_s)
    end

    def test_inspect
      assert_equal("#<WIN32OLE_VARIABLE:ssfDESKTOP=0>",  @var1.inspect)
      assert_equal("#<WIN32OLE_VARIABLE:UILevel=nil>", @var2.inspect)
    end

  end
end
