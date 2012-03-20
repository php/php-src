#! /usr/local/bin/ruby
# convert ls-lR filename into fullpath.

if ARGV[0] =~ /-p/
  ARGV.shift
  path = ARGV.shift
end

if path == nil
  path = ""
elsif path !~ %r|/$|
  path += "/"
end

while line = gets()
  case line
  when /:$/
    path = line.chop.chop + "/"
  when /^total/, /^d/
  when /^(.*\d )(.+)$/
    print($1, path, $2, "\n")
  end
end
