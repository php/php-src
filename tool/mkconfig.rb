#!./miniruby -s

# avoid warnings with -d.
$install_name ||= nil
$so_name ||= nil

srcdir = File.expand_path('../..', __FILE__)
$:.replace [srcdir+"/lib"] unless defined?(CROSS_COMPILING)
$:.unshift(".")

require "fileutils"
mkconfig = File.basename($0)

rbconfig_rb = ARGV[0] || 'rbconfig.rb'
unless File.directory?(dir = File.dirname(rbconfig_rb))
  FileUtils.makedirs(dir, :verbose => true)
end

version = RUBY_VERSION
config = ""
def config.write(arg)
  concat(arg.to_s)
end
$stdout = config

fast = {'prefix'=>TRUE, 'ruby_install_name'=>TRUE, 'INSTALL'=>TRUE, 'EXEEXT'=>TRUE}
print %[
# This file was created by #{mkconfig} when ruby was built.  Any
# changes made to this file will be lost the next time ruby is built.

module RbConfig
  RUBY_VERSION == "#{version}" or
    raise "ruby lib version (#{version}) doesn't match executable version (\#{RUBY_VERSION})"

]

arch = RUBY_PLATFORM
win32 = /mswin/ =~ arch
universal = /universal.*darwin/ =~ arch
v_fast = []
v_others = []
vars = {}
continued_name = nil
continued_line = nil
File.foreach "config.status" do |line|
  next if /^#/ =~ line
  name = nil
  case line
  when /^s([%,])@(\w+)@\1(?:\|\#_!!_\#\|)?(.*)\1/
    name = $2
    val = $3.gsub(/\\(?=,)/, '')
  when /^S\["(\w+)"\]\s*=\s*"(.*)"\s*(\\)?$/
    name = $1
    val = $2
    if $3
      continued_line = [val]
      continued_name = name
      next
    end
  when /^"(.*)"\s*(\\)?$/
    next if !continued_line
    continued_line << $1
    next if $2
    val = continued_line.join
    name = continued_name
    continued_line = nil
  when /^(?:ac_given_)?INSTALL=(.*)/
    v_fast << "  CONFIG[\"INSTALL\"] = " + $1 + "\n"
  end

  if name
    case name
    when /^(?:ac_.*|configure_input|(?:top_)?srcdir|\w+OBJS)$/; next
    when /^(?:X|(?:MINI|RUN)RUBY$)/; next
    when /^(?:MAJOR|MINOR|TEENY)$/; next
    when /^RUBY_INSTALL_NAME$/; next if $install_name
    when /^RUBY_SO_NAME$/; next if $so_name
    when /^arch$/; if val.empty? then val = arch else arch = val end
    when /^sitearch/; val = '$(arch)' if val.empty?
    end
    case val
    when /^\$\(ac_\w+\)$/; next
    when /^\$\{ac_\w+\}$/; next
    when /^\$ac_\w+$/; next
    end
    if /^program_transform_name$/ =~ name
      val.sub!(/\As(\\?\W)(?:\^|\${1,2})\1\1(;|\z)/, '')
      if val.empty?
        $install_name ||= "ruby"
        next
      end
      unless $install_name
        $install_name = "ruby"
        val.gsub!(/\$\$/, '$')
        val.scan(%r[\G[\s;]*(/(?:\\.|[^/])*/)?([sy])(\\?\W)((?:(?!\3)(?:\\.|.))*)\3((?:(?!\3)(?:\\.|.))*)\3([gi]*)]) do
          |addr, cmd, sep, pat, rep, opt|
          if addr
            Regexp.new(addr[/\A\/(.*)\/\z/, 1]) =~ $install_name or next
          end
          case cmd
          when 's'
            pat = Regexp.new(pat, opt.include?('i'))
            if opt.include?('g')
              $install_name.gsub!(pat, rep)
            else
              $install_name.sub!(pat, rep)
            end
          when 'y'
            $install_name.tr!(Regexp.quote(pat), rep)
          end
        end
      end
    end
    if name == "configure_args"
      val.gsub!(/ +(?!-)/, "=") if win32
      val.gsub!(/--with-out-ext/, "--without-ext")
    end
    val = val.gsub(/\$(?:\$|\{?(\w+)\}?)/) {$1 ? "$(#{$1})" : $&}.dump
    case name
    when /^prefix$/
      val = "(TOPDIR || DESTDIR + #{val})"
    when /^ARCH_FLAG$/
      val = "arch_flag || #{val}" if universal
    when /^UNIVERSAL_ARCHNAMES$/
      universal, val = val, 'universal' if universal
    when /^arch$/
      if universal
        val.sub!(/universal/, %q[#{arch && universal[/(?:\A|\s)#{Regexp.quote(arch)}=(\S+)/, 1] || '\&'}])
      end
    end
    v = "  CONFIG[\"#{name}\"] #{win32 && vars[name] ? '<< "\n"' : '='} #{val}\n"
    vars[name] = true
    if fast[name]
      v_fast << v
    else
      v_others << v
    end
    case name
    when "ruby_version"
      version = val[/\A"(.*)"\z/, 1]
    end
  end
#  break if /^CEOF/
end

drive = File::PATH_SEPARATOR == ';'

prefix = "/lib/ruby/#{version}/#{arch}"
print "  TOPDIR = File.dirname(__FILE__).chomp!(#{prefix.dump})\n"
print "  DESTDIR = ", (drive ? "TOPDIR && TOPDIR[/\\A[a-z]:/i] || " : ""), "'' unless defined? DESTDIR\n"
print <<'ARCH' if universal
  arch_flag = ENV['ARCHFLAGS'] || ((e = ENV['RC_ARCHS']) && e.split.uniq.map {|a| "-arch #{a}"}.join(' '))
  arch = arch_flag && arch_flag[/\A\s*-arch\s+(\S+)\s*\z/, 1]
ARCH
print "  universal = #{universal}\n" if universal
print "  CONFIG = {}\n"
print "  CONFIG[\"DESTDIR\"] = DESTDIR\n"

versions = {}
IO.foreach(File.join(srcdir, "version.h")) do |l|
  m = /^\s*#\s*define\s+RUBY_(PATCHLEVEL)\s+(-?\d+)/.match(l)
  if m
    versions[m[1]] = m[2]
    break
  end
end
IO.foreach(File.join(srcdir, "include/ruby/version.h")) do |l|
  m = /^\s*#\s*define\s+RUBY_API_VERSION_(MAJOR|MINOR|TEENY)\s+(-?\d+)/.match(l)
  if m
    versions[m[1]] = m[2]
    break if versions.size == 4
  end
end
%w[MAJOR MINOR TEENY PATCHLEVEL].each do |v|
  print "  CONFIG[#{v.dump}] = #{versions[v].dump}\n"
end

dest = drive ? %r'= "(?!\$[\(\{])(?i:[a-z]:)' : %r'= "(?!\$[\(\{])'
v_disabled = {}
v_others.collect! do |x|
  if /^\s*CONFIG\["((?!abs_|old)[a-z]+(?:_prefix|dir))"\]/ === x
    name = $1
    if /= "no"$/ =~ x
      v_disabled[name] = true
      v_others.delete(name)
      next
    end
    x.sub(dest, '= "$(DESTDIR)')
  else
    x
  end
end
v_others.compact!

if $install_name
  v_fast << "  CONFIG[\"ruby_install_name\"] = \"" + $install_name + "\"\n"
  v_fast << "  CONFIG[\"RUBY_INSTALL_NAME\"] = \"" + $install_name + "\"\n"
end
if $so_name
  v_fast << "  CONFIG[\"RUBY_SO_NAME\"] = \"" + $so_name + "\"\n"
end

print(*v_fast)
print(*v_others)
print <<EOS
  CONFIG["rubylibdir"] = "$(rubylibprefix)/$(ruby_version)"
  CONFIG["archdir"] = "$(rubylibdir)/$(arch)"
EOS
print <<EOS unless v_disabled["sitedir"]
  CONFIG["sitelibdir"] = "$(sitedir)/$(ruby_version)"
  CONFIG["sitearchdir"] = "$(sitelibdir)/$(sitearch)"
EOS
print <<EOS unless v_disabled["vendordir"]
  CONFIG["vendorlibdir"] = "$(vendordir)/$(ruby_version)"
  CONFIG["vendorarchdir"] = "$(vendorlibdir)/$(sitearch)"
EOS
print <<EOS
  CONFIG["topdir"] = File.dirname(__FILE__)
  MAKEFILE_CONFIG = {}
  CONFIG.each{|k,v| MAKEFILE_CONFIG[k] = v.dup}
  def RbConfig::expand(val, config = CONFIG)
    newval = val.gsub(/\\$\\$|\\$\\(([^()]+)\\)|\\$\\{([^{}]+)\\}/) {
      var = $&
      if !(v = $1 || $2)
	'$'
      elsif key = config[v = v[/\\A[^:]+(?=(?::(.*?)=(.*))?\\z)/]]
	pat, sub = $1, $2
	config[v] = false
	config[v] = RbConfig::expand(key, config)
	key = key.gsub(/\#{Regexp.quote(pat)}(?=\\s|\\z)/n) {sub} if pat
	key
      else
	var
      end
    }
    val.replace(newval) unless newval == val
    val
  end
  CONFIG.each_value do |val|
    RbConfig::expand(val)
  end

  # returns the absolute pathname of the ruby command.
  def RbConfig.ruby
    File.join(
      RbConfig::CONFIG["bindir"],
      RbConfig::CONFIG["ruby_install_name"] + RbConfig::CONFIG["EXEEXT"]
    )
  end
end
autoload :Config, "rbconfig/obsolete.rb" # compatibility for ruby-1.8.4 and older.
CROSS_COMPILING = nil unless defined? CROSS_COMPILING
EOS

$stdout = STDOUT
mode = IO::RDWR|IO::CREAT
mode |= IO::BINARY if defined?(IO::BINARY)
open(rbconfig_rb, mode) do |f|
  if $timestamp and f.stat.size == config.size and f.read == config
    puts "#{rbconfig_rb} unchanged"
  else
    puts "#{rbconfig_rb} updated"
    f.rewind
    f.truncate(0)
    f.print(config)
  end
end
if String === $timestamp
  FileUtils.touch($timestamp)
end

# vi:set sw=2:
