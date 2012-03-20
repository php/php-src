#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mb = Tk::Iwidgets::Messagebox.new(:hscrollmode=>:dynamic,
                                  :labeltext=>'Messages', :labelpos=>:n,
                                  :height=>120, :width=>550,
                                  :savedir=>'/tmp', :textbackground=>'#d9d9d9')
mb.pack(:padx=>5, :pady=>5, :fill=>:both, :expand=>true)

mb.type_add('ERROR', :background=>'red', :foreground=>'white', :bell=>true)
mb.type_add('WARNING', :background=>'yellow', :foreground=>'black')
mb.type_add('INFO', :background=>'white', :foreground=>'black')

mb.issue('This is an error message in red with a beep', 'ERROR')
mb.issue('This warning message in yellow', 'WARNING')
mb.issue('This is an informational message', 'INFO')

Tk.mainloop
