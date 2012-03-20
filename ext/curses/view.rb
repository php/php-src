#!/usr/local/bin/ruby

require "curses"
include Curses

#
# main
#

if ARGV.size != 1 then
  printf("usage: view file\n");
  exit
end
begin
  fp = open(ARGV[0], "r")
rescue
  raise "cannot open file: #{ARGV[1]}"
end

# signal(SIGINT, finish)

init_screen
#keypad(stdscr, TRUE)
nonl
cbreak
noecho
#scrollok(stdscr, TRUE)

# slurp the file
data_lines = []
fp.each_line { |l|
  data_lines.push(l)
}
fp.close


lptr = 0
while TRUE
  i = 0
  while i < lines
    setpos(i, 0)
    #clrtoeol
    addstr(data_lines[lptr + i]) #if data_lines[lptr + i]
    i += 1
  end
  refresh

  explicit = FALSE
  n = 0
  while TRUE
    c = getch
    if c =~ /[0-9]/
      n = 10 * n + c.to_i
    else
      break
    end
  end

  n = 1 if !explicit && n == 0

  case c
  when "n"  #when KEY_DOWN
    i = 0
    while i < n
      if lptr + lines < data_lines.size then
	lptr += 1
      else
	break
      end
      i += 1
    end
    #wscrl(i)

  when "p"  #when KEY_UP
    i = 0
    while i < n
      if lptr > 0 then
	lptr -= 1
      else
	break
      end
      i += 1
    end
    #wscrl(-i)

  when "q"
    break
  end

end
close_screen
