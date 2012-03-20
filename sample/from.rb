#! /usr/local/bin/ruby

require "time"
require "kconv"

class String
  def kjust(len)
    res = ''
    rlen = 0
    self.each_char do |char|
      delta = char.bytesize > 1 ? 2 : 1
      break if rlen + delta > len
      rlen += delta
      res += char
    end
    res += ' ' * (len - rlen) if rlen < len
    res
  end
end

def fromout(date, from, subj)
  return 0 if !date
  y, m, d = Time.parse(date).to_a.reverse[4, 3] if date
  y ||= 0; m ||= 0; d ||= 0
  from ||= "sombody@somewhere"
  from.delete!("\r\n")
  from = from.kconv(Encoding.default_external).kjust(28)
  subj ||= "(nil)"
  subj.delete!("\r\n")
  subj = subj.kconv(Encoding.default_external).kjust(40)
  printf "%02d/%02d/%02d [%s] %s\n", y%100, m, d, from, subj
  return 1
end

def get_mailfile(user)
  file = user
  unless user
    file = ENV['MAIL']
    user = ENV['USER'] || ENV['USERNAME'] || ENV['LOGNAME']
  end

  if file == nil or !File.exist?(file)
    [ENV['SPOOLDIR'], '/usr/spool', '/var/spool', '/usr', '/var'].each do |m|
      path = "#{m}/mail/#{user}"
      if File.exist?(path)
	file = path
	break
      end
    end
  end
  file
end

def from_main
  if ARGV[0] == '-w'
    wait = true
    ARGV.shift
  end
  file = get_mailfile(ARGV[0])

  outcount = 0
  if File.exist?(file)
    atime = File.atime(file)
    mtime = File.mtime(file)
    open(file, "r") do |f|
      until f.eof?
	header = {}
	f.each_line do |line|
	  next if /^From / =~ line # skip From-line
	  break if /^$/ =~ line	 # end of header

	  if /^(?<attr>\S+?):\s*(?<value>.*)/ =~ line
	    attr.capitalize!
	    header[attr] = value
	  elsif attr
	    header[attr] += "\n" + line.lstrip
	  end
	end

	f.each_line do |line|
	  break if /^From / =~ line
	end
	outcount += fromout(header['Date'], header['From'], header['Subject'])
      end
    end
    File.utime(atime, mtime, file)
  end

  if outcount == 0
    print "You have no mail.\n"
    sleep 2 if wait
  elsif wait
    system "stty cbreak -echo"
    $stdin.getc
    system "stty cooked echo"
  end
end

if __FILE__ == $0
  from_main
end

__END__

=begin

= from.rb

== USAGE

ruby from.rb [-w] [username_or_filename]

=end
