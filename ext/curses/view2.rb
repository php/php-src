#!/usr/local/bin/ruby

require "curses"


# A curses based file viewer
class FileViewer

  # Create a new fileviewer, and view the file.
  def initialize(filename)
    @data_lines = []
    @screen = nil
    @top = nil
    init_curses
    load_file(filename)
    interact
  end

  # Perform the curses setup
  def init_curses
    # signal(SIGINT, finish)

    Curses.init_screen
    Curses.nonl
    Curses.cbreak
    Curses.noecho

    @screen = Curses.stdscr

    @screen.scrollok(true)
    #$screen.keypad(true)
  end

  # Load the file into memory, and put
  # the first part on the curses display.
  def load_file(filename)
    fp = open(filename, "r") do |fp|
    # slurp the file
    fp.each_line { |l|
      @data_lines.push(l.chop)
    }
    end
    @top = 0
    @data_lines[0..@screen.maxy-1].each_with_index{|line, idx|
      @screen.setpos(idx, 0)
      @screen.addstr(line)
    }
    @screen.setpos(0,0)
    @screen.refresh
  rescue
    raise "cannot open file '#{filename}' for reading"
  end


  # Scroll the display up by one line
  def scroll_up
    if( @top > 0 )
      @screen.scrl(-1)
      @top -= 1
      str = @data_lines[@top]
      if( str )
        @screen.setpos(0, 0)
        @screen.addstr(str)
      end
      return true
    else
      return false
    end
  end

  # Scroll the display down by one line
  def scroll_down
    if( @top + @screen.maxy < @data_lines.length )
      @screen.scrl(1)
      @top += 1
      str = @data_lines[@top + @screen.maxy - 1]
      if( str )
        @screen.setpos(@screen.maxy - 1, 0)
        @screen.addstr(str)
      end
      return true
    else
      return false
    end
  end

  # Allow the user to interact with the display.
  # This uses EMACS-like keybindings, and also
  # vi-like keybindings as well, except that left
  # and right move to the beginning and end of the
  # file, respectively.
  def interact
    while true
      result = true
      c = Curses.getch
      case c
      when Curses::KEY_DOWN, Curses::KEY_CTRL_N, ?j
        result = scroll_down
      when Curses::KEY_UP, Curses::KEY_CTRL_P, ?k
        result = scroll_up
      when Curses::KEY_NPAGE, ?\s  # white space
        for i in 0..(@screen.maxy - 2)
          if( ! scroll_down )
            if( i == 0 )
              result = false
            end
            break
          end
        end
      when Curses::KEY_PPAGE
        for i in 0..(@screen.maxy - 2)
          if( ! scroll_up )
            if( i == 0 )
              result = false
            end
            break
          end
        end
      when Curses::KEY_LEFT, Curses::KEY_CTRL_T, ?h
        while( scroll_up )
        end
      when Curses::KEY_RIGHT, Curses::KEY_CTRL_B, ?l
        while( scroll_down )
        end
      when ?q
        break
      else
        @screen.setpos(0,0)
        @screen.addstr("[unknown key `#{Curses.keyname(c)}'=#{c}] ")
      end
      if( !result )
        Curses.beep
      end
      @screen.setpos(0,0)
    end
    Curses.close_screen
  end
end


# If we are being run as a main program...
if __FILE__ == $0
  if ARGV.size != 1 then
    printf("usage: #{$0} file\n");
    exit
  end

  viewer = FileViewer.new(ARGV[0])
end
