#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

lw = Tk::Iwidgets::Labeledwidget.new(:labeltext=>'Canvas Widget',
                                     :labelpos=>:s)
lw.pack(:fill=>:both, :expand=>true, :padx=>10, :pady=>10)

cw = TkCanvas.new(lw.child_site, :relief=>:raised, :width=>200, :height=>200,
                  :borderwidth=>3, :background=>:white)
cw.pack(:padx=>10, :pady=>10)

Tk.mainloop
