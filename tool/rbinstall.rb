#!./miniruby

begin
  load "./rbconfig.rb"
rescue LoadError
  CONFIG = Hash.new {""}
else
  include RbConfig
  $".unshift File.expand_path("./rbconfig.rb")
end

srcdir = File.expand_path('../..', __FILE__)
unless defined?(CROSS_COMPILING) and CROSS_COMPILING
  $:.replace([srcdir+"/lib", Dir.pwd])
end
require 'fileutils'
require 'shellwords'
require 'optparse'
require 'optparse/shellwords'
require 'ostruct'

STDOUT.sync = true
File.umask(0)

def parse_args(argv = ARGV)
  $mantype = 'doc'
  $destdir = nil
  $extout = nil
  $make = 'make'
  $mflags = []
  $install = []
  $installed_list = nil
  $dryrun = false
  $rdocdir = nil
  $data_mode = 0644
  $prog_mode = 0755
  $dir_mode = nil
  $script_mode = nil
  $strip = false
  $cmdtype = (if File::ALT_SEPARATOR == '\\'
                File.exist?("rubystub.exe") ? 'exe' : 'bat'
              end)
  mflags = []
  opt = OptionParser.new
  opt.on('-n', '--dry-run') {$dryrun = true}
  opt.on('--dest-dir=DIR') {|dir| $destdir = dir}
  opt.on('--extout=DIR') {|dir| $extout = (dir unless dir.empty?)}
  opt.on('--make=COMMAND') {|make| $make = make}
  opt.on('--mantype=MAN') {|man| $mantype = man}
  opt.on('--make-flags=FLAGS', '--mflags', Shellwords) do |v|
    if arg = v.first
      arg.insert(0, '-') if /\A[^-][^=]*\Z/ =~ arg
    end
    $mflags.concat(v)
  end
  opt.on('-i', '--install=TYPE', $install_procs.keys) do |ins|
    $install << ins
  end
  opt.on('--data-mode=OCTAL-MODE', OptionParser::OctalInteger) do |mode|
    $data_mode = mode
  end
  opt.on('--prog-mode=OCTAL-MODE', OptionParser::OctalInteger) do |mode|
    $prog_mode = mode
  end
  opt.on('--dir-mode=OCTAL-MODE', OptionParser::OctalInteger) do |mode|
    $dir_mode = mode
  end
  opt.on('--script-mode=OCTAL-MODE', OptionParser::OctalInteger) do |mode|
    $script_mode = mode
  end
  opt.on('--installed-list [FILENAME]') {|name| $installed_list = name}
  opt.on('--rdoc-output [DIR]') {|dir| $rdocdir = dir}
  opt.on('--cmd-type=TYPE', %w[bat cmd plain]) {|cmd| $cmdtype = (cmd unless cmd == 'plain')}
  opt.on('--[no-]strip') {|strip| $strip = strip}

  opt.order!(argv) do |v|
    case v
    when /\AINSTALL[-_]([-\w]+)=(.*)/
      argv.unshift("--#{$1.tr('_', '-')}=#{$2}")
    when /\A\w[-\w+]*=\z/
      mflags << v
    when /\A\w[-\w+]*\z/
      $install << v.intern
    else
      raise OptionParser::InvalidArgument, v
    end
  end rescue abort "#{$!.message}\n#{opt.help}"

  unless defined?(RbConfig)
    puts opt.help
    exit
  end

  $make, *rest = Shellwords.shellwords($make)
  $mflags.unshift(*rest) unless rest.empty?
  $mflags.unshift(*mflags)

  def $mflags.set?(flag)
    grep(/\A-(?!-).*#{flag.chr}/i) { return true }
    false
  end
  def $mflags.defined?(var)
    grep(/\A#{var}=(.*)/) {return block_given? ? yield($1) : $1}
    false
  end

  if $mflags.set?(?n)
    $dryrun = true
  else
    $mflags << '-n' if $dryrun
  end

  $destdir ||= $mflags.defined?("DESTDIR")
  if $extout ||= $mflags.defined?("EXTOUT")
    RbConfig.expand($extout)
  end

  $continue = $mflags.set?(?k)

  if $installed_list ||= $mflags.defined?('INSTALLED_LIST')
    RbConfig.expand($installed_list, RbConfig::CONFIG)
    $installed_list = open($installed_list, "ab")
    $installed_list.sync = true
  end

  $rdocdir ||= $mflags.defined?('RDOCOUT')

  $dir_mode ||= $prog_mode | 0700
  $script_mode ||= $prog_mode
end

$install_procs = Hash.new {[]}
def install?(*types, &block)
  $install_procs[:all] <<= block
  types.each do |type|
    $install_procs[type] <<= block
  end
end

def strip_file(files)
  if !defined?($strip_command) and (cmd = CONFIG["STRIP"])
    case cmd
    when "", "true", ":" then return
    else $strip_command = Shellwords.shellwords(cmd)
    end
  elsif !$strip_command
    return
  end
  system(*($strip_command + [files].flatten))
end

def install(src, dest, options = {})
  options = options.clone
  strip = options.delete(:strip)
  options[:preserve] = true
  d = with_destdir(dest)
  super(src, d, options)
  srcs = Array(src)
  if strip
    d = srcs.map {|src| File.join(d, File.basename(src))} if $made_dirs[dest]
    strip_file(d)
  end
  if $installed_list
    dest = srcs.map {|src| File.join(dest, File.basename(src))} if $made_dirs[dest]
    $installed_list.puts dest
  end
end

def ln_sf(src, dest)
  super(src, with_destdir(dest))
  $installed_list.puts dest if $installed_list
end

$made_dirs = {}
def makedirs(dirs)
  dirs = fu_list(dirs)
  dirs.collect! do |dir|
    realdir = with_destdir(dir)
    realdir unless $made_dirs.fetch(dir) do
      $made_dirs[dir] = true
      $installed_list.puts(File.join(dir, "")) if $installed_list
      File.directory?(realdir)
    end
  end.compact!
  super(dirs, :mode => $dir_mode) unless dirs.empty?
end

FalseProc = proc {false}
def path_matcher(pat)
  if pat and !pat.empty?
    proc {|f| pat.any? {|n| File.fnmatch?(n, f)}}
  else
    FalseProc
  end
end

def install_recursive(srcdir, dest, options = {})
  opts = options.clone
  noinst = opts.delete(:no_install)
  glob = opts.delete(:glob) || "*"
  subpath = (srcdir.size+1)..-1
  prune = []
  skip = []
  if noinst
    if Array === noinst
      prune = noinst.grep(/#{File::SEPARATOR}/o).map!{|f| f.chomp(File::SEPARATOR)}
      skip = noinst.grep(/\A[^#{File::SEPARATOR}]*\z/o)
    else
      if noinst.index(File::SEPARATOR)
        prune = [noinst]
      else
        skip = [noinst]
      end
    end
  end
  skip |= %w"#*# *~ *.old *.bak *.orig *.rej *.diff *.patch *.core"
  prune = path_matcher(prune)
  skip = path_matcher(skip)
  File.directory?(srcdir) or return rescue return
  paths = [[srcdir, dest, true]]
  found = []
  while file = paths.shift
    found << file
    file, d, dir = *file
    if dir
      files = []
      Dir.foreach(file) do |f|
        src = File.join(file, f)
        d = File.join(dest, dir = src[subpath])
        stat = File.stat(src) rescue next
        if stat.directory?
          files << [src, d, true] if /\A\./ !~ f and !prune[dir]
        else
          files << [src, d, false] if File.fnmatch?(glob, f) and !skip[f]
        end
      end
      paths.insert(0, *files)
    end
  end
  for src, d, dir in found
    if dir
      makedirs(d)
    else
      makedirs(d[/.*(?=\/)/m])
      if block_given?
        yield src, d, opts
      else
        install src, d, opts
      end
    end
  end
end

def open_for_install(path, mode)
  data = open(realpath = with_destdir(path), "rb") {|f| f.read} rescue nil
  newdata = yield
  unless $dryrun
    unless newdata == data
      open(realpath, "wb", mode) {|f| f.write newdata}
    end
    File.chmod(mode, realpath)
  end
  $installed_list.puts path if $installed_list
end

def with_destdir(dir)
  return dir if !$destdir or $destdir.empty?
  dir = dir.sub(/\A\w:/, '') if File::PATH_SEPARATOR == ';'
  $destdir + dir
end

def prepare(mesg, basedir, subdirs=nil)
  return unless basedir
  case
  when !subdirs
    dirs = basedir
  when subdirs.size == 0
    subdirs = nil
  when subdirs.size == 1
    dirs = [basedir = File.join(basedir, subdirs)]
    subdirs = nil
  else
    dirs = [basedir, *subdirs.collect {|dir| File.join(basedir, dir)}]
  end
  printf("installing %-18s %s%s\n", "#{mesg}:", basedir,
         (subdirs ? " (#{subdirs.join(', ')})" : ""))
  makedirs(dirs)
end

def CONFIG.[](name, mandatory = false)
  value = super(name)
  if mandatory
    raise "CONFIG['#{name}'] must be set" if !value or value.empty?
  end
  value
end

exeext = CONFIG["EXEEXT"]

ruby_install_name = CONFIG["ruby_install_name", true]
rubyw_install_name = CONFIG["rubyw_install_name"]
goruby_install_name = "go" + ruby_install_name

bindir = CONFIG["bindir", true]
libdir = CONFIG["libdir", true]
archhdrdir = rubyhdrdir = CONFIG["rubyhdrdir", true]
archhdrdir += "/" + CONFIG["arch", true]
rubylibdir = CONFIG["rubylibdir", true]
archlibdir = CONFIG["archdir", true]
sitelibdir = CONFIG["sitelibdir"]
sitearchlibdir = CONFIG["sitearchdir"]
vendorlibdir = CONFIG["vendorlibdir"]
vendorarchlibdir = CONFIG["vendorarchdir"]
mandir = CONFIG["mandir", true]
capidir = CONFIG["docdir", true]
configure_args = Shellwords.shellwords(CONFIG["configure_args"])
enable_shared = CONFIG["ENABLE_SHARED"] == 'yes'
dll = CONFIG["LIBRUBY_SO", enable_shared]
lib = CONFIG["LIBRUBY", true]
arc = CONFIG["LIBRUBY_A", true]
major = CONFIG["MAJOR", true]
minor = CONFIG["MINOR", true]
load_relative = configure_args.include?("--enable-load-relative")

install?(:local, :arch, :bin, :'bin-arch') do
  prepare "binary commands", bindir

  install ruby_install_name+exeext, bindir, :mode => $prog_mode, :strip => $strip
  if rubyw_install_name and !rubyw_install_name.empty?
    install rubyw_install_name+exeext, bindir, :mode => $prog_mode, :strip => $strip
  end
  if File.exist? goruby_install_name+exeext
    install goruby_install_name+exeext, bindir, :mode => $prog_mode, :strip => $strip
  end
  if enable_shared and dll != lib
    install dll, bindir, :mode => $prog_mode, :strip => $strip
  end
end

install?(:local, :arch, :lib) do
  prepare "base libraries", libdir

  install lib, libdir, :mode => $prog_mode, :strip => $strip unless lib == arc
  install arc, libdir, :mode => $data_mode
  if dll == lib and dll != arc
    for link in CONFIG["LIBRUBY_ALIASES"].split
      ln_sf(dll, File.join(libdir, link))
    end
  end

  prepare "arch files", archlibdir
  install "rbconfig.rb", archlibdir, :mode => $data_mode
  if CONFIG["ARCHFILE"]
    for file in CONFIG["ARCHFILE"].split
      install file, archlibdir, :mode => $data_mode
    end
  end
end

install?(:local, :arch, :data) do
  pc = CONFIG["ruby_pc"]
  if pc and File.file?(pc) and File.size?(pc)
    prepare "pkgconfig data", pkgconfigdir = File.join(libdir, "pkgconfig")
    install pc, pkgconfigdir, :mode => $data_mode
  end
end

install?(:ext, :arch, :'ext-arch') do
  prepare "extension objects", archlibdir
  noinst = %w[-* -*/] | (CONFIG["no_install_files"] || "").split
  install_recursive("#{$extout}/#{CONFIG['arch']}", archlibdir, :no_install => noinst, :mode => $prog_mode, :strip => $strip)
  prepare "extension objects", sitearchlibdir
  prepare "extension objects", vendorarchlibdir
end
install?(:ext, :arch, :'ext-arch') do
  prepare "extension headers", archhdrdir
  install_recursive("#{$extout}/include/#{CONFIG['arch']}", archhdrdir, :glob => "*.h", :mode => $data_mode)
end
install?(:ext, :comm, :'ext-comm') do
  prepare "extension scripts", rubylibdir
  install_recursive("#{$extout}/common", rubylibdir, :mode => $data_mode)
  prepare "extension scripts", sitelibdir
  prepare "extension scripts", vendorlibdir
end
install?(:ext, :comm, :'ext-comm') do
  hdrdir = rubyhdrdir + "/ruby"
  prepare "extension headers", hdrdir
  install_recursive("#{$extout}/include/ruby", hdrdir, :glob => "*.h", :mode => $data_mode)
end

install?(:doc, :rdoc) do
  if $rdocdir
    ridatadir = File.join(CONFIG['ridir'], CONFIG['ruby_version'], "system")
    prepare "rdoc", ridatadir
    install_recursive($rdocdir, ridatadir, :mode => $data_mode)
  end
end
install?(:doc, :capi) do
  prepare "capi-docs", capidir
  install_recursive "doc/capi", capidir, :mode => $data_mode
end

if load_relative
  PROLOG_SCRIPT = <<EOS
#!/bin/sh\n# -*- ruby -*-
bindir=`#{CONFIG["CHDIR"]} "${0%/*}" 2>/dev/null; pwd`
EOS
  if CONFIG["LIBRUBY_RELATIVE"] != 'yes' and libpathenv = CONFIG["LIBPATHENV"]
    pathsep = File::PATH_SEPARATOR
    PROLOG_SCRIPT << <<EOS
prefix="${bindir%/bin}"
export #{libpathenv}="$prefix/lib${#{libpathenv}#{pathsep}+#{pathsep}$#{libpathenv}}"
EOS
  end
  PROLOG_SCRIPT << %Q[exec "$bindir/#{ruby_install_name}" -x "$0" "$@"\n]
else
  PROLOG_SCRIPT = nil
end

install?(:local, :comm, :bin, :'bin-comm') do
  prepare "command scripts", bindir

  ruby_shebang = File.join(bindir, ruby_install_name)
  if File::ALT_SEPARATOR
    ruby_bin = ruby_shebang.tr(File::SEPARATOR, File::ALT_SEPARATOR)
    if $cmdtype == 'exe'
      stub = File.open("rubystub.exe", "rb") {|f| f.read} << "\n" rescue nil
    end
  end
  if trans = CONFIG["program_transform_name"]
    exp = []
    trans.gsub!(/\$\$/, '$')
    trans.scan(%r[\G[\s;]*(/(?:\\.|[^/])*/)?([sy])(\\?\W)((?:(?!\3)(?:\\.|.))*)\3((?:(?!\3)(?:\\.|.))*)\3([gi]*)]) do
      |addr, cmd, sep, pat, rep, opt|
      addr &&= Regexp.new(addr[/\A\/(.*)\/\z/, 1])
      case cmd
      when 's'
        next if pat == '^' and rep.empty?
        exp << [addr, (opt.include?('g') ? :gsub! : :sub!),
                Regexp.new(pat, opt.include?('i')), rep.gsub(/&/){'\&'}]
      when 'y'
        exp << [addr, :tr!, Regexp.quote(pat), rep]
      end
    end
    trans = proc do |base|
      exp.each {|addr, opt, pat, rep| base.__send__(opt, pat, rep) if !addr or addr =~ base}
      base
    end
  elsif /ruby/ =~ ruby_install_name
    trans = proc {|base| ruby_install_name.sub(/ruby/, base)}
  else
    trans = proc {|base| base}
  end
  install_recursive(File.join(srcdir, "bin"), bindir) do |src, cmd|
    cmd = cmd.sub(/[^\/]*\z/m) {|n| RbConfig.expand(trans[n])}

    shebang = ''
    body = ''
    open(src, "rb") do |f|
      shebang = f.gets
      body = f.read
    end
    if PROLOG_SCRIPT
      shebang.sub!(/\A(\#!.*?ruby\b)?/) {PROLOG_SCRIPT + ($1 || "#!ruby\n")}
    else
      shebang.sub!(/\A\#!.*?ruby\b/) {"#!" + ruby_shebang}
    end
    shebang.sub!(/\r$/, '')
    body.gsub!(/\r$/, '')

    cmd << ".#{$cmdtype}" if $cmdtype
    open_for_install(cmd, $script_mode) do
      case $cmdtype
      when "exe"
        stub + shebang + body
      when "bat"
        [<<-"EOH".gsub(/^\s+/, ''), shebang, body, "__END__\n:endofruby\n"].join.gsub(/$/, "\r")
          @echo off
          @if not "%~d0" == "~d0" goto WinNT
          #{ruby_bin} -x "#{cmd}" %1 %2 %3 %4 %5 %6 %7 %8 %9
          @goto endofruby
          :WinNT
          "%~dp0#{ruby_install_name}" -x "%~f0" %*
          @goto endofruby
        EOH
      when "cmd"
        <<"/EOH" << shebang << body
@"%~dp0#{ruby_install_name}" -x "%~f0" %*
@exit /b %ERRORLEVEL%
/EOH
      else
        shebang + body
      end
    end
  end
end

install?(:local, :comm, :lib) do
  prepare "library scripts", rubylibdir
  noinst = %w[README* *.txt *.rdoc]
  install_recursive(File.join(srcdir, "lib"), rubylibdir, :no_install => noinst, :mode => $data_mode)
end

install?(:local, :arch, :lib) do
  prepare "common headers", rubyhdrdir

  noinst = []
  unless RUBY_PLATFORM =~ /mswin|mingw|bccwin/
    noinst << "win32.h"
  end
  noinst = nil if noinst.empty?
  install_recursive(File.join(srcdir, "include"), rubyhdrdir, :no_install => noinst, :glob => "*.h", :mode => $data_mode)
end

install?(:local, :comm, :man) do
  mdocs = Dir["#{srcdir}/man/*.[1-9]"]
  prepare "manpages", mandir, ([] | mdocs.collect {|mdoc| mdoc[/\d+$/]}).sort.collect {|sec| "man#{sec}"}

  mandir = File.join(mandir, "man")
  has_goruby = File.exist?(goruby_install_name+exeext)
  require File.join(srcdir, "tool/mdoc2man.rb") if $mantype != "doc"
  mdocs.each do |mdoc|
    next unless File.file?(mdoc) and open(mdoc){|fh| fh.read(1) == '.'}
    base = File.basename(mdoc)
    if base == "goruby.1"
      next unless has_goruby
    end

    destdir = mandir + (section = mdoc[/\d+$/])
    destname = ruby_install_name.sub(/ruby/, base.chomp(".#{section}"))
    destfile = File.join(destdir, "#{destname}.#{section}")

    if $mantype == "doc"
      install mdoc, destfile, :mode => $data_mode
    else
      class << (w = [])
        alias print push
      end
      open(mdoc) {|r| Mdoc2Man.mdoc2man(r, w)}
      open_for_install(destfile, $data_mode) {w.join("")}
    end
  end
end

# :stopdoc:
module Gem
  if defined?(Specification)
    remove_const(:Specification)
  end

  class Specification < OpenStruct
    def initialize(*)
      super
      yield(self) if defined?(yield)
      self.executables ||= []
    end

    def self.load(path)
      src = File.open(path, "rb") {|f| f.read}
      src.sub!(/\A#.*/, '')
      eval(src, nil, path)
    end
  end
end

module RbInstall
  module Specs
    class Reader < Struct.new(:src)
      def gemspec
        @gemspec ||= begin
          Gem::Specification.load(src) || raise("invalid spec in #{src}")
        end
      end

      def spec_source
        File.read src
      end
    end

    class Generator < Struct.new(:name, :src, :execs)
      def gemspec
        @gemspec ||= eval spec_source
      end

      def spec_source
        <<-GEMSPEC
Gem::Specification.new do |s|
  s.name = #{name.dump}
  s.version = #{version.dump}
  s.summary = "This #{name} is bundled with Ruby"
  s.executables = #{execs.inspect}
end
        GEMSPEC
      end

      private
      def version
        version = open(src) { |f|
          f.find { |s| /^\s*\w*VERSION\s*=(?!=)/ =~ s }
        } or return
        version.split(%r"=\s*", 2)[1].strip[/\A([\'\"])(.*?)\1/, 2]
      end
    end
  end
end
# :startdoc:

install?(:ext, :comm, :gem) do
  $:.unshift(File.join(srcdir, "lib"))
  require("rubygems.rb")
  gem_dir = Gem.default_dir
  directories = Gem.ensure_gem_subdirectories(gem_dir)
  prepare "default gems", gem_dir, directories

  spec_dir = File.join(gem_dir, directories.grep(/^spec/)[0])
  gems = {}
  File.foreach(File.join(srcdir, "defs/default_gems")) do |line|
    line.chomp!
    line.sub!(/\s*#.*/, '')
    next if line.empty?
    words = []
    line.scan(/\G\s*([^\[\]\s]+|\[([^\[\]]*)\])/) do
      words << ($2 ? $2.split : $1)
    end
    name, src, execs = *words
    next unless name and src

    src       = File.join(srcdir, src)
    specgen   = RbInstall::Specs::Generator.new(name, src, execs || [])
    gems[name] ||= specgen
  end

  Dir.glob(srcdir+"/{lib,ext}/**/*.gemspec").each do |src|
    specgen   = RbInstall::Specs::Reader.new(src)
    gems[specgen.gemspec.name] ||= specgen
  end

  gems.sort.each do |name, specgen|
    gemspec   = specgen.gemspec
    full_name = "#{gemspec.name}-#{gemspec.version}"

    puts "#{" "*30}#{gemspec.name} #{gemspec.version}"
    open_for_install(File.join(spec_dir, "#{full_name}.gemspec"), $data_mode) do
      specgen.spec_source
    end

    unless gemspec.executables.empty? then
      bin_dir = File.join(gem_dir, 'gems', full_name, 'bin')
      makedirs(bin_dir)

      execs = gemspec.executables.map {|exec| File.join(srcdir, 'bin', exec)}
      install(execs, bin_dir, :mode => $prog_mode)
    end
  end
end

parse_args()

include FileUtils
include FileUtils::NoWrite if $dryrun
@fileutils_output = STDOUT
@fileutils_label = ''

$install << :local << :ext if $install.empty?
$install.each do |inst|
  if !(procs = $install_procs[inst]) || procs.empty?
    next warn("unknown install target - #{inst}")
  end
  procs.each do |block|
    dir = Dir.pwd
    begin
      block.call
    ensure
      Dir.chdir(dir)
    end
  end
end

# vi:set sw=2:
