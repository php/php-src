require 'test/unit'
require 'dl'
require_relative '../ruby/envutil'

libc_so = libm_so = nil

case RUBY_PLATFORM
when /cygwin/
  libc_so = "cygwin1.dll"
  libm_so = "cygwin1.dll"
when /x86_64-linux/
  libc_so = "/lib64/libc.so.6"
  libm_so = "/lib64/libm.so.6"
when /linux/
  libdir = '/lib'
  case [0].pack('L!').size
  when 4
    # 32-bit ruby
    libdir = '/lib32' if File.directory? '/lib32'
  when 8
    # 64-bit ruby
    libdir = '/lib64' if File.directory? '/lib64'
  end
  libc_so = File.join(libdir, "libc.so.6")
  libm_so = File.join(libdir, "libm.so.6")
when /mingw/, /mswin/
  require "rbconfig"
  libc_so = libm_so = RbConfig::CONFIG["RUBY_SO_NAME"].split(/-/).find{|e| /^msvc/ =~ e} + ".dll"
when /darwin/
  libc_so = "/usr/lib/libc.dylib"
  libm_so = "/usr/lib/libm.dylib"
when /kfreebsd/
  libc_so = "/lib/libc.so.0.1"
  libm_so = "/lib/libm.so.1"
when /bsd|dragonfly/
  libc_so = "/usr/lib/libc.so"
  libm_so = "/usr/lib/libm.so"
when /solaris/
  libdir = '/lib'
  case [0].pack('L!').size
  when 4
    # 32-bit ruby
    libdir = '/lib' if File.directory? '/lib'
  when 8
    # 64-bit ruby
    libdir = '/lib/64' if File.directory? '/lib/64'
  end
  libc_so = File.join(libdir, "libc.so")
  libm_so = File.join(libdir, "libm.so")
when /aix/
  pwd=Dir.pwd
  libc_so = libm_so = "#{pwd}/libaixdltest.so"
  unless File.exist? libc_so
    cobjs=%w!strcpy.o!
    mobjs=%w!floats.o sin.o!
    funcs=%w!sin sinf strcpy strncpy!
    expfile='dltest.exp'
    require 'tmpdir'
    Dir.mktmpdir do |dir|
      begin
        Dir.chdir dir
        %x!/usr/bin/ar x /usr/lib/libc.a #{cobjs.join(' ')}!
        %x!/usr/bin/ar x /usr/lib/libm.a #{mobjs.join(' ')}!
        %x!echo "#{funcs.join("\n")}\n" > #{expfile}!
        require 'rbconfig'
        if RbConfig::CONFIG["GCC"] = 'yes'
          lflag='-Wl,'
        else
          lflag=''
        end
        flags="#{lflag}-bE:#{expfile} #{lflag}-bnoentry -lm"
        %x!#{RbConfig::CONFIG["LDSHARED"]} -o #{libc_so} #{(cobjs+mobjs).join(' ')} #{flags}!
      ensure
        Dir.chdir pwd
      end
    end
  end
else
  libc_so = ARGV[0] if ARGV[0] && ARGV[0][0] == ?/
  libm_so = ARGV[1] if ARGV[1] && ARGV[1][0] == ?/
  if( !(libc_so && libm_so) )
    $stderr.puts("libc and libm not found: #{$0} <libc> <libm>")
  end
end

libc_so = nil if !libc_so || (libc_so[0] == ?/ && !File.file?(libc_so))
libm_so = nil if !libm_so || (libm_so[0] == ?/ && !File.file?(libm_so))

if !libc_so || !libm_so
  ruby = EnvUtil.rubybin
  ldd = `ldd #{ruby}`
  #puts ldd
  libc_so = $& if !libc_so && %r{/\S*/libc\.so\S*} =~ ldd
  libm_so = $& if !libm_so && %r{/\S*/libm\.so\S*} =~ ldd
  #p [libc_so, libm_so]
end

DL::LIBC_SO = libc_so
DL::LIBM_SO = libm_so

module DL
  class TestBase < Test::Unit::TestCase
    include Math
    include DL

    def setup
      @libc = dlopen(LIBC_SO)
      @libm = dlopen(LIBM_SO)
    end

    def assert_match(expected, actual, message="")
      assert_operator(expected, :===, actual, message)
    end

    def assert_positive(actual)
      assert_operator(actual, :>, 0)
    end

    def assert_zero(actual)
      assert_equal(0, actual)
    end

    def assert_negative(actual)
      assert_operator(actual, :<, 0)
    end

    def test_empty()
    end
  end
end
