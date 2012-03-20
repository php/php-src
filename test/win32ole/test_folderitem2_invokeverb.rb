#
# This script check that Win32OLE can execute InvokeVerb method of FolderItem2.
#

begin
  require 'win32ole'
rescue LoadError
end
require 'test/unit'

if defined?(WIN32OLE)
  class TestInvokeVerb < Test::Unit::TestCase
    def setup
      # make dummy file for InvokeVerb test.
      @fso = WIN32OLE.new('Scripting.FileSystemObject')
      dummy_file = @fso.GetTempName
      @cfolder = @fso.getFolder(".")
      f = @cfolder.CreateTextFile(dummy_file)
      f.close
      @dummy_path = @cfolder.path + "\\" + dummy_file

      shell=WIN32OLE.new('Shell.Application')
      @nsp = shell.NameSpace(@cfolder.path)
      @fi2 = @nsp.parseName(dummy_file)
    end

    def find_link(path)
      arlink = []
      @cfolder.files.each do |f|
        if /\.lnk$/ =~ f.path
          linkinfo = @nsp.parseName(f.name).getLink
          arlink.push f if linkinfo.path == path
        end
      end
      arlink
    end

    def test_invokeverb
      # in Windows Vista (not tested), Windows 7
      # The verb must be in English.
      # Creating Shortcut is "Link"
      links = find_link(@dummy_path)
      assert_equal(0, links.size)

      # Now create shortcut to @dummy_path
      arg = WIN32OLE_VARIANT.new("Link")
      @fi2.InvokeVerb(arg)

      # Now search shortcut to @dummy_path
      links = find_link(@dummy_path)
      assert_equal(1, links.size)
      @lpath = links[0].path
    end

    def teardown
      if @lpath
        @fso.deleteFile(@lpath)
      end
      if @dummy_path
        @fso.deleteFile(@dummy_path)
      end
    end

  end
end
