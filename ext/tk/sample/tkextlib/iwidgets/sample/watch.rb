#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Thread.new{
  trap('INT') {puts 'catch SIGINT'}
  sleep 5
  trap('INT', 'DEFAULT')
}

Tk::Iwidgets::Watch.new(:state=>:disabled, :showampm=>:no,
                        :width=>155, :height=>155){|w|
  w.pack(:padx=>10, :pady=>10, :fill=>:both, :expand=>true)
  # TkTimer.new(1000, -1, proc{w.show; Tk.update}).start
  TkTimer.new(25, -1, proc{w.show; Tk.update}).start
}

Tk.mainloop
