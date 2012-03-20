# ioctl example works on Sun

CBREAK = 0x00000002
ECHO = 0x00000008
TIOCGETP = 0x40067408
TIOCSETP = 0x80067409

def cbreak ()
  set_cbreak(true)
end

def cooked ()
  set_cbreak(false)
end

def set_cbreak (on)
  tty = "\0" * 256
  STDIN.ioctl(TIOCGETP, tty)
  ttys = tty.unpack("C4 S")
  if on
    ttys[4] |= CBREAK
    ttys[4] &= ~ECHO
  else
    ttys[4] &= ~CBREAK
    ttys[4] |= ECHO
  end
  tty = ttys.pack("C4 S")
  STDIN.ioctl(TIOCSETP, tty)
end
cbreak();

print("this is no-echo line: ");
readline().print
cooked();
print("this is echo line: ");
readline()
