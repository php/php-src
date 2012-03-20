#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

Thread.new{Tk.mainloop}

d = Tk::Iwidgets::Dialog.new(:modality=>:application)

d.buttonconfigure('OK', :command=>proc{puts 'OK'; d.deactivate true})
d.buttonconfigure('Apply', :command=>proc{puts 'Apply'})
d.buttonconfigure('Cancel', :command=>proc{puts 'Cancel'; d.deactivate false})
d.buttonconfigure('Help', :command=>proc{puts 'Help'})

TkListbox.new(d.child_site, :relief=>:sunken).pack(:expand=>true, :fill=>:both)

if TkComm.bool(d.activate)
  puts "Exit via OK button"
else
  puts "Exit via Cancel button"
end
