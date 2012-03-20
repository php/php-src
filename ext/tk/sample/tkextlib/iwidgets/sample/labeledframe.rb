#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

lf = Tk::Iwidgets::Labeledframe.new(:labeltext=>'Entry Frame', :labelpos=>:n)
lf.pack(:fill=>:both, :expand=>true, :padx=>10, :pady=>10)

cs = lf.child_site

Tk::Iwidgets::Entryfield.new(cs, :labeltext=>'Name:').pack(:side=>:top, :fill=>:x)
Tk::Iwidgets::Spinint.new(cs, :labeltext=>'Number:').pack(:side=>:top, :fill=>:x)
Tk::Iwidgets::Pushbutton.new(cs, :text=>'Details:').pack(:side=>:top, :fill=>:x)

Tk.mainloop
