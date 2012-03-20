#!/usr/local/bin/ruby

require "curses"
include Curses

def show_message(message)
  width = message.length + 6
  win = Window.new(5, width,
		   (lines - 5) / 2, (cols - width) / 2)
  win.box(?|, ?-)
  win.setpos(2, 3)
  win.addstr(message)
  win.refresh
  win.getch
  win.close
end

init_screen
begin
  crmode
#  show_message("Hit any key")
  setpos((lines - 5) / 2, (cols - 10) / 2)
  addstr("Hit any key")
  refresh
  getch
  show_message("Hello, World!")
  refresh
ensure
  close_screen
end
