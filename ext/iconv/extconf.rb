require 'mkmf'

dir_config("iconv")

conf = File.exist?(File.join($srcdir, "config.charset"))
conf = with_config("config-charset", enable_config("config-charset", conf))

if have_func("iconv", "iconv.h") or
    have_library("iconv", "iconv", "iconv.h")
  check_signedness("size_t")
  if checking_for("const of iconv() 2nd argument") do
      create_tmpsrc(cpp_include("iconv.h") + "---> iconv(cd,0,0,0,0) <---")
      src = xpopen(cpp_command("")) {|f|f.read}
      if !(func = src[/^--->\s*(\w+).*\s*<---/, 1])
        Logging::message "iconv function name not found"
        false
      elsif !(second = src[%r"\b#{func}\s*\(.*?,(.*?),.*?\)\s*;"m, 1])
        Logging::message "prototype for #{func}() not found"
        false
      else
        Logging::message $&+"\n"
        /\bconst\b/ =~ second
      end
    end
    $defs.push('-DICONV_INPTR_CONST')
  end
  have_func("iconvlist", "iconv.h")
  have_func("__iconv_free_list", "iconv.h")
  if conf
    prefix = '$(srcdir)'
    prefix =  $nmake ? "{#{prefix}}" : "#{prefix}/"
    if $extout
      wrapper = "$(RUBYARCHDIR)/iconv.rb"
    else
      wrapper = "./iconv.rb"
      $INSTALLFILES = [[wrapper, "$(RUBYARCHDIR)"]]
    end
    if String === conf
      require 'uri'
      scheme = URI.parse(conf).scheme
    else
      conf = "$(srcdir)/config.charset"
    end
    $cleanfiles << wrapper
  end
  create_makefile("iconv")
  if conf
    open("Makefile", "a") do |mf|
      mf.print("\nall: #{wrapper}\n\n#{wrapper}: #{prefix}charset_alias.rb")
      mf.print(" ", conf) unless scheme
      mf.print("\n\t$(RUBY) $(srcdir)/charset_alias.rb #{conf} $@\n")
    end
  end
end
