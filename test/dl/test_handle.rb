require_relative 'test_base'

module DL
  class TestHandle < TestBase
    def test_to_i
      handle = DL::Handle.new(LIBC_SO)
      assert_kind_of Integer, handle.to_i
    end

    def test_static_sym_secure
      assert_raises(SecurityError) do
        Thread.new do
          $SAFE = 2
          DL::Handle.sym('calloc')
        end.join
      end
    end

    def test_static_sym_unknown
      assert_raises(DL::DLError) { DL::Handle.sym('fooo') }
      assert_raises(DL::DLError) { DL::Handle['fooo'] }
    end

    def test_static_sym
      skip "DL::Handle.sym is not supported" if /mswin|mingw/ =~ RUBY_PLATFORM
      begin
        # Linux / Darwin / FreeBSD
       assert_not_nil DL::Handle.sym('dlopen')
       assert_equal DL::Handle.sym('dlopen'), DL::Handle['dlopen']
      rescue
        # NetBSD
        require 'objspace'
       assert_not_nil DL::Handle.sym('Init_objspace')
       assert_equal DL::Handle.sym('Init_objspace'), DL::Handle['Init_objspace']
      end
    end

    def test_sym_closed_handle
      handle = DL::Handle.new(LIBC_SO)
      handle.close
      assert_raises(DL::DLError) { handle.sym("calloc") }
      assert_raises(DL::DLError) { handle["calloc"] }
    end

    def test_sym_unknown
      handle = DL::Handle.new(LIBC_SO)
      assert_raises(DL::DLError) { handle.sym('fooo') }
      assert_raises(DL::DLError) { handle['fooo'] }
    end

    def test_sym_with_bad_args
      handle = DL::Handle.new(LIBC_SO)
      assert_raises(TypeError) { handle.sym(nil) }
      assert_raises(TypeError) { handle[nil] }
    end

    def test_sym_secure
      assert_raises(SecurityError) do
        Thread.new do
          $SAFE = 2
          handle = DL::Handle.new(LIBC_SO)
          handle.sym('calloc')
        end.join
      end
    end

    def test_sym
      handle = DL::Handle.new(LIBC_SO)
      assert_not_nil handle.sym('calloc')
      assert_not_nil handle['calloc']
    end

    def test_handle_close
      handle = DL::Handle.new(LIBC_SO)
      assert_equal 0, handle.close
    end

    def test_handle_close_twice
      handle = DL::Handle.new(LIBC_SO)
      handle.close
      assert_raises(DL::DLError) do
        handle.close
      end
    end

    def test_dlopen_returns_handle
      assert_instance_of DL::Handle, dlopen(LIBC_SO)
    end

    def test_dlopen_safe
      assert_raises(SecurityError) do
        Thread.new do
          $SAFE = 2
          dlopen(LIBC_SO)
        end.join
      end
    end

    def test_initialize_safe
      assert_raises(SecurityError) do
        Thread.new do
          $SAFE = 2
          DL::Handle.new(LIBC_SO)
        end.join
      end
    end

    def test_initialize_noargs
      handle = DL::Handle.new
      assert_not_nil handle['rb_str_new']
    end

    def test_initialize_flags
      handle = DL::Handle.new(LIBC_SO, DL::RTLD_LAZY | DL::RTLD_GLOBAL)
      assert_not_nil handle['calloc']
    end

    def test_enable_close
      handle = DL::Handle.new(LIBC_SO)
      assert !handle.close_enabled?, 'close is enabled'

      handle.enable_close
      assert handle.close_enabled?, 'close is not enabled'
    end

    def test_disable_close
      handle = DL::Handle.new(LIBC_SO)

      handle.enable_close
      assert handle.close_enabled?, 'close is enabled'
      handle.disable_close
      assert !handle.close_enabled?, 'close is enabled'
    end

    def test_NEXT
      begin
        # Linux / Darwin
        #
        # There are two special pseudo-handles, RTLD_DEFAULT and RTLD_NEXT.  The  former  will  find
        # the  first  occurrence  of the desired symbol using the default library search order.  The
        # latter will find the next occurrence of a function in the search order after  the  current
        # library.   This  allows  one  to  provide  a  wrapper  around a function in another shared
        # library.
        # --- Ubuntu Linux 8.04 dlsym(3)
        handle = DL::Handle::NEXT
        assert_not_nil handle['malloc']
      rescue
        # BSD
        #
        # If dlsym() is called with the special handle RTLD_NEXT, then the search
        # for the symbol is limited to the shared objects which were loaded after
        # the one issuing the call to dlsym().  Thus, if the function is called
        # from the main program, all the shared libraries are searched.  If it is
        # called from a shared library, all subsequent shared libraries are
        # searched.  RTLD_NEXT is useful for implementing wrappers around library
        # functions.  For example, a wrapper function getpid() could access the
        # "real" getpid() with dlsym(RTLD_NEXT, "getpid").  (Actually, the dlfunc()
        # interface, below, should be used, since getpid() is a function and not a
        # data object.)
        # --- FreeBSD 8.0 dlsym(3)
        require 'objspace'
        handle = DL::Handle::NEXT
        assert_not_nil handle['Init_objspace']
      end
    end unless /mswin|mingw/ =~ RUBY_PLATFORM

    def test_DEFAULT
      skip "DL::Handle::DEFAULT is not supported" if /mswin|mingw/ =~ RUBY_PLATFORM
      handle = DL::Handle::DEFAULT
      assert_not_nil handle['malloc']
    end unless /mswin|mingw/ =~ RUBY_PLATFORM

    def test_dlerror
      # FreeBSD (at least 7.2 to 7.2) calls nsdispatch(3) when it calls
      # getaddrinfo(3). And nsdispatch(3) doesn't call dlerror(3) even if
      # it calls _nss_cache_cycle_prevention_function with dlsym(3).
      # So our DL::Handle#sym must call dlerror(3) before call dlsym.
      # In general uses of dlerror(3) should call it before use it.
      require 'socket'
      Socket.gethostbyname("localhost")
      DL.dlopen("/usr/lib/libc.so").sym('strcpy')
    end if /freebsd/=~ RUBY_PLATFORM
  end
end
