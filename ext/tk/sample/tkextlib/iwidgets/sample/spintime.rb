#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Spintime.new.pack(:padx=>10, :pady=>10)

Tk.mainloop
