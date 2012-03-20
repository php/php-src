module Memory
  keys = []
  vals = []

  case
  when File.exist?(procfile = "/proc/self/status") && (pat = /^Vm(\w+):\s+(\d+)/) =~ File.binread(procfile)
    PROC_FILE = procfile
    VM_PAT = pat
    def self.read_status
      IO.foreach(PROC_FILE, encoding: Encoding::ASCII_8BIT) do |l|
        yield($1.downcase.intern, $2.to_i * 1024) if VM_PAT =~ l
      end
    end

    read_status {|k, v| keys << k; vals << v}

  when /mswin|mingw/ =~ RUBY_PLATFORM
    require 'dl/import'
    require 'dl/types'

    module Win32
      extend DL::Importer
      dlload "kernel32.dll", "psapi.dll"
      include DL::Win32Types
      typealias "SIZE_T", "size_t"

      PROCESS_MEMORY_COUNTERS = struct [
        "DWORD  cb",
        "DWORD  PageFaultCount",
        "SIZE_T PeakWorkingSetSize",
        "SIZE_T WorkingSetSize",
        "SIZE_T QuotaPeakPagedPoolUsage",
        "SIZE_T QuotaPagedPoolUsage",
        "SIZE_T QuotaPeakNonPagedPoolUsage",
        "SIZE_T QuotaNonPagedPoolUsage",
        "SIZE_T PagefileUsage",
        "SIZE_T PeakPagefileUsage",
      ]

      typealias "PPROCESS_MEMORY_COUNTERS", "PROCESS_MEMORY_COUNTERS*"

      extern "HANDLE GetCurrentProcess()", :stdcall
      extern "BOOL GetProcessMemoryInfo(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD)", :stdcall

      module_function
      def memory_info
        size = PROCESS_MEMORY_COUNTERS.size
        data = PROCESS_MEMORY_COUNTERS.malloc
        data.cb = size
        data if GetProcessMemoryInfo(GetCurrentProcess(), data, size)
      end
    end

    keys << :peak << :size
    def self.read_status
      if info = Win32.memory_info
        yield :peak, info.PeakPagefileUsage
        yield :size, info.PagefileUsage
      end
    end
  else
    PSCMD = ["ps", "-ovsz=","-orss=", "-p"]
    PAT = /^\s*(\d+)\s+(\d+)$/

    keys << :size << :rss
    def self.read_status
      if PAT =~ IO.popen(PSCMD + [$$.to_s], "r", err: [:child, :out], &:read)
        yield :size, $1.to_i*1024
        yield :rss, $2.to_i*1024
      end
    end
  end

  Status = Struct.new(*keys)

  class Status
    def _update
      Memory.read_status do |key, val|
        self[key] = val
      end
    end
  end

  class Status
    Header = members.map {|k| k.to_s.upcase.rjust(6)}.join('')
    Format = "%6d"

    def initialize
      _update
    end
  end
end
