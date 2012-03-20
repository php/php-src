#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Calendar.new(:command=>proc{|arg| puts(arg.date)},
                           :weekendbackground=>'mistyrose',
                           :weekdaybackground=>'ghostwhite',
                           :outline=>'black', :startday=>'wednesday',
                           :days=>%w(We Th Fr Sa Su Mo Tu)).pack
Tk.mainloop
