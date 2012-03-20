#!/usr/bin/env ruby

ENV.delete('PWD')

require 'optparse'

unless File.respond_to? :realpath
  require 'pathname'
  def File.realpath(arg)
    Pathname(arg).realpath.to_s
  end
end

Program = $0

class VCS
  class NotFoundError < RuntimeError; end

  @@dirs = []
  def self.register(dir)
    @@dirs << [dir, self]
  end

  def self.detect(path)
    @@dirs.each do |dir, klass|
      return klass.new(path) if File.directory?(File.join(path, dir))
      prev = path
      loop {
        curr = File.realpath(File.join(prev, '..'))
        break if curr == prev	# stop at the root directory
        return klass.new(path) if File.directory?(File.join(curr, dir))
        prev = curr
      }
    end
    raise VCS::NotFoundError, "does not seem to be under a vcs: #{path}"
  end

  def initialize(path)
    @srcdir = path
    super()
  end

  # return a pair of strings, the last revision and the last revision in which
  # +path+ was modified.
  def get_revisions(path)
    path = relative_to(path)
    last, changed, *rest = Dir.chdir(@srcdir) {self.class.get_revisions(path)}
    last or raise "last revision not found"
    changed or raise "changed revision not found"
    return last, changed, *rest
  end

  def relative_to(path)
    if path
      srcdir = File.realpath(@srcdir)
      path = File.realpath(path)
      list1 = srcdir.split(%r{/})
      list2 = path.split(%r{/})
      while !list1.empty? && !list2.empty? && list1.first == list2.first
        list1.shift
        list2.shift
      end
      if list1.empty? && list2.empty?
        "."
      else
        ([".."] * list1.length + list2).join("/")
      end
    else
      '.'
    end
  end

  class SVN < self
    register(".svn")

    def self.get_revisions(path)
      begin
        nulldevice = %w[/dev/null NUL NIL: NL:].find {|dev| File.exist?(dev)}
        if nulldevice
          save_stderr = STDERR.dup
          STDERR.reopen nulldevice, 'w'
        end
        info_xml = `svn info --xml "#{path}"`
      ensure
        if save_stderr
          STDERR.reopen save_stderr
          save_stderr.close
        end
      end
      _, last, _, changed, _ = info_xml.split(/revision="(\d+)"/)
      [last, changed]
    end
  end

  class GIT < self
    register(".git")

    def self.get_revisions(path)
      logcmd = %Q[git log -n1 --grep="^ *git-svn-id: .*@[0-9][0-9]* "]
      idpat = /git-svn-id: .*?@(\d+) \S+\Z/
      last = `#{logcmd}`[idpat, 1]
      changed = path ? `#{logcmd} "#{path}"`[idpat, 1] : last
      [last, changed]
    end
  end
end

@output = nil
def self.output=(output)
  if @output and @output != output
    raise "you can specify only one of --changed, --revision.h and --doxygen"
  end
  @output = output
end
@suppress_not_found = false

srcdir = nil
parser = OptionParser.new {|opts|
  opts.on("--srcdir=PATH", "use PATH as source directory") do |path|
    srcdir = path
  end
  opts.on("--changed", "changed rev") do
    self.output = :changed
  end
  opts.on("--revision.h", "RUBY_REVISION macro") do
    self.output = :revision_h
  end
  opts.on("--doxygen", "Doxygen format") do
    self.output = :doxygen
  end
  opts.on("-q", "--suppress_not_found") do
    @suppress_not_found = true
  end
}
parser.parse! rescue abort "#{File.basename(Program)}: #{$!}\n#{parser}"

srcdir ||= File.dirname(File.dirname(Program))
begin
  vcs = VCS.detect(srcdir)
rescue VCS::NotFoundError => e
  abort "#{File.basename(Program)}: #{e.message}" unless @suppress_not_found
else
  begin
    last, changed = vcs.get_revisions(ARGV.shift)
  rescue => e
    abort "#{File.basename(Program)}: #{e.message}" unless @suppress_not_found
    exit false
  end
end

case @output
when :changed, nil
  puts changed
when :revision_h
  puts "#define RUBY_REVISION #{changed.to_i}"
when :doxygen
  puts "r#{changed}/r#{last}"
else
  raise "unknown output format `#{@output}'"
end
