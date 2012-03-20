require 'dl'
require 'thread'

module DL
	# The mutual exclusion (Mutex) semaphore for the DL module
  SEM = Mutex.new # :nodoc:

  if DL.fiddle?
    # A Hash of callback Procs
    #
    # Uses Fiddle
    CdeclCallbackProcs = {} # :nodoc:

    # A Hash of the addresses of callback Proc
    #
    # Uses Fiddle
    CdeclCallbackAddrs = {} # :nodoc:

    # A Hash of Stdcall callback Procs
    #
    # Uses Fiddle on win32
    StdcallCallbackProcs = {} # :nodoc:

    # A Hash of the addresses of Stdcall callback Procs
    #
    # Uses Fiddle on win32
    StdcallCallbackAddrs = {} # :nodoc:
  end

  def set_callback_internal(proc_entry, addr_entry, argc, ty, abi = nil, &cbp)
    if( argc < 0 )
      raise(ArgumentError, "arity should not be less than 0.")
    end
    addr = nil

    if DL.fiddle?
      abi ||= Fiddle::Function::DEFAULT
      closure = Fiddle::Closure::BlockCaller.new(ty, [TYPE_VOIDP] * argc, abi, &cbp)
      proc_entry[closure.to_i] = closure
      addr = closure.to_i
    else
      SEM.synchronize{
        ary = proc_entry[ty]
        (0...MAX_CALLBACK).each{|n|
          idx = (n * DLSTACK_SIZE) + argc
          if( ary[idx].nil? )
            ary[idx] = cbp
            addr = addr_entry[ty][idx]
            break
          end
        }
      }
    end

    addr
  end

  def set_cdecl_callback(ty, argc, &cbp)
    set_callback_internal(CdeclCallbackProcs, CdeclCallbackAddrs, argc, ty, &cbp)
  end

  def set_stdcall_callback(ty, argc, &cbp)
    if DL.fiddle?
      set_callback_internal(StdcallCallbackProcs, StdcallCallbackAddrs, argc, ty, Fiddle::Function::STDCALL, &cbp)
    else
      set_callback_internal(StdcallCallbackProcs, StdcallCallbackAddrs, argc, ty, &cbp)
    end
  end

  def remove_callback_internal(proc_entry, addr_entry, addr, ctype = nil)
    if DL.fiddle?
      addr = addr.to_i
      return false unless proc_entry.key?(addr)
      proc_entry.delete(addr)
      true
    else
      index = nil
      if( ctype )
        addr_entry[ctype].each_with_index{|xaddr, idx|
          if( xaddr == addr )
            index = idx
          end
        }
      else
        addr_entry.each{|ty,entry|
          entry.each_with_index{|xaddr, idx|
            if( xaddr == addr )
              index = idx
            end
          }
        }
      end
      if( index and proc_entry[ctype][index] )
        proc_entry[ctype][index] = nil
        return true
      else
        return false
      end
    end
  end

  def remove_cdecl_callback(addr, ctype = nil)
    remove_callback_internal(CdeclCallbackProcs, CdeclCallbackAddrs, addr, ctype)
  end

  def remove_stdcall_callback(addr, ctype = nil)
    remove_callback_internal(StdcallCallbackProcs, StdcallCallbackAddrs, addr, ctype)
  end

  alias set_callback set_cdecl_callback
  alias remove_callback remove_cdecl_callback
end
