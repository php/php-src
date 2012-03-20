require_relative 'test_base'
require 'dl/import'
require 'dl/types'

class DL::TestWin32 < DL::TestBase
module Win32API
  extend DL::Importer

  dlload "kernel32.dll"

  include DL::Win32Types

  OSVERSIONINFO = struct [
    "DWORD dwOSVersionInfoSize",
    "DWORD dwMajorVersion",
    "DWORD dwMinorVersion",
    "DWORD dwBuildNumber",
    "DWORD dwPlatformId",
    "UCHAR szCSDVersion[128]",
  ]

  typealias "POSVERSIONINFO", "OSVERSIONINFO*"

  extern "BOOL GetVersionEx(POSVERSIONINFO)", :stdcall

  def get_version_ex()
    ptr = OSVERSIONINFO.malloc()
    ptr.dwOSVersionInfoSize = OSVERSIONINFO.size
    ret = GetVersionEx(ptr)
    if( ret )
      ptr
    else
      nil
    end
  end
  module_function :get_version_ex
rescue DL::DLError
end

if defined?(Win32API::OSVERSIONINFO)
  def test_version()
    platform = Win32API.get_version_ex().dwPlatformId
    case ENV['OS']
    when 'Windows_NT'
      expect = 2
    when /Windows.+/
      expect = 1
    else
      expect = 0
    end
    assert_equal(expect, platform)
  end
end
end
