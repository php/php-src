# extconf.rb for tcltklib

require 'mkmf'

is_win32 = (/mswin|mingw|cygwin|bccwin/ =~ RUBY_PLATFORM)
#is_macosx = (/darwin/ =~ RUBY_PLATFORM)

have_func("ruby_native_thread_p", "ruby.h")
have_func("rb_errinfo", "ruby.h")
have_func("rb_safe_level", "ruby.h")
have_struct_member("struct RArray", "ptr", "ruby.h")
have_struct_member("struct RArray", "len", "ruby.h")

def find_framework(tcl_hdr, tk_hdr)
  if framework_dir = with_config("tcltk-framework")
    paths = [framework_dir]
  else
    unless tcl_hdr || tk_hdr ||
        enable_config("tcltk-framework", false) ||
        enable_config("mac-tcltk-framework", false)
      return false
    end
    paths = ["/Library/Frameworks", "/System/Library/Frameworks"]
  end

  checking_for('Tcl/Tk Framework') {
    paths.find{|dir|
      dir.strip!
      dir.chomp!('/')
      (tcl_hdr || FileTest.directory?(dir + "/Tcl.framework/") ) &&
        (tk_hdr || FileTest.directory?(dir + "/Tk.framework/") )
    }
  }
end

tcl_framework_header = with_config("tcl-framework-header")
tk_framework_header  = with_config("tk-framework-header")

tcltk_framework = find_framework(tcl_framework_header, tk_framework_header)

unless is_win32
  have_library("nsl", "t_open")
  have_library("socket", "socket")
  have_library("dl", "dlopen")
  have_library("m", "log")
end

tk_idir,  tk_ldir  = dir_config("tk")
tcl_idir, tcl_ldir = dir_config("tcl")
x11_idir, x11_ldir = dir_config("X11")

tk_ldir2  = with_config("tk-lib")
tcl_ldir2 = with_config("tcl-lib")
x11_ldir2 = with_config("X11-lib")

tk_ldir_list  = [tk_ldir2,  tk_ldir]
tcl_ldir_list = [tcl_ldir2, tcl_ldir]

tklib = with_config("tklib")
tcllib = with_config("tcllib")
stubs = enable_config("tcltk_stubs") || with_config("tcltk_stubs")

tcltk_version = with_config("tcltkversion")

use_X = with_config("X11", (! is_win32))

