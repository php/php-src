#
#  old-fashioned 'shl' like program
#  by A. Ito
#
#  commands:
#     c        creates new shell
#     C-z      suspends shell
#     p        lists all shell
#     0,1,...  choose shell
#     q        quit

require 'pty'

$shells = []
$n_shells = 0

$r_pty = nil
$w_pty = nil

def writer
  system "stty -echo raw"
  begin
    while true
      c = STDIN.getc
      if c == 26 then # C-z
        $reader.raise(nil)
        return 'Suspend'
      end
      $w_pty.print c.chr
      $w_pty.flush
    end
  rescue
    $reader.raise(nil)
    return 'Exit'
  ensure
    system "stty echo -raw"
  end
end

$reader = Thread.new {
  while true
    begin
      next if $r_pty.nil?
      c = $r_pty.getc
      if c.nil? then
        Thread.stop
      end
      print c.chr
      STDOUT.flush
    rescue
      Thread.stop
    end
  end
}

# $reader.raise(nil)


while true
  print ">> "
  STDOUT.flush
  case gets
  when /^c/i
    $shells[$n_shells] = PTY.spawn("/bin/csh")
    $r_pty,$w_pty = $shells[$n_shells]
    $n_shells += 1
    $reader.run
    if writer == 'Exit'
      $n_shells -= 1
      $shells[$n_shells] = nil
    end
  when /^p/i
    for i in 0..$n_shells
      unless $shells[i].nil?
        print i,"\n"
      end
    end
  when /^([0-9]+)/
    n = $1.to_i
    if $shells[n].nil?
      print "\##{i} doesn't exist\n"
    else
      $r_pty,$w_pty = $shells[n]
      $reader.run
      if writer == 'Exit' then
        $shells[n] = nil
      end
    end
  when /^q/i
    exit
  end
end
