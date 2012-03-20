#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mb = Tk::Iwidgets::Messagebox.new(:hscrollmode=>:dynamic,
                                  :labeltext=>'Messages', :labelpos=>:n,
                                  :height=>120, :width=>550,
                                  :savedir=>'/tmp', :textbackground=>'#d9d9d9')
mb.pack(:padx=>5, :pady=>5, :fill=>:both, :expand=>true)

error   = mb.type_add(:background=>'red', :foreground=>'white', :bell=>true)
warning = mb.type_add(:background=>'yellow', :foreground=>'black')
info    = mb.type_add(:background=>'white', :foreground=>'black')

mb.issue('This is an error message in red with a beep', error)
mb.issue('This warning message in yellow', warning)
mb.issue('This is an informational message', info)

Tk.mainloop
