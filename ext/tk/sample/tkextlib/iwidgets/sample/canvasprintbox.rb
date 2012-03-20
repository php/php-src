#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Tk::Iwidgets::Canvasprintbox.new(:orient=>:landscape, :stretch=>1) \
  .pack(:padx=>10, :pady=>10, :fill=>:both, :expand=>true)

Tk.mainloop
