module DL
  # Adds Windows type aliases to the including class for use with
  # DL::Importer.
  #
  # The aliases added are:
  # * ATOM
  # * BOOL
  # * BYTE
  # * DWORD
  # * DWORD32
  # * DWORD64
  # * HANDLE
  # * HDC
  # * HINSTANCE
  # * HWND
  # * LPCSTR
  # * LPSTR
  # * PBYTE
  # * PDWORD
  # * PHANDLE
  # * PVOID
  # * PWORD
  # * UCHAR
  # * UINT
  # * ULONG
  # * WORD
  module Win32Types
    def included(m) # :nodoc:
      m.module_eval{
        typealias "DWORD", "unsigned long"
        typealias "PDWORD", "unsigned long *"
        typealias "DWORD32", "unsigned long"
        typealias "DWORD64", "unsigned long long"
        typealias "WORD", "unsigned short"
        typealias "PWORD", "unsigned short *"
        typealias "BOOL", "int"
        typealias "ATOM", "int"
        typealias "BYTE", "unsigned char"
        typealias "PBYTE", "unsigned char *"
        typealias "UINT", "unsigned int"
        typealias "ULONG", "unsigned long"
        typealias "UCHAR", "unsigned char"
        typealias "HANDLE", "uintptr_t"
        typealias "PHANDLE", "void*"
        typealias "PVOID", "void*"
        typealias "LPCSTR", "char*"
        typealias "LPSTR", "char*"
        typealias "HINSTANCE", "unsigned int"
        typealias "HDC", "unsigned int"
        typealias "HWND", "unsigned int"
      }
    end
    module_function :included
  end

  # Adds basic type aliases to the including class for use with DL::Importer.
  #
  # The aliases added are +uint+ and +u_int+ (<tt>unsigned int</tt>) and
  # +ulong+ and +u_long+ (<tt>unsigned long</tt>)
  module BasicTypes
    def included(m) # :nodoc:
      m.module_eval{
        typealias "uint", "unsigned int"
        typealias "u_int", "unsigned int"
        typealias "ulong", "unsigned long"
        typealias "u_long", "unsigned long"
      }
    end
    module_function :included
  end
end
