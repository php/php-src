#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mainloop = Thread.new{Tk.mainloop}

st = Tk::Iwidgets::Scrolledtext.new.pack
st.insert('end', "Now is the time for all good men\n")
st.insert('end', "to come to the aid of their country")

fd = Tk::Iwidgets::Finddialog.new(:textwidget=>st)
fd.center(st)
fd.activate

mainloop.join