def parse_tclConfig(file)
  # check tclConfig.sh/tkConfig.sh
  tbl = {}
  IO.foreach(file){|line|
    line.strip!
    next if line !~ /^([^\#=][^=]*)=(['"]|)(.*)\2$/
    key, val = $1, $3
    tbl[key] = val.gsub(/\$\{([^}]+)\}/){|s| tbl[$1]} rescue nil
  }
  tbl
end

def check_tcltk_version(version)
  return [nil, nil] unless version

  version = version.strip

  tclver = version.dup
  tkver  = version.dup

  major = dot = minor = dot = plvl = ext = nil

  if version =~ /^(\d)(\.?)(\d)(\.?)(\d*)(.*)$/
    major = $1; minor_dot = $2; minor = $3; plvl_dot  = $4; plvl = $5; ext = $6
    dot = ! minor_dot.empty?
    if plvl_dot.empty? && ! plvl.empty?
      minor << plvl
    end
  elsif version =~ /^(\d)(\.?)(\d?)(.*)$/
    major = $1; minor_dot = $2; minor = $3; ext = $4
    dot = ! minor_dot.empty?
  else # unknown -> believe user
    return [tclver, tkver]
  end

  # check Tcl7.6 / Tk4.2 ?
  if major == "7" # Tcl7.6 ( not support Tclversion < 7.6 )
    # Tk4.2
    tkver  = "4" + ((dot)? ".": "") + ((minor.empty)? "": "2") + ext
  elsif major == "4" # Tk4.2 ( not support Tkversion < 4.2 )
    # Tcl7.6
    tclver = "7" + ((dot)? ".": "") + ((minor.empty)? "": "6") + ext
  end

  [tclver, tkver]
end

def find_tcl(tcllib, stubs, version, *opt_paths)
  default_paths = ["/usr/local/lib", "/usr/pkg/lib", "/usr/lib"]
  default_paths << "/Tcl/lib"  # default for ActiveTcl

  if (paths = opt_paths.compact).empty?
    paths = default_paths
  end

  if stubs
    func = "Tcl_InitStubs"
    lib = "tclstub"
  else
    func = "Tcl_FindExecutable"
    lib = "tcl"
  end

  if version && ! version.empty?
    versions = [version]
  else
    versions = %w[8.7 8.6 8.5 8.4 8.3 8.2 8.1 8.0 7.6]
  end

  if tcllib
    st = find_library(tcllib, func, *paths)
  else
    st = versions.find { |ver|
           find_library("#{lib}#{ver}", func, *paths) or
           find_library("#{lib}#{ver.delete('.')}", func, *paths) or
           find_library("#{lib}#{ver}g", func, *paths) or
           find_library("#{lib}#{ver.delete('.')}g", func, *paths) or
           find_library("tcl#{ver}", func, *paths) or
           find_library("tcl#{ver.delete('.')}", func, *paths) or
           find_library("tcl#{ver}g", func, *paths) or
           find_library("tcl#{ver.delete('.')}g", func, *paths)
         } || (!version && find_library(lib, func, *paths))
  end

  unless st
    puts("Warning:: cannot find Tcl library. tcltklib will not be compiled (tcltklib is disabled on your Ruby == Ruby/Tk will not work). Please check configure options.")
  end
  st
end

def find_tk(tklib, stubs, version, *opt_paths)
  default_paths = ["/usr/local/lib", "/usr/pkg/lib", "/usr/lib"]
  default_paths << "/Tcl/lib"  # default for ActiveTcl

  if (paths = opt_paths.compact).empty?
    paths = default_paths
  end

  if stubs
    func = "Tk_InitStubs"
    lib = "tkstub"
  else
    func = "Tk_Init"
    lib = "tk"
  end

  if version && ! version.empty?
    versions = [version]
  else
    versions = %w[8.7 8.6 8.5 8.4 8.3 8.2 8.1 8.0 4.2]
  end

  if tklib
    st = find_library(tklib, func, *paths)
  else
    st = versions.find { |ver|
           find_library("#{lib}#{ver}", func, *paths) or
           find_library("#{lib}#{ver.delete('.')}", func, *paths) or
           find_library("#{lib}#{ver}g", func, *paths) or
           find_library("#{lib}#{ver.delete('.')}g", func, *paths) or
           find_library("tk#{ver}", func, *paths) or
           find_library("tk#{ver.delete('.')}", func, *paths) or
           find_library("tk#{ver}g", func, *paths) or
           find_library("tk#{ver.delete('.')}g", func, *paths)
         } || (!version && find_library(lib, func, *paths))
  end

  unless st
    puts("Warning:: cannot find Tk library. tcltklib will not be compiled (tcltklib is disabled on your Ruby == Ruby/Tk will not work). Please check configure options.")
  end
  st
end

def find_tcltk_header(tclver, tkver)
  base_dir = ['/usr/local/include', '/usr/pkg/include', '/usr/include']
  base_dir << '/Tcl/include' # default for ActiveTcl

  unless have_tcl_h = have_header('tcl.h')
    if tclver && ! tclver.empty?
      versions = [tclver]
    else
      versions = %w[8.7 8.6 8.5 8.4 8.3 8.2 8.1 8.0 7.6]
    end
    paths = base_dir.dup
    versions.each{|ver| paths.concat(base_dir.map{|dir| dir + '/tcl' + ver})}
    have_tcl_h = find_header('tcl.h', *paths)
  end

  unless have_tk_h  = have_header("tk.h")
    if tkver && ! tkver.empty?
      versions = [tkver]
    else
      versions = %w[8.7 8.6 8.5 8.4 8.3 8.2 8.1 8.0 4.2]
    end
    paths = base_dir.dup
    versions.each{|ver| paths.concat(base_dir.map{|dir| dir + '/tk' + ver})}
    have_tk_h = find_header('tk.h', *paths)
  end

  have_tcl_h && have_tk_h
end

def find_X11(*opt_paths)
  default_paths =
    [ "/usr/X11/lib", "/usr/lib/X11", "/usr/X11R6/lib", "/usr/openwin/lib" ]
  paths = opt_paths.compact.concat(default_paths)
  st = find_library("X11", "XOpenDisplay", *paths)
  unless st
    puts("Warning:: cannot find X11 library. tcltklib will not be compiled (tcltklib is disabled on your Ruby == Ruby/Tk will not work). Please check configure options. If your Tcl/Tk don't require X11, please try --without-X11.")
  end
  st
end

def pthread_check()
  tcl_major_ver = nil
  tcl_minor_ver = nil

  # Is tcl-thread given by user ?
  case enable_config("tcl-thread")
  when true
    tcl_enable_thread = true
  when false
    tcl_enable_thread = false
  else
    tcl_enable_thread = nil
  end

  if (tclConfig = with_config("tclConfig-file"))
    if tcl_enable_thread == true
      puts("Warning: --with-tclConfig-file option is ignored, because --enable-tcl-thread option is given.")
    elsif tcl_enable_thread == false
      puts("Warning: --with-tclConfig-file option is ignored, because --disable-tcl-thread option is given.")
    else
      # tcl-thread is unknown and tclConfig.sh is given
      begin
        tbl = parse_tclConfig(tclConfig)
        if tbl['TCL_THREADS']
          tcl_enable_thread = (tbl['TCL_THREADS'] == "1")
        else
          tcl_major_ver = tbl['TCL_MAJOR_VERSION'].to_i
          tcl_minor_ver = tbl['TCL_MINOR_VERSION'].to_i
          if tcl_major_ver < 8 || (tcl_major_ver == 8 && tcl_minor_ver == 0)
            tcl_enable_thread = false
          end
        end

        if tcl_enable_thread == nil
          # cannot find definition
          if tcl_major_ver
            puts("Warning: '#{tclConfig}' doesn't include TCL_THREADS definition.")
          else
            puts("Warning: '#{tclConfig}' may not be a tclConfig file.")
          end
          tclConfig = false
        end
      rescue Exception
        puts("Warning: fail to read '#{tclConfig}'!! --> ignore the file")
        tclConfig = false
      end
    end
  end

  if tcl_enable_thread == nil && !tclConfig
    # tcl-thread is unknown and tclConfig is unavailable
    begin
      try_run_available = try_run("int main() { exit(0); }")
    rescue Exception
      # cannot try_run. Is CROSS-COMPILE environment?
      puts(%Q'\
*****************************************************************************
**
** PTHREAD SUPPORT CHECK WARNING:
**
**   We cannot check the consistency of pthread support between Ruby
**   and the Tcl/Tk library in your environment (are you perhaps
**   cross-compiling?). If pthread support for these 2 packages is
**   inconsistent you may find you get errors when running Ruby/Tk
**   (e.g. hangs or segmentation faults).  We strongly recommend
**   you to check the consistency manually.
**
*****************************************************************************
')
      return true
    end
  end

  if tcl_enable_thread == nil
    # tcl-thread is unknown
    if try_run(<<EOF)
#include <tcl.h>
int main() {
   Tcl_Interp *ip;
   ip = Tcl_CreateInterp();
   exit((Tcl_Eval(ip, "set tcl_platform(threaded)") == TCL_OK)? 0: 1);
}
EOF
      tcl_enable_thread = true
    elsif try_run(<<EOF)
#include <tcl.h>
static Tcl_ThreadDataKey dataKey;
int main() { exit((Tcl_GetThreadData(&dataKey, 1) == dataKey)? 1: 0); }
EOF
      tcl_enable_thread = true
    else
      tcl_enable_thread = false
    end
  end

  # check pthread mode
  if (macro_defined?('HAVE_NATIVETHREAD', '#include "ruby.h"'))
    # ruby -> enable
    unless tcl_enable_thread
      # ruby -> enable && tcl -> disable
      puts(%Q'\
*****************************************************************************
**
** PTHREAD SUPPORT MODE WARNING:
**
**   Ruby is compiled with --enable-pthread, but your Tcl/Tk library
**   seems to be compiled without pthread support. Although you can
**   create the tcltklib library, this combination may cause errors
**   (e.g. hangs or segmentation faults). If you have no reason to
**   keep the current pthread support status, we recommend you reconfigure
**   and recompile the libraries so that both or neither support pthreads.
**
**   If you want change the status of pthread support, please recompile
**   Ruby without "--enable-pthread" configure option or recompile Tcl/Tk
**   with "--enable-threads" configure option (if your Tcl/Tk is later
**   than or equal to Tcl/Tk 8.1).
**
*****************************************************************************
')
    end

    # ruby -> enable && tcl -> enable/disable
    if tcl_enable_thread
      $CPPFLAGS += ' -DWITH_TCL_ENABLE_THREAD=1'
    else
      $CPPFLAGS += ' -DWITH_TCL_ENABLE_THREAD=0'
    end

    return true

  else
    # ruby -> disable
    if tcl_enable_thread
      # ruby -> disable && tcl -> enable
      puts(%Q'\
*****************************************************************************
**
** PTHREAD SUPPORT MODE ERROR:
**
**   Ruby is not compiled with --enable-pthread, but your Tcl/Tk
**   library seems to be compiled with pthread support. This
**   combination may cause frequent hang or segmentation fault
**   errors when Ruby/Tk is working. We recommend that you NEVER
**   create the library with such a combination of pthread support.
**
**   Please recompile Ruby with the "--enable-pthread" configure option
**   or recompile Tcl/Tk with the "--disable-threads" configure option.
**
*****************************************************************************
')
      $CPPFLAGS += ' -DWITH_TCL_ENABLE_THREAD=1'
      return false
    else
      # ruby -> disable && tcl -> disable
      $CPPFLAGS += ' -DWITH_TCL_ENABLE_THREAD=0'
      return true
    end
  end
end

tclver, tkver = check_tcltk_version(tcltk_version)

if ( tcltk_framework ||
       ( find_tcltk_header(tclver, tkver) &&
           ( !use_X || find_X11(x11_ldir2, x11_ldir) ) &&
           find_tcl(tcllib, stubs, tclver, *tcl_ldir_list) &&
           find_tk(tklib, stubs, tkver, *tk_ldir_list) ) )
  $CPPFLAGS += ' -DUSE_TCL_STUBS -DUSE_TK_STUBS' if stubs
  $CPPFLAGS += ' -D_WIN32' if /cygwin/ =~ RUBY_PLATFORM

  if tcltk_framework
    if tcl_framework_header
      $CPPFLAGS += " -I#{tcl_framework_header}"
    else
      $CPPFLAGS += " -I#{tcltk_framework}/Tcl.framework/Headers"
    end

    if tk_framework_header
      $CPPFLAGS += " -I#{tk_framework_header}"
    else
      $CPPFLAGS += " -I#{tcltk_framework}/Tk.framework/Headers"
    end

    $LDFLAGS += ' -framework Tk -framework Tcl'
  end

  if stubs or pthread_check
    # create Makefile

    # for SUPPORT_STATUS
    $INSTALLFILES ||= []
    $INSTALLFILES << ["lib/tkextlib/SUPPORT_STATUS", "$(RUBYLIBDIR)", "lib"]

    have_func("rb_hash_lookup", "ruby.h")

    # create
    $defs << %[-DRUBY_VERSION=\\"#{RUBY_VERSION}\\"]
    $defs << %[-DRUBY_RELEASE_DATE=\\"#{RUBY_RELEASE_DATE}\\"]
    create_makefile("tcltklib")
  end
end
