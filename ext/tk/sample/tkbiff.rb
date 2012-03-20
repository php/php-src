#!/usr/bin/env ruby

if ARGV[0] != '-d'
  unless $DEBUG
    exit if fork
  end
else
  ARGV.shift
end

if ARGV.length == 0
  if ENV['MAIL']
    $spool = ENV['MAIL']
  else
    $spool = '/var/spool/mail/' + ENV['USER']
  end
else
  $spool = ARGV[0]
end

require "parsedate"
require "base64"

include ParseDate

class Mail
  def Mail.new(f)
    if !f.kind_of?(IO)
      f = open(f, "r")
      me = super
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
      line.chop!
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

require "tkscrollbox"

my_appname = Tk.appname('tkbiff')
$top = TkRoot.new
if ((TkWinfo.interps($top) - [my_appname]).find{|ip| ip =~ /^tkbiff/})
  STDERR.print("Probably other 'tkbiff's are running. Bye.\n")
  exit
end

$top.withdraw
$list = TkScrollbox.new($top) {
  relief 'raised'
  width 80
  height 8
  setgrid 'yes'
  pack
}
TkButton.new($top) {
  text 'Dismiss'
  command proc {$top.withdraw}
  pack('fill'=>'both','expand'=>'yes')
}
$top.bind "Control-c", proc{exit}
$top.bind "Control-q", proc{exit}
$top.bind "space", proc{exit}

$spool_size = 0
$check_time = Time.now

def check
  $check_time = Time.now
  size = File.size($spool)
  if size and size != $spool_size
    $spool_size = size
    pop_up if size > 0
  end
  Tk.after 5000, proc{check}
end

if defined? Thread
  Thread.start do
    loop do
      sleep 600
      if Time.now - $check_time > 200
        Tk.after 5000, proc{check}
      end
    end
  end
end

def pop_up
  outcount = 0;
  $list.delete 0, 'end'
  f = open($spool, "r")
  while !f.eof?
    mail = Mail.new(f)
    date, from, subj =  mail.header['Date'], mail.header['From'], mail.header['Subject']
    next if !date
    y = m = d = 0
    y, m, d = parsedate(date) if date
    from = "sombody@somewhere" if ! from
    subj = "(nil)" if ! subj
    from = decode_b(from)
    subj = decode_b(subj)
    $list.insert 'end', format('%-02d/%02d/%02d [%-28.28s] %s',y,m,d,from,subj)
    outcount += 1
  end
  f.close
  if outcount == 0
    $list.insert 'end', "You have no mail."
  else
    $list.see 'end'
  end
  $top.deiconify
  Tk.after 2000, proc{$top.iconify}
end

$list.insert 'end', "You have no mail."
check
Tk.after 2000, proc{$top.iconify}
begin
  Tk.mainloop
rescue
  `echo #$! > /tmp/tkbiff`
end
