#!./miniruby

show = false
precommand = []
while arg = ARGV[0]
  break ARGV.shift if arg == '--'
  /\A--([-\w]+)(?:=(.*))?\z/ =~ arg or break
  arg, value = $1, $2
  re = Regexp.new('\A'+arg.gsub(/\w+\b/, '\&\\w*')+'\z', "i")
  case
  when re =~ "srcdir"
    srcdir = value
  when re =~ "archdir"
    archdir = value
  when re =~ "cpu"
    precommand << "arch" << "-arch" << value
  when re =~ "extout"
    extout = value
  when re =~ "pure"
    # obsolete switch do nothing
  when re =~ "debugger"
    require 'shellwords'
    precommand.concat(value ? (Shellwords.shellwords(value) unless value == "no") : %w"gdb --args")
  when re =~ "precommand"
    require 'shellwords'
    precommand.concat(Shellwords.shellwords(value))
  when re =~ "show"
    show = true
  else
    break
  end
  ARGV.shift
end

srcdir ||= File.expand_path('..', File.dirname(__FILE__))
archdir ||= '.'

abs_archdir = File.expand_path(archdir)
$:.unshift(abs_archdir)

config = File.read(conffile = File.join(abs_archdir, 'rbconfig.rb'))
config.sub!(/^(\s*)RUBY_VERSION\s*==.*(\sor\s*)$/, '\1true\2')
config = Module.new {module_eval(config, conffile)}::RbConfig::CONFIG

ruby = File.join(archdir, config["RUBY_INSTALL_NAME"]+config['EXEEXT'])
unless File.exist?(ruby)
  abort "#{ruby} is not found.\nTry `make' first, then `make test', please.\n"
end

libs = [abs_archdir]
extout ||= config["EXTOUT"]
if extout
  abs_extout = File.expand_path(extout)
  libs << File.expand_path("common", abs_extout) << File.expand_path(config['arch'], abs_extout)
end
libs << File.expand_path("lib", srcdir)
config["bindir"] = abs_archdir

env = {}

env["RUBY"] = File.expand_path(ruby)
env["PATH"] = [abs_archdir, ENV["PATH"]].compact.join(File::PATH_SEPARATOR)

if e = ENV["RUBYLIB"]
  libs |= e.split(File::PATH_SEPARATOR)
end
env["RUBYLIB"] = $:.replace(libs).join(File::PATH_SEPARATOR)

libruby_so = File.join(abs_archdir, config['LIBRUBY_SO'])
if File.file?(libruby_so)
  if e = config['LIBPATHENV'] and !e.empty?
    env[e] = [abs_archdir, ENV[e]].compact.join(File::PATH_SEPARATOR)
  end
  if /linux/ =~ RUBY_PLATFORM
    env["LD_PRELOAD"] = [libruby_so, ENV["LD_PRELOAD"]].compact.join(' ')
  end
end

ENV.update env

cmd = [ruby]
cmd.concat(ARGV)
cmd.unshift(*precommand) unless precommand.empty?

if show
  require 'shellwords'
  env.each {|k,v| puts "#{k}=#{v}"}
  puts Shellwords.join(cmd)
end

exec(*cmd)
