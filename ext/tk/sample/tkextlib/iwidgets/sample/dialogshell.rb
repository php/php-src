#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

ds = Tk::Iwidgets::Dialogshell.new(:modality=>:none)

ds.add('OK', :text=>'OK', :command=>proc{puts 'OK'; ds.deactivate})
ds.add('Cancel', :text=>'Cancel', :command=>proc{puts 'Cancel'; ds.deactivate})
ds.default('OK')

TkButton.new(:text=>'ACTIVATE', :padx=>7, :pady=>7,
             :command=>proc{puts ds.activate}).pack(:padx=>10, :pady=>10)

Tk.mainloop
