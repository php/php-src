require 'pty'

if ARGV.size == 0 then
  ofile = "typescript"
else
  ofile = ARGV[0]
end

logfile = File.open(ofile,"a")

system "stty -echo raw lnext ^_"

PTY.spawn("/bin/csh") do |r_pty,w_pty,pid|

  Thread.new do
    while true
      w_pty.print STDIN.getc.chr
      w_pty.flush
    end
  end

  begin
    while true
      c = r_pty.sysread(512)
      break if c.nil?
      print c
      STDOUT.flush
      logfile.print c
    end
  rescue
  #  print $@,':',$!,"\n"
    logfile.close
  end
end

system "stty echo -raw lnext ^v"

