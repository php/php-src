#
# cgi_runner.rb -- CGI launcher.
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000 TAKAHASHI Masayoshi, GOTOU YUUZOU
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: cgi_runner.rb,v 1.9 2002/09/25 11:33:15 gotoyuzo Exp $

def sysread(io, size)
  buf = ""
  while size > 0
    tmp = io.sysread(size)
    buf << tmp
    size -= tmp.bytesize
  end
  return buf
end

STDIN.binmode

len = sysread(STDIN, 8).to_i
out = sysread(STDIN, len)
STDOUT.reopen(open(out, "w"))

len = sysread(STDIN, 8).to_i
err = sysread(STDIN, len)
STDERR.reopen(open(err, "w"))

len  = sysread(STDIN, 8).to_i
dump = sysread(STDIN, len)
hash = Marshal.restore(dump)
ENV.keys.each{|name| ENV.delete(name) }
hash.each{|k, v| ENV[k] = v if v }

dir = File::dirname(ENV["SCRIPT_FILENAME"])
Dir::chdir dir

if ARGV[0]
  argv = ARGV.dup
  argv << ENV["SCRIPT_FILENAME"]
  exec(*argv)
  # NOTREACHED
end
exec ENV["SCRIPT_FILENAME"]
