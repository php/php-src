#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

sc = Tk::Iwidgets::Scrolledcanvas.new

sc.create(TkcRectangle, 100, 100, 400, 400, :fill=>'red')
TkcRectangle.new(sc, 300, 300, 600, 600, :fill=>'green')
TkcRectangle.new(sc, [[200, 200], [500, 500]], :fill=>'blue')

sc.pack(:expand=>true, :fill=>:both, :padx=>10, :pady=>10)

Tk.mainloop
