#! /bin/ruby
# This needs ruby 1.9 and subversion.
# run this in a repository to commit.

require 'date'
require 'tempfile'

$repos = 'svn+ssh://svn@ci.ruby-lang.org/ruby/'
ENV['LC_ALL'] = 'C'

def help
  puts <<-end
simple backport
  ruby #$0 1234

range backport
  ruby #$0 1234:5678

backport from other branch
  ruby #$0 17502 mvm

revision increment
  ruby #$0 revisionup

tagging
  ruby #$0 tag

* all operations shall be applied to the working directory.
end
end

def version
  v = p = nil
  open 'version.h', 'rb' do |f|
    f.each_line do |l|
      case l
      when /^#define RUBY_VERSION "(\d)\.(\d)\.(\d)"$/
        v = $~.captures
      when /^#define RUBY_PATCHLEVEL (-?\d+)$/
        p = $1
      end
    end
  end
  return v, p
end

def interactive str
  loop do
    yield
    STDERR.puts str
    case STDIN.gets
    when /\Aa/i then exit
    when /\Ar/i then redo
    when /\Ay/i then break
    else exit
    end
  end
end

def version_up
  d = DateTime.now
  d = d.new_offset(Rational(9,24)) # we need server locale (i.e. japanese) time
  system *%w'svn revert version.h'
  v, p = version

  teeny = v[2]
  case v
  when %w/1 9 2/
    teeny = 1
  end

  p = p.to_i
  if p != -1
    p += 1
  end

  str = open 'version.h', 'rb' do |f| f.read end
  [%W[RUBY_VERSION      "#{v.join '.'}"],
   %W[RUBY_VERSION_CODE  #{v.join ''}],
   %W[RUBY_VERSION_MAJOR #{v[0]}],
   %W[RUBY_VERSION_MINOR #{v[1]}],
   %W[RUBY_VERSION_TEENY #{teeny}],
   %W[RUBY_RELEASE_DATE "#{d.strftime '%Y-%m-%d'}"],
   %W[RUBY_RELEASE_CODE  #{d.strftime '%Y%m%d'}],
   %W[RUBY_PATCHLEVEL    #{p}],
   %W[RUBY_RELEASE_YEAR  #{d.year}],
   %W[RUBY_RELEASE_MONTH #{d.month}],
   %W[RUBY_RELEASE_DAY   #{d.day}],
  ].each do |(k, i)|
    str.sub! /^(#define\s+#{k}\s+).*$/, "\\1#{i}"
  end
  str.sub! /\s+\z/m, ''
  fn = sprintf 'version.h.tmp.%032b', rand(1 << 31)
  File.rename 'version.h', fn
  open 'version.h', 'wb' do |f|
    f.puts str
  end
  File.unlink fn
end

def tag intv_p = false
  v, p = version
  x = v.join('_')
  y = $repos + 'branches/ruby_' + x
  z = 'v' + x + '_' + p
  w = $repos + 'tags/' + z
  if intv_p
    interactive "OK? svn cp -m \"add tag #{z}\" #{y} #{w} ([y]es|[a]bort|[r]etry)" do
    end
  end
  system *%w'svn cp -m' + ["add tag #{z}"] + [y, w]
end

def default_merge_branch
  %r{^URL: .*/branches/ruby_1_8_} =~ `svn info` ? 'branches/ruby_1_8' : 'trunk'
end

case ARGV[0]
when "up", /\A(ver|version|rev|revision|lv|level|patch\s*level)\s*up/
  version_up
  system 'svn diff version.h'
when "tag"
  tag :interactive
when nil, "-h", "--help"
  help
  exit
else
  system 'svn up'

  q = $repos + (ARGV[1] || default_merge_branch)
  revs = ARGV[0].split /,\s*/
  log = ''
  log_svn = ''

  revs.each do |rev|
    case rev
    when /\Ar?\d+:r?\d+\z/
      r = ['-r', rev]
    when /\Ar?\d+\z/
      r = ['-c', rev]
    when nil then
      puts "#$0 revision"
      exit
    end

    l = IO.popen %w'svn diff' + r + %w'--diff-cmd=diff -x -pU0' + [File.join(q, 'ChangeLog')] do |f|
      f.read
    end

    log << l
    log_svn << l.lines.grep(/^\+\t/).join.gsub(/^\+/, '').gsub(/^\t\*/, "\n\t\*")

    if log_svn.empty?
      log_svn = IO.popen %w'svn log ' + r + [q] do |f|
        f.read
      end.sub(/\A-+\nr.*\n/, '').sub(/\n-+\n\z/, '').gsub(/^(?=\S)/, "\t")
    end

    a = %w'svn merge --accept=postpone' + r + [q]
    STDERR.puts a.join(' ')

    system(*a)
    system *%w'svn revert ChangeLog' if /^\+/ =~ l
  end

  if /^\+/ =~ log
    system *%w'svn revert ChangeLog'
    IO.popen %w'patch -p0', 'wb' do |f|
      f.write log.gsub(/\+(Mon|Tue|Wed|Thu|Fri|Sat|Sun) (Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) [ 123][0-9] [012][0-9]:[0-5][0-9]:[0-5][0-9] \d\d\d\d/,
                       # this format-time-string was from the file local variables of ChangeLog
                       '+'+Time.now.strftime('%a %b %e %H:%M:%S %Y'))
    end
    system *%w'touch ChangeLog' # needed somehow, don't know why...
  else
    STDERR.puts '*** You should write ChangeLog NOW!!! ***'
  end

  version_up
  f = Tempfile.new 'merger.rb'
  f.printf "merge revision(s) %s:\n", ARGV[0]
  f.write log_svn
  f.flush
  f.close
  f.open # avoid gc

  interactive 'conflicts resolved? (y:yes, a:abort, r:retry, otherwise abort)' do
    f.rewind
    IO.popen('-', 'wb') do |g|
      if g
        g << `svn stat`
        g << "\n\n"
        g << f.read
        g << "\n\n"
        g << `svn diff --diff-cmd=diff -x -upw`
      else
        exec 'less'
      end
    end
    Process.waitall
  end

  if system *%w'svn ci -F' + [f.path]
    # tag :interactive # no longer needed.
    system 'rm -f subversion.commitlog'
  else
    puts 'commit failed; try again.'
  end

  f.close
end
