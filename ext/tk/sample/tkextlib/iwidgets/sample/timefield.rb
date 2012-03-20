#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

tf = Tk::Iwidgets::Timefield.new(:command=>proc{puts(tf.get)})
tf.pack(:fill=>:x, :expand=>true, :padx=>10,  :pady=>10)

Tk.mainloop
