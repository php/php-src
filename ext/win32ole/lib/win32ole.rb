require 'win32ole.so'

# re-define Thread#initialize
# bug #2618(ruby-core:27634)

class Thread
  alias :org_initialize :initialize
  def initialize(*arg, &block)
    if block
      org_initialize(*arg) {
        WIN32OLE.ole_initialize
        begin
          block.call(*arg)
        ensure
          WIN32OLE.ole_uninitialize
        end
      }
    else
      org_initialize(*arg)
    end
  end
end
