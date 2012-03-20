#!/usr/bin/env ruby

require "parsedate"
require "base64"

include ParseDate

class Mail
  def Mail.new(f)
    if !f.kind_of?(IO)
      f = open(f, "r")
      me = super(f)
      f.close
    else
      me = super
    end
    return me
  end

  def initialize(f)
    @header = {}
    @body = []
    while line = f.gets()
      $_.chop!
      next if /^From / =~ line  # skip From-line
      break if /^$/ =~ line     # end of header
      if /^(\S+):\s*(.*)/ =~ line
        @header[attr = $1.capitalize] = $2
      elsif attr
        sub(/^\s*/, '')
        @header[attr] += "\n" + $_
      end
    end

    return unless $_

    while line = f.gets()
      break if /^From / =~ line
      @body.push($_)
    end
  end

  def header
    return @header
  end

  def body
    return @body
  end

end

if ARGV.length == 0
  if ENV['MAIL']
    ARGV[0] = ENV['MAIL']
  elsif ENV['USER']
    ARGV[0] = '/var/spool/mail/' + ENV['USER']
  elsif ENV['LOGNAME']
    ARGV[0] = '/var/spool/mail/' + ENV['LOGNAME']
  end
end

require "tk"
list = scroll = nil
TkFrame.new{|f|
  list = TkListbox.new(f) {
    yscroll proc{|*idx|
        scroll.set *idx
    }
    relief 'raised'
#    geometry "80x5"
    width 80
    height 5
    setgrid 'yes'
    pack('side'=>'left','fill'=>'both','expand'=>'yes')
  }
  scroll = TkScrollbar.new(f) {
    command proc{|idx|
      list.yview *idx
    }
    pack('side'=>'right','fill'=>'y')
  }
  pack
}
root = Tk.root
TkButton.new(root) {
  text 'Dismiss'
  command proc {exit}
  pack('fill'=>'both','expand'=>'yes')
}
root.bind "Control-c", proc{exit}
root.bind "Control-q", proc{exit}
root.bind "space", proc{exit}

$outcount = 0;
for file in ARGV
  next unless File.exist?(file)
  atime = File.atime(file)
  mtime = File.mtime(file)
  f = open(file, "r")
  begin
    until f.eof
      mail = Mail.new(f)
      date = mail.header['Date']
      next unless date
      from = mail.header['From']
      subj = mail.header['Subject']
      y = m = d = 0
      y, m, d = parsedate(date) if date
      from = "sombody@somewhere" unless from
      subj = "(nil)" unless subj
      from = decode_b(from)
      subj = decode_b(subj)
      list.insert 'end', format('%-02d/%02d/%02d [%-28.28s] %s',y,m,d,from,subj)
      $outcount += 1
    end
  ensure
    f.close
    File.utime(atime, mtime, file)
    list.see 'end'
  end
end

limit = 10000
if $outcount == 0
  list.insert 'end', "You have no mail."
  limit = 2000
end
Tk.after limit, proc{
  exit
}
Tk.mainloop
