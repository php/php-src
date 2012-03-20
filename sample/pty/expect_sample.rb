#
# sample program of expect.rb
#
#  by A. Ito
#
#  This program reports the latest version of ruby interpreter
#  by connecting to ftp server at ruby-lang.org.
#
require 'pty'
require 'expect'

fnames = []
PTY.spawn("ftp ftp.ruby-lang.org") do |r_f,w_f,pid|
  w_f.sync = true

  $expect_verbose = false

  if !ENV['USER'].nil?
    username = ENV['USER']
  elsif !ENV['LOGNAME'].nil?
    username = ENV['LOGNAME']
  else
    username = 'guest'
  end

  r_f.expect(/^(Name).*: |(word):|> /) do
    w_f.puts($1 ? "ftp" : $2 ? "#{username}@" : "cd pub/ruby")
  end
  r_f.expect("> ") do
    w_f.print "dir\n"
  end

  r_f.expect(/[^\-]> /) do |output|
    for x in output[0].split("\n")
      if x =~ /(ruby.*?\.tar\.gz)/ then
         fnames.push $1
      end
    end
  end
  begin
    w_f.print "quit\n"
  rescue
  end
end

print "The latest ruby interpreter is "
print fnames.sort.pop
print "\n"
