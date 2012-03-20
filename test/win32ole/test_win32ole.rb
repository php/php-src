#

begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'

if defined?(WIN32OLE)
  module CONST1
  end
  module CONST2
  end

  module TestCaseForDict
    def test_convert_bignum
      @dict1.add("a", 9999999999)
      @dict1.add("b", 999999999)
      @dict1.add("c", @dict1.item("b") * 10 + 9)
      assert_equal(9999999999, @dict1.item("a"))
      assert_equal(9999999999, @dict1.item("c"))
    end
    def test_add
      @dict1.add("a", 1000)
      assert_equal(1000, @dict1.item("a"))
    end
    def test_setproperty_equal_ended
      @dict1.compareMode = 1
      @dict1.add("one", 1)
      assert_equal(1, @dict1.item("ONE"))
      @dict2.add("one", 1)
      assert_nil(@dict2.item("ONE"))
      assert_equal(1, @dict2.item("one"))
    end
    def test_non_exist_property
      assert_raise(WIN32OLERuntimeError) {
        @dict1.unknown_property = 1
      }
    end

    def test_raise_message
      exc = assert_raise(WIN32OLERuntimeError) {
        @dict1.add
      }
      assert_match(/^\(in OLE method `add': \)/, exc.message) #`

      exc = assert_raise(WIN32OLERuntimeError) {
        @dict1._invoke(1, [], [])
      }
      assert_match(/^\(in OLE method `<dispatch id:1>': \)/, exc.message) #`

      exc = assert_raise(WIN32OLERuntimeError) {
        @dict1.compareMode = -1
      }
      assert_match(/^\(in setting property `compareMode': \)/, exc.message) #`
    end

    def test_no_method_error
      exc = assert_raise(NoMethodError) {
          @dict1.non_exist_method
      }
      assert_match(/non_exist_method/, exc.message)
    end

    def test_ole_methods
      methods = @dict1.ole_methods
      mnames = methods.collect {|m|
        m.name
      }
      assert(mnames.include?("Add"))
    end

    def test_ole_func_methods
      methods = @dict1.ole_func_methods
      mnames = methods.collect {|m|
        m.name
      }
      assert(mnames.include?("Add"))
    end

    def test_ole_put_methods
      methods = @dict1.ole_put_methods
      mnames = methods.collect {|m|
        m.name
      }
      assert(mnames.include?("CompareMode"))
    end

    def test_ole_get_methods
      methods = @dict1.ole_get_methods
      mnames = methods.collect {|m|
        m.name
      }
      assert(mnames.include?("Count"))
    end

    def test_ole_mehtod_help
      minfo = @dict1.ole_method_help("Add")
      assert_equal(2, minfo.size_params)
    end

    def test_ole_typelib
      tlib = @dict1.ole_typelib
      assert_equal("Microsoft Scripting Runtime", tlib.name);
    end

    def test_each
      @dict1.add("one", 1)
      @dict1.add("two", 2)
      i = 0
      @dict1.keys.each do |item|
        i += 1
      end
      assert_equal(2, i)
    end

    def test_bracket
      @dict1.add("foo", "FOO")
      assert_equal("FOO", @dict1.item("foo"))
      assert_equal("FOO", @dict1["foo"])
    end

    def test_bracket_equal
      @dict1.add("foo", "FOO")
      @dict1["foo"] = "BAR"
      assert_equal("BAR", @dict1["foo"])
    end

    def test_bracket_with_numkey
      @dict1.add(1, "ONE")
      @dict1.add(2, "two")
      assert_equal("ONE", @dict1[1])
      @dict1[2] = "TWO"
      assert_equal("TWO", @dict1[2])
    end

    def test_invoke_with_array
      @dict1.add("ary1", [1,2,3])
      assert_equal([1,2,3], @dict1["ary1"])

      @dict1.add("ary2", [[1,2,"a"], [3,4,"b"]])
      assert_equal([[1,2,"a"], [3,4,"b"]], @dict1["ary2"])

      @dict1.add("ary3", [[[1]]])
      assert_equal([[[1]]], @dict1["ary3"])

      @dict1.add("ary4", [[[1], [2], [3]], [[4], [5], [6]]])
      assert_equal([[[1],[2], [3]], [[4], [5], [6]]], @dict1["ary4"])
    end
  end

  class TestWin32OLE < Test::Unit::TestCase
    include TestCaseForDict
    def setup
      @dict1 = WIN32OLE.new('Scripting.Dictionary')
      @dict2 = WIN32OLE.new('Scripting.Dictionary')
    end
    def test_s_new
      assert_instance_of(WIN32OLE, @dict1)
      assert_instance_of(WIN32OLE, @dict2)
    end

    def test_s_new_exc
      assert_raise(TypeError) {
        WIN32OLE.new(1)
      }
      assert_raise(TypeError) {
        WIN32OLE.new("Scripting.Dictionary", 1)
      }
    end

    def test_s_new_DCOM
      rshell = WIN32OLE.new("Shell.Application")
      assert_instance_of(WIN32OLE, rshell)
    end

    def test_s_new_from_clsid
      shell = WIN32OLE.new("{13709620-C279-11CE-A49E-444553540000}")
      assert_instance_of(WIN32OLE, shell)
      exc = assert_raise(WIN32OLERuntimeError) {
        WIN32OLE.new("{000}")
      }
      assert_match(/unknown OLE server: `\{000\}'/, exc.message) #`
    end

    def test_s_connect
      obj = WIN32OLE.connect("winmgmts:")
      assert_instance_of(WIN32OLE, obj)
    end

    def test_s_connect_exc
      assert_raise(TypeError) {
        WIN32OLE.connect(1)
      }
    end

    def test_invoke_accept_symbol_hash_key
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      afolder = fso.getFolder(".")
      bfolder = fso.getFolder({"FolderPath" => "."})
      cfolder = fso.getFolder({:FolderPath => "."})
      assert_equal(afolder.path, bfolder.path)
      assert_equal(afolder.path, cfolder.path)
      fso = nil
    end

    def test_setproperty
      installer = WIN32OLE.new("WindowsInstaller.Installer")
      record = installer.CreateRecord(2)
      # this is the way to set property with argument in Win32OLE.
      record.setproperty( "StringData", 1, 'dddd')
      assert_equal('dddd', record.StringData(1))
    end

    def test_ole_type
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      tobj = fso.ole_type
      assert_match(/^IFileSystem/, tobj.name)
    end

    def test_ole_obj_help
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      tobj = fso.ole_obj_help
      assert_match(/^IFileSystem/, tobj.name)
    end


    def test_invoke_hash_key_non_str_sym
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      begin
        bfolder = fso.getFolder({1 => "."})
        assert(false)
      rescue TypeError
        assert(true)
      end
      fso = nil
    end

    def test_get_win32ole_object
      shell = WIN32OLE.new('Shell.Application')
      folder = shell.nameSpace(0)
      assert_instance_of(WIN32OLE, folder)
    end

    def test_invoke_accept_multi_hash_key
      shell = WIN32OLE.new('Shell.Application')
      folder = shell.nameSpace(0)
      item = folder.items.item(0)
      name = folder.getDetailsOf(item, 0)
      assert_equal(item.name, name)
      name = folder.getDetailsOf({:vItem => item, :iColumn => 0})
      assert_equal(item.name, name)
      name = folder.getDetailsOf({"vItem" => item, :iColumn => 0})
      assert_equal(item.name, name)
    end

    def test_ole_invoke_with_named_arg_last
      shell = WIN32OLE.new('Shell.Application')
      folder = shell.nameSpace(0)
      item = folder.items.item(0)
      name = folder.getDetailsOf(item, {:iColumn => 0})
      assert_equal(item.name, name)
    end

    def test__invoke
      shell=WIN32OLE.new('Shell.Application')
      assert_equal(shell.NameSpace(0).title, shell._invoke(0x60020002, [0], [WIN32OLE::VARIANT::VT_VARIANT]).title)
    end

    def test_ole_query_interface
      shell=WIN32OLE.new('Shell.Application')
      assert_raise(ArgumentError) {
        shell2 = shell.ole_query_interface
      }
      shell2 = shell.ole_query_interface('{A4C6892C-3BA9-11D2-9DEA-00C04FB16162}')
      assert_instance_of(WIN32OLE, shell2)
    end

    def test_ole_respond_to
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      assert(fso.ole_respond_to?('getFolder'))
      assert(fso.ole_respond_to?('GETFOLDER'))
      assert(fso.ole_respond_to?(:getFolder))
      assert(!fso.ole_respond_to?('XXXXX'))
      assert_raise(TypeError) {
        assert_raise(fso.ole_respond_to?(1))
      }
    end

    def test_invoke
      fso = WIN32OLE.new('Scripting.FileSystemObject')
      assert(fso.invoke(:getFolder, "."))
      assert(fso.invoke('getFolder', "."))
    end

    def test_s_const_load
      assert(!defined?(CONST1::SsfWINDOWS))
      shell=WIN32OLE.new('Shell.Application')
      WIN32OLE.const_load(shell, CONST1)
      assert_equal(36, CONST1::SsfWINDOWS)

      assert(!defined?(CONST2::SsfWINDOWS))
      WIN32OLE.const_load("Microsoft Shell Controls And Automation", CONST2)
      assert_equal(36, CONST2::SsfWINDOWS)
    end

    def test_s_create_guid
      guid = WIN32OLE.create_guid
      assert_match(/^\{[A-Z0-9]{8}\-[A-Z0-9]{4}\-[A-Z0-9]{4}\-[A-Z0-9]{4}\-[A-Z0-9]{12}/,
                   guid)
    end

    #
    # WIN32OLE.codepage is initialized according to Encoding.default_external.
    #
    # def test_s_codepage
    #   assert_equal(WIN32OLE::CP_ACP, WIN32OLE.codepage)
    # end

    def test_s_codepage_set
      cp = WIN32OLE.codepage
      WIN32OLE.codepage = WIN32OLE::CP_UTF8
      assert_equal(WIN32OLE::CP_UTF8, WIN32OLE.codepage)
      WIN32OLE.codepage = cp
    end

    def test_s_codepage_changed
      cp = WIN32OLE.codepage
      fso = WIN32OLE.new("Scripting.FileSystemObject")
      fname = fso.getTempName
      begin
        obj = WIN32OLE_VARIANT.new([0x3042].pack("U*").force_encoding("UTF-8"))
        WIN32OLE.codepage = WIN32OLE::CP_UTF8
        assert_equal("\xE3\x81\x82".force_encoding("CP65001"), obj.value)

        begin
          WIN32OLE.codepage = 932 # Windows-31J
        rescue WIN32OLERuntimeError
        end
        if (WIN32OLE.codepage == 932)
          assert_equal("\x82\xA0".force_encoding("CP932"), obj.value)
        end

        begin
          WIN32OLE.codepage = 20932 # MS EUC-JP
        rescue WIN32OLERuntimeError
        end
        if (WIN32OLE.codepage == 20932)
          assert_equal("\xA4\xA2".force_encoding("CP20932"), obj.value)
        end

        WIN32OLE.codepage = cp
        file = fso.opentextfile(fname, 2, true)
        begin
          file.write [0x3042].pack("U*").force_encoding("UTF-8")
        ensure
          file.close
        end
        str = ""
        open(fname, "r:ascii-8bit") {|ifs|
          str = ifs.read
        }
        assert_equal("\202\240", str)

        # This test fail if codepage 20932 (euc) is not installed.
        begin
          WIN32OLE.codepage = 20932
        rescue WIN32OLERuntimeError
        end
        if (WIN32OLE.codepage == 20932)
          WIN32OLE.codepage = cp
          file = fso.opentextfile(fname, 2, true)
          begin
            file.write [164, 162].pack("c*").force_encoding("EUC-JP")
          ensure
            file.close
          end
          open(fname, "r:ascii-8bit") {|ifs|
            str = ifs.read
          }
          assert_equal("\202\240", str)
        end

      ensure
        WIN32OLE.codepage = cp
        if (File.exist?(fname))
          File.unlink(fname)
        end
      end
    end

    def test_cp51932
      cp = WIN32OLE.codepage
      begin
        obj = WIN32OLE_VARIANT.new([0x3042].pack("U*").force_encoding("UTF-8"))
        begin
          WIN32OLE.codepage = 51932
        rescue
        end
        if WIN32OLE.codepage == 51932
          assert_equal("\xA4\xA2".force_encoding("CP51932"), obj.value)
        end
      ensure
        WIN32OLE.codepage = cp
      end
    end

    def test_s_locale
      assert_equal(WIN32OLE::LOCALE_SYSTEM_DEFAULT, WIN32OLE.locale)
    end

    def test_s_locale_set
      begin
        begin
          WIN32OLE.locale = 1041
        rescue WIN32OLERuntimeError
          STDERR.puts("\n#{__FILE__}:#{__LINE__}:#{self.class.name}.test_s_locale_set is skipped(Japanese locale is not installed)")
          return
        end
        assert_equal(1041, WIN32OLE.locale)
        WIN32OLE.locale = WIN32OLE::LOCALE_SYSTEM_DEFAULT
        assert_raise(WIN32OLERuntimeError) {
          WIN32OLE.locale = 111
        }
        assert_equal(WIN32OLE::LOCALE_SYSTEM_DEFAULT, WIN32OLE.locale)
      ensure
        WIN32OLE.locale = WIN32OLE::LOCALE_SYSTEM_DEFAULT
      end
    end

    def test_s_locale_change
      begin
        begin
          WIN32OLE.locale = 0x0411
        rescue WIN32OLERuntimeError
        end
        if WIN32OLE.locale == 0x0411
          obj = WIN32OLE_VARIANT.new("\\100,000", WIN32OLE::VARIANT::VT_CY)
          assert_equal("100000", obj.value)
          assert_raise(WIN32OLERuntimeError) {
            obj = WIN32OLE_VARIANT.new("$100.000", WIN32OLE::VARIANT::VT_CY)
          }
        else
          STDERR.puts("\n#{__FILE__}:#{__LINE__}:#{self.class.name}.test_s_locale_change is skipped(Japanese locale is not installed)")
        end

        begin
          WIN32OLE.locale = 1033
        rescue WIN32OLERuntimeError
        end
        if WIN32OLE.locale == 1033
          obj = WIN32OLE_VARIANT.new("$100,000", WIN32OLE::VARIANT::VT_CY)
          assert_equal("100000", obj.value)
        else
          STDERR.puts("\n#{__FILE__}:#{__LINE__}:#{self.class.name}.test_s_locale_change is skipped(US English locale is not installed)")
        end
      ensure
        WIN32OLE.locale = WIN32OLE::LOCALE_SYSTEM_DEFAULT
      end
    end

    def test_const_CP_ACP
      assert_equal(0, WIN32OLE::CP_ACP)
    end

    def test_const_CP_OEMCP
      assert_equal(1, WIN32OLE::CP_OEMCP)
    end

    def test_const_CP_MACCP
      assert_equal(2, WIN32OLE::CP_MACCP)
    end

    def test_const_CP_THREAD_ACP
      assert_equal(3, WIN32OLE::CP_THREAD_ACP)
    end

    def test_const_CP_SYMBOL
      assert_equal(42, WIN32OLE::CP_SYMBOL)
    end

    def test_const_CP_UTF7
      assert_equal(65000, WIN32OLE::CP_UTF7)
    end

    def test_const_CP_UTF8
      assert_equal(65001, WIN32OLE::CP_UTF8)
    end

    def test_const_LOCALE_SYSTEM_DEFAULT
      assert_equal(0x0800, WIN32OLE::LOCALE_SYSTEM_DEFAULT);
    end

    def test_const_LOCALE_USER_DEFAULT
      assert_equal(0x0400, WIN32OLE::LOCALE_USER_DEFAULT);
    end
  end

  # test of subclass of WIN32OLE
  class MyDict < WIN32OLE
    def MyDict.new
      super('Scripting.Dictionary')
    end
  end
  class TestMyDict < Test::Unit::TestCase
    include TestCaseForDict
    def setup
      @dict1 = MyDict.new
      @dict2 = MyDict.new
    end
    def test_s_new
      assert_instance_of(MyDict, @dict1)
      assert_instance_of(MyDict, @dict2)
    end
  end
end
