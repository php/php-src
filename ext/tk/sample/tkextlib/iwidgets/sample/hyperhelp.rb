#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mainloop = Thread.new{Tk.mainloop}

dir  = '/usr/local/ActiveTcl/demos/IWidgets/html/'
href = [ 'hyperhelp.n', 'buttonbox.n', 'calendar.n' ]

hh = Tk::Iwidgets::Hyperhelp.new(:topics=>href, :helpdir=>dir)
hh.show_topic('hyperhelp.n')
hh.activate

mainloop.join
